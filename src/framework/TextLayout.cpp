#include <TextLayout.h>
#include <FileSystem.h>
#include <GpuProgram.h>
#include <GpuProgramConstants.h>
#include <glm/gtc/matrix_transform.hpp> // for ortho()

namespace Acidrain {

    string wrapText(const string& text, int lineMaxWidth, Font* font) {
        string newText;
        lineMaxWidth = (int) (lineMaxWidth * TEXT_DOWNSAMPLE_FACTOR);
        int lineWidth = 0;
        bool hasLastSpace = false;
        int charactersSinceLastBreak = 0;
        for (auto it = text.begin(); it != text.end(); it++) {
            char characterToRender = *it;
            newText.push_back(characterToRender);

            if (characterToRender == '\n') {
                lineWidth = 0;
                hasLastSpace = false;
                charactersSinceLastBreak = 0;
            } else if (characterToRender == ' ') {
                hasLastSpace = true;
                charactersSinceLastBreak = 0;
                lineWidth += font->getCharWidth(characterToRender);
            } else if (font->hasChar(characterToRender)) {
                charactersSinceLastBreak++;
                int charWidth = font->getCharWidth(characterToRender);
                if (((charWidth + lineWidth) > lineMaxWidth) && hasLastSpace) {
                    newText = newText.substr(0, newText.length() - charactersSinceLastBreak);
                    newText.push_back('\n');
                    it -= charactersSinceLastBreak; // because of for loop postincrement
                    lineWidth = 0;
                    hasLastSpace = false;
                    charactersSinceLastBreak = 0;
                } else {
                    lineWidth += charWidth;
                }
            }
        }
        return newText;
    }

    TextLayout::TextLayout(const string& textToRender, Box constraint, Font* fnt)
            : font(fnt), textBox(constraint) {

        // initialize shader
        gpuProgram = make_shared<GpuProgram>(
                FILESYS.getFileContent("shaders/font.vs.glsl"),
                FILESYS.getFileContent("shaders/font.ps.glsl")
        );

        gpuProgramConstantBundle = make_shared<GpuProgramConstantBundle>();

        gpuProgramConstantBundle->set("orthoMatrix",
                                      GpuProgramConstant(ortho(0.0f, 1024.0f * TEXT_DOWNSAMPLE_FACTOR, 768.0f * TEXT_DOWNSAMPLE_FACTOR, 0.0f, 0.0f, 1.0f)));

        int textureSamplerIndex = 0;
        gpuProgramConstantBundle->set("diffuseSampler", GpuProgramConstant(textureSamplerIndex));

        gpuProgram->addConstants(gpuProgramConstantBundle.get());


        buildVbo(textToRender);
    }

    TextLayout::~TextLayout() {
    }

    void TextLayout::buildVbo(const string& text) {
        string textToRender = wrapText(text, static_cast<int>(textBox.width()), font);

        penPosition = textBox.tl + vec2(0, (font->metrics().ascent - font->atlas().padding()) / TEXT_DOWNSAMPLE_FACTOR);
        penPosition.x = round(penPosition.x * TEXT_DOWNSAMPLE_FACTOR);
        penPosition.y = round(penPosition.y * TEXT_DOWNSAMPLE_FACTOR);
        previousChar = 0;
        vbo.empty();
        for (auto it = textToRender.begin(); it != textToRender.end(); it++) {
            char charToRender = *it;
            if (charToRender == '\n') {
                penPosition.y += font->metrics().lineSkip;
                penPosition.x = textBox.tl.x * TEXT_DOWNSAMPLE_FACTOR;
                previousChar = 0;
            } else if (font->hasChar(charToRender)) {
                addCharToVbo(charToRender);
            } else {
                // non printable character. do nothing
            }
        }
    }

    void TextLayout::addCharToVbo(const char& charToRender) {
        const GlyphInfo& glyphInfo = font->atlas().glyph(charToRender);

        if (previousChar != 0) {
            penPosition.x += font->getCharWidth(previousChar);
            penPosition.x += font->getKerning(previousChar, charToRender);
        } else {
            penPosition.x -= font->atlas().padding();
        }

        float penX = penPosition.x;
        float penY = penPosition.y;
        vector<vec2> verts = {
                {penX + glyphInfo.bitmapLeft,               penY - glyphInfo.bitmapTop},
                {penX + glyphInfo.bitmapLeft + glyphInfo.w, penY - glyphInfo.bitmapTop},
                {penX + glyphInfo.bitmapLeft + glyphInfo.w, penY + glyphInfo.h - glyphInfo.bitmapTop},
                {penX + glyphInfo.bitmapLeft,               penY + glyphInfo.h - glyphInfo.bitmapTop}
        };

        double textureXNormalizationFactor = 1.0 / (double) font->atlas().width();
        double textureYNormalizationFactor = 1.0 / (double) font->atlas().height();
        double x, y, w, h;
        x = glyphInfo.x * textureXNormalizationFactor;
        y = glyphInfo.y * textureYNormalizationFactor;
        w = glyphInfo.w * textureXNormalizationFactor;
        h = glyphInfo.h * textureYNormalizationFactor;

        vector<vec2> texs = {
                {x,     y},
                {x + w, y},
                {x + w, y + h},
                {x,     y + h}
        };

        vbo.addQuad(verts, texs, vec4(1, 1, 1, 1));
        previousChar = charToRender;
    }

    const Box& TextLayout::getTextBox() const {
        return textBox;
    }

    void TextLayout::render() const {
        vbo.draw();
    }

    void TextLayout::render(int howManyChars) const {
        // TODO: make it possible to also provide a from/to range (mind the buffering in the vbo)
        vbo.draw(howManyChars * 2); // 2 triangles per char
    }

    void TextLayout::render(vec4 color, vec4 outlineColor) {
        gpuProgramConstantBundle->set("textDiffuseColor", GpuProgramConstant(color));
        gpuProgramConstantBundle->set("textOutlineColor", GpuProgramConstant(outlineColor));

        gpuProgram->use();
        font->texture()->useForUnit(0);
        render();
        gpuProgram->unuse();
    }

    void TextLayout::renderAt(vec4 color, vec4 outlineColor, float x, float y) const {
        gpuProgramConstantBundle->set("textDiffuseColor", GpuProgramConstant(color));
        gpuProgramConstantBundle->set("textOutlineColor", GpuProgramConstant(outlineColor));

        mat4 orthoMatrix = ortho(0.0f, 1024.0f * TEXT_DOWNSAMPLE_FACTOR, 768.0f * TEXT_DOWNSAMPLE_FACTOR, 0.0f, 0.0f, 1.0f);
        orthoMatrix = translate(orthoMatrix, vec3(x * TEXT_DOWNSAMPLE_FACTOR, y * TEXT_DOWNSAMPLE_FACTOR, 0));

        gpuProgramConstantBundle->set("orthoMatrix", GpuProgramConstant(orthoMatrix));

        gpuProgram->use();
        font->texture()->useForUnit(0);
        render();
        gpuProgram->unuse();
    }
}