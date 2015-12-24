#include <GameStateTest.h>
#include <Stardust.h>
#include <Dialog.h>
#include <Font.h>
#include <GfxSystem.h>
#include <AudioSystem.h>
#include <InputProvider.h>

#include <luaconf.h>
#include <lua.hpp>
#include <FileSystem.h>
#include <easylogging++.h>
#include <glm/ext.hpp>
#include <DialogRepository.h>
#include <Script.h>
#include <SDL_image.h>
#include <GpuProgram.h>
#include <GpuProgramConstants.h>
#include <Vbo.h>
#include <FpsCounter.h>

#include <ft2build.h>
#include <ftglyph.h>
#include <ftoutln.h>
#include <fttrigon.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_STROKER_H
#include FT_LCD_FILTER_H
#include FT_GLYPH_H
#include FT_TRUETYPE_IDS_H

#define FT_FLOOR(X) ((X & -64) / 64)
#define FT_CEIL(X)  (((X + 63) & -64) / 64)


#define CHECK_GL_ERROR() _check_gl_error(__FILE__,__LINE__)

namespace Acidrain {

    static Script* script;
    static shared_ptr<GpuProgram> gpuProgram;
    static shared_ptr<GpuProgramConstantBundle> gpuProgramConstantBundle;
    static shared_ptr<Texture> fontTexture;
    static shared_ptr<Texture> bigFontTexture;
    static FT_Library library;

    struct GlyphInfo {
        int x;
        int y;
        int w;
        int h;
        int advance;
        int bitmapLeft;
        int bitmapTop;
    };

    struct GlyphAtlas {

        GlyphAtlas() {
            atlasTexture = new unsigned char[atlasWidth * atlasHeight * 4];
            memset(atlasTexture, 0, (size_t) (atlasWidth * atlasHeight * 4));
        }

        void enlargeAtlas() {
            int newAtlasWidth = atlasWidth;
            int newAtlasHeight = atlasHeight;

            if (newAtlasWidth > 512) {
                newAtlasHeight += 100;
            } else {
                newAtlasWidth *= 2;
            }

            unsigned char* newAtlasTexture = new unsigned char[newAtlasWidth * newAtlasHeight * 4];
            memset(newAtlasTexture, 0, (size_t) (newAtlasWidth * newAtlasHeight * 4));

            // relocate glyphs
            currentX = currentY = maxHeightOnLastRow = 0;

            for (auto& kv : glyphs) {
                int newX = currentX;
                int newY = currentY;

                bool noMoreRoomOnRow = (newX + kv.second.w) >= newAtlasWidth;
                if (noMoreRoomOnRow) {
                    newX = 0;
                    newY += maxHeightOnLastRow;
                }

                for (int y = 0; y < kv.second.h; y++) {
                    unsigned char* sourcePointer = &atlasTexture[((y + kv.second.y) * atlasWidth + kv.second.x) * 4];
                    unsigned char* destinationPointer = &newAtlasTexture[((y + newY) * newAtlasWidth + newX) * 4];
                    memcpy(destinationPointer, sourcePointer, (size_t) (kv.second.w * 4));
                }
                maxHeightOnLastRow = std::max(maxHeightOnLastRow, kv.second.h);

                kv.second.x = newX;
                kv.second.y = newY;

                // advance
                currentX = newX + kv.second.w;
                currentY = newY;
            }

            // remove old data and replace it with the new one
            delete[] atlasTexture;

            atlasTexture = newAtlasTexture;
            atlasWidth = newAtlasWidth;
            atlasHeight = newAtlasHeight;
        }

        bool hasRoomFor(int width, int height) {
            int y = (currentX + width) >= atlasWidth ?
                    currentY + maxHeightOnLastRow :
                    currentY;
            return (y + height) < atlasHeight;
        }

        void add(uint16 asciiChar, FT_GlyphSlot glyphSlot, FT_Bitmap outlineBitmap, int outlineSize) {

            // establish glyph location in atlas
            SDL_Rect srcRect;
            srcRect.x = 0;
            srcRect.y = 0;
            srcRect.w = outlineBitmap.width;
            srcRect.h = outlineBitmap.rows;

            while (!hasRoomFor(outlineBitmap.width + paddingSize * 2, outlineBitmap.rows + paddingSize * 2)) {
                enlargeAtlas();
            }

            SDL_Rect dstRect;
            if ((currentX + outlineBitmap.width + paddingSize * 2) >= atlasWidth) {
                currentX = 0;
                currentY += maxHeightOnLastRow;
                maxHeightOnLastRow = 0;
            }
            maxHeightOnLastRow = std::max(maxHeightOnLastRow, (int) (outlineBitmap.rows + paddingSize * 2));

            dstRect.x = currentX;
            dstRect.y = currentY;
            dstRect.w = outlineBitmap.width;
            dstRect.h = outlineBitmap.rows;

            glyphs[asciiChar] = GlyphInfo{
                    dstRect.x,
                    dstRect.y,
                    dstRect.w + paddingSize * 2,
                    dstRect.h + paddingSize * 2,
                    (int) (glyphSlot->advance.x / 64.0),
                    glyphSlot->bitmap_left,
                    glyphSlot->bitmap_top
            };

            // render outlined glyph
            for (int y = 0; y < outlineBitmap.rows; y++) {
                int sourceOffset = y * outlineBitmap.pitch;
                int destinationOffset = (dstRect.y + y + paddingSize) * atlasWidth + dstRect.x + paddingSize;
                unsigned char* sourcePointer = &outlineBitmap.buffer[sourceOffset];
                for (int x = 0; x < outlineBitmap.width; x++) {
                    unsigned char alphaComponent = *sourcePointer++;
                    atlasTexture[(destinationOffset * 4 + 1)] = alphaComponent;
                    atlasTexture[(destinationOffset * 4 + 3)] = 255;
                    destinationOffset++;
                }
            }

            // copy rendered glyph
            for (int y = 0; y < glyphSlot->bitmap.rows; y++) {
                int sourceOffset = y * glyphSlot->bitmap.pitch;
                int destinationOffset = (dstRect.y + y + outlineSize + paddingSize) * atlasWidth + dstRect.x + outlineSize + paddingSize;
                unsigned char* sourcePointer = &glyphSlot->bitmap.buffer[sourceOffset];
                for (int x = 0; x < glyphSlot->bitmap.width; x++) {
                    unsigned char alphaComponent = *sourcePointer++;
                    atlasTexture[(destinationOffset * 4 + 0)] = alphaComponent;
                    atlasTexture[(destinationOffset * 4 + 3)] = 255;
                    destinationOffset++;
                }
            }

            // copy shadow glyph
            int shadowOffsetX = 2;
            int shadowOffsetY = 2;
            for (int y = 0; y < glyphSlot->bitmap.rows; y++) {
                int dstY = dstRect.y + y + outlineSize + shadowOffsetY + paddingSize;
//                if (y >= (outlineBitmap.rows + paddingSize)) continue;

                int destinationOffset = dstY * atlasWidth + dstRect.x + outlineSize + shadowOffsetX + paddingSize;

                int sourceOffset = y * glyphSlot->bitmap.pitch;
                unsigned char* sourcePointer = &glyphSlot->bitmap.buffer[sourceOffset];
                for (int x = 0; x < glyphSlot->bitmap.width; x++) {
                    unsigned char alphaComponent = *sourcePointer++;
                    atlasTexture[(destinationOffset * 4 + 2)] = alphaComponent;
                    atlasTexture[(destinationOffset * 4 + 3)] = 255;
                    destinationOffset++;
                }
            }

            currentX += srcRect.w + paddingSize * 2;
        }

        void dump() {
            LOG(INFO) << "Font atlas size is " << atlasWidth << "x" << atlasHeight;
            ofstream file("font.raw");
            file.write((const char*) atlasTexture, atlasWidth * atlasHeight * 4);
            file.close();
        }

        int paddingSize = 5;
        int atlasWidth = 16;
        int atlasHeight = 16;
        int currentX = 0;
        int currentY = 0;
        int maxHeightOnLastRow = 0;
        unsigned char* atlasTexture;
        map<uint16, GlyphInfo> glyphs;
        bool containsGlyphFor(char characterToRender);
    };

    struct FontMetrics {
        void setFrom(FT_Face face) {
            if (FT_IS_SCALABLE(face)) {
                FT_Fixed scale = face->size->metrics.y_scale;
                ascent = FT_CEIL(FT_MulFix(face->ascender, scale));
                descent = FT_CEIL(FT_MulFix(face->descender, scale));
                height = ascent - descent + /* baseline */ 1;
                lineSkip = FT_CEIL(FT_MulFix(face->height, scale));
                family = string(face->family_name);
            } else {
                LOG(ERROR) << "Non scalable font, cannot get font metrics";
            }
        }

        int height;
        int lineSkip;
        int ascent;
        int descent;
        string family;
    };

    struct FontRenderStyle {
        int outlineSize = 1;
        int shadowOffsetX = 3;
        int shadowOffsetY = 3;
    };

    class TtfFont {
    public:

        FT_Face face;
        FT_Error error;

        void buildGlyphAtlas() {
            for (uint16 asciiChar = 32; asciiChar < 127; ++asciiChar) {

                FT_UInt glyphIndex = FT_Get_Char_Index(face, asciiChar);
                if (glyphIndex == 0) {
                    LOG(ERROR) << "Font " << fontMetrics.family << " does not have glyph [" << asciiChar << "] provided";
                    continue;
                }

                u8 lcd_weights[10];

                lcd_weights[0] = 0x10;
                lcd_weights[1] = 0x40;
                lcd_weights[2] = 0x70;
                lcd_weights[3] = 0x40;
                lcd_weights[4] = 0x10;

                int flags = FT_LOAD_FORCE_AUTOHINT;
                FT_Library_SetLcdFilter(library, FT_LCD_FILTER_LIGHT);
                flags |= FT_LOAD_TARGET_LCD;
                FT_Library_SetLcdFilterWeights(library, lcd_weights);

                error = FT_Load_Glyph(face, glyphIndex, flags);
                if (error) {
                    LOG(ERROR) << "Failed to load glyph [" << asciiChar << "] for font " << fontMetrics.family;
                    continue;
                }

//                // render outline if needed, overwriting the normal glyph but maintaining the sizes
                int outlineSize = 1;
                FT_Stroker stroker;
                error = FT_Stroker_New(library, &stroker);
                FT_Stroker_Set(stroker, outlineSize * 64, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);

                FT_Load_Glyph(face, glyphIndex, flags);
                FT_Glyph bitmapGlyph;
                FT_Get_Glyph(face->glyph, &bitmapGlyph);
                FT_Glyph_Stroke(&bitmapGlyph, stroker, 1 /* delete the original glyph */ );
                // FT_Glyph_StrokeBorder(&bitmapGlyph, stroker, false, true /* delete the original glyph */ );
                FT_Stroker_Done(stroker);
                FT_Glyph_To_Bitmap(&bitmapGlyph, FT_RENDER_MODE_NORMAL, nullptr, true); // no translation, destroy original image

                // render normal glyph
                error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
                if (error) {
                    LOG(ERROR) << "Failed to render glyph for slot " << face->glyph;
                    continue;
                }

//                face->glyph->bitmap = ((FT_BitmapGlyph) bitmapGlyph)->bitmap;

                atlas_.add(asciiChar, face->glyph, ((FT_BitmapGlyph) bitmapGlyph)->bitmap, outlineSize);
            }
            atlas_.dump();
        }


        TtfFont(const string& fontFile, int pointSize, FontRenderStyle renderStyle) {
            this->renderStyle = renderStyle;
            FT_Error error = FT_Init_FreeType(&library);
            if (error) {
                LOG(ERROR) << "Failed to initialize freetype lib";
            } else {
                LOG(INFO) << "Freetype initialized OK";
            }

            error = FT_New_Face(library, fontFile.c_str(), 0, &face);
            if (error) {
                LOG(ERROR) << "Failed to load freetype face";
            } else {
                LOG(INFO) << "Freetype face loaded OK";
            }

            error = FT_Set_Char_Size(
                    face,    /* handle to face object           */
                    0,       /* char_width in 1/64th of points  */
                    pointSize * 64,   /* char_height in 1/64th of points */
                    0,     /* horizontal device resolution    */
                    0);   /* vertical device resolution      */
            if (error) {
                LOG(ERROR) << "Failed to set char size";
            } else {
                LOG(INFO) << "Char size set OK";
            }
            fontMetrics.setFrom(face);

            buildGlyphAtlas();
        }

        void addCharToVbo(const char& charToRender, const vec4& color) {
            GlyphInfo glyphInfo = atlas_.glyphs[charToRender];

            if (previousChar != 0) {
                penPosition.x += atlas_.glyphs[previousChar].advance;

                if (FT_HAS_KERNING(face)) {
                    FT_UInt previousGlyphIndex = FT_Get_Char_Index(face, previousChar);
                    FT_UInt currentGlyphIndex = FT_Get_Char_Index(face, charToRender);

                    if (previousGlyphIndex != 0 && currentGlyphIndex != 0) {
                        FT_Vector kerningInfo;
                        FT_Error error = FT_Get_Kerning(face, previousGlyphIndex, currentGlyphIndex, FT_KERNING_DEFAULT, &kerningInfo);
                        if (error) {
                            LOG(ERROR) << "Failed to get kerning info between " << to_string(previousChar) << " and " << to_string(charToRender);
                        } else {
                            penPosition.x += kerningInfo.x / 64.0; // 2^6 = 64
                        }
                    }
                }
            }

            float penX = penPosition.x;
            float penY = penPosition.y;
            vector<vec2> verts = {
                    {penX + glyphInfo.bitmapLeft,               penY - glyphInfo.bitmapTop},
                    {penX + glyphInfo.bitmapLeft + glyphInfo.w, penY - glyphInfo.bitmapTop},
                    {penX + glyphInfo.bitmapLeft + glyphInfo.w, penY + glyphInfo.h - glyphInfo.bitmapTop},
                    {penX + glyphInfo.bitmapLeft,               penY + glyphInfo.h - glyphInfo.bitmapTop}
            };

            double textureXNormalizationFactor = 1.0 / (double) atlas_.atlasWidth;
            double textureYNormalizationFactor = 1.0 / (double) atlas_.atlasHeight;
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

            vbo.addQuad(verts, texs, color);

            previousChar = charToRender;
        }

        void print(float x, float y, const string& text, const vec4& color) {
            // creates a vbo, sets texture, shader, renders
            penPosition = vec2(x, y);
            previousChar = 0;
            vbo.empty();
            for (auto it = text.begin(); it != text.end(); it++) {
                char charToRender = *it;
                if (charToRender == '\n') {
                    penPosition.y += fontMetrics.height;
                    penPosition.x = x;
                    previousChar = 0;
                } else {
                    addCharToVbo(*it, color);
                }
            }

            vbo.draw();
        }

        const FontMetrics& metrics() const {
            return fontMetrics;
        }

        const GlyphAtlas& atlas() const {
            return atlas_;
        }

        shared_ptr<Texture> getTexture();
        bool hasChar(char characterToRender);
        int getKerning(char previousChar, const char& currentChar);
        int getCharWidth(const char& character);
    private:

        char previousChar;
        Vbo vbo;
        GlyphAtlas atlas_;
        FontMetrics fontMetrics;
        vec2 penPosition;
        FontRenderStyle renderStyle;
    };


    GameStateTest& GameStateTest::instance() {
        static GameStateTest instance;
        return instance;
    }

    void _check_gl_error(const char* file, int line) {
        GLenum err(glGetError());

        while (err != GL_NO_ERROR) {
            string error;

            switch (err) {
                case GL_INVALID_OPERATION:
                    error = "INVALID_OPERATION";
                    break;
                case GL_INVALID_ENUM:
                    error = "INVALID_ENUM";
                    break;
                case GL_INVALID_VALUE:
                    error = "INVALID_VALUE";
                    break;
                case GL_OUT_OF_MEMORY:
                    error = "OUT_OF_MEMORY";
                    break;
                case GL_INVALID_FRAMEBUFFER_OPERATION:
                    error = "INVALID_FRAMEBUFFER_OPERATION";
                    break;
                default:
                    error = "UNKNOWN_ERROR";
                    break;
            }

            cerr << "GL_" << error.c_str() << " - " << file << ":" << line << endl;
            err = glGetError();
        }
    }

    class TextLayout {
    public:
        void wrapLastChars(int howManyChars);

        TextLayout(const string& textToRender, Box constraint, TtfFont* fnt)
                : text(textToRender), font(fnt), textBox(constraint) {
            penPosition = textBox.tl + vec2(0, font->metrics().ascent);
            previousChar = 0;
            vbo.empty();
            for (auto it = textToRender.begin(); it != textToRender.end(); it++) {
                char charToRender = *it;
                if (charToRender == '\n') {
                    penPosition.y += font->metrics().lineSkip;
                    penPosition.x = textBox.tl.x;
                    previousChar = 0;
                } else if (font->hasChar(charToRender)) {
//                    int width = font->getCharWidth(charToRender);
                    addCharToVbo(charToRender);
                } else {
                    // non printable character. do nothing
                }
            }
        }

        void addCharToVbo(const char& charToRender) {
            const GlyphInfo& glyphInfo = font->atlas().glyphs.at(static_cast<uint16>(charToRender));

            if (previousChar != 0) {
                penPosition.x += font->getCharWidth(previousChar);
                penPosition.x += font->getKerning(previousChar, charToRender);
            }

            float penX = penPosition.x;
            float penY = penPosition.y;
            vector<vec2> verts = {
                    {penX + glyphInfo.bitmapLeft,               penY - glyphInfo.bitmapTop},
                    {penX + glyphInfo.bitmapLeft + glyphInfo.w, penY - glyphInfo.bitmapTop},
                    {penX + glyphInfo.bitmapLeft + glyphInfo.w, penY + glyphInfo.h - glyphInfo.bitmapTop},
                    {penX + glyphInfo.bitmapLeft,               penY + glyphInfo.h - glyphInfo.bitmapTop}
            };

            double textureXNormalizationFactor = 1.0 / (double) font->atlas().atlasWidth;
            double textureYNormalizationFactor = 1.0 / (double) font->atlas().atlasHeight;
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

        const Box& getTextBox() const {
            return textBox;
        }

        void render() {
            vbo.draw();
        }

        void render(int howManyChars) {
            // TODO: make it possible to also provide a from/to range (mind the buffering in the vbo)
            vbo.draw(howManyChars * 2); // 2 triangles per char
        }

    private:
        string text;
        TtfFont* font;
        Box textBox;
        vec2 penPosition;
        char previousChar;
        Vbo vbo;
    };

    shared_ptr<TtfFont> newFont;
    shared_ptr<TtfFont> bigFont;
    FpsCounter fpsCounter;
    shared_ptr<TextLayout> textLayout;

    void GameStateTest::onEnter(Stardust* game) {
        FontRenderStyle renderStyle;
        newFont = shared_ptr<TtfFont>(new TtfFont("../data/fonts/Neo Sans Pro Bold.ttf", 32, renderStyle));
        fontTexture = newFont->getTexture();

        bigFont = shared_ptr<TtfFont>(new TtfFont("../data/fonts/arial.ttf", 50, renderStyle));
        bigFontTexture = bigFont->getTexture();


        string textToRender = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.\nVivamus nunc mauris, feugiat at tortor quis, ullamcorper porta tellus.\nPhasellus sit amet eleifend orci.";
        Box constraint(400, 300);
        constraint.translate(vec2(200, 300));
        textLayout = shared_ptr<TextLayout>(new TextLayout(textToRender, constraint, newFont.get()));
        // ------------------------------------

        script = new Script("scripts/dialog.lua");

        GFXSYS.setClearColor(vec3(0, 0, 0));
        AUDIOSYS.playMusic("menu3.ogg");

        // initialize shader
        gpuProgram = make_shared<GpuProgram>(
                FILESYS.getFileContent("shaders/test.vs.glsl"),
                FILESYS.getFileContent("shaders/test.ps.glsl")
        );

        gpuProgramConstantBundle = make_shared<GpuProgramConstantBundle>();

        gpuProgramConstantBundle->add("orthoMatrix",
                                      GpuProgramConstant(ortho(0.0f, 1024.0f, 768.0f, 0.0f, 0.0f, 1.0f)));
        int textureSamplerIndex = 0;
        gpuProgramConstantBundle->add("diffuseSampler", GpuProgramConstant(textureSamplerIndex));
        gpuProgramConstantBundle->add("cameraShakeMatrix", GpuProgramConstant(glm::mat4()));
        gpuProgramConstantBundle->add("textDiffuseColor", GpuProgramConstant(vec4(0.5, 0.5, 0.6, 0.9)));
        gpuProgramConstantBundle->add("textOutlineColor", GpuProgramConstant(vec4(0.0, 0.0, 0.0, 0.4)));
        gpuProgramConstantBundle->add("textShadowColor", GpuProgramConstant(vec4(0.0, 0.0, 0.0, 0.4)));

        gpuProgram->addConstants(gpuProgramConstantBundle.get());
    }

    void GameStateTest::onExit(Stardust* game) {
        delete script;
    }

    float textAlpha = 0;

    void GameStateTest::update(Stardust* game, float elapsedSeconds) {
//        DialogRepository::getInstance().updateAll(elapsedSeconds);

        fpsCounter.update(elapsedSeconds);
        textAlpha += elapsedSeconds;

        if (INPUT.isKeyJustPressed(SDL_SCANCODE_ESCAPE)) {
            game->quitGame = true;
        }
    }



    void GameStateTest::render(Stardust* game, float alpha) {
        GFXSYS.setClearColor(vec3(0.1 * 2, 0.1 * 2, 0.12 * 2));
        GFXSYS.clearScreen();

//        DialogRepository::getInstance().renderAll();

        GFXSYS.setTransparencyMode(TransparencyMode::Transparent);

        gpuProgramConstantBundle->add("textDiffuseColor", GpuProgramConstant(vec4(1.0, 0.5, 0.2, 1)));
        gpuProgram->use();
        fontTexture->useForUnit(0);

//        const char* text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.\nVivamus nunc mauris, feugiat at tortor quis, ullamcorper porta tellus.\nPhasellus sit amet eleifend orci.";
//        newFont->print(100, 200, text, vec4(1, 0.5, 0.2, 1));
//        newFont->print(100, 200, text, vec4(0.5, 0.5, 0.6, (sin(textAlpha) / 2.0) + 0.5));
//        newFont->print(100, 200, text, vec4(0.5, 0.5, 0.6, 1));

        stringstream ss;
        ss << "FPS: " << fpsCounter.getFps();
        bigFontTexture->useForUnit(0);
        bigFont->print(0, 100, ss.str().c_str(), vec4(1.0, 0.5, 0.2, 1));

        gpuProgram->unuse();
//        GFXSYS.drawFilledRectangle(textLayout->getTextBox().tl, textLayout->getTextBox().tl, vec4(0, 0, 0, 0.5));
        GFXSYS.drawFilledRectangle(textLayout->getTextBox().tl, textLayout->getTextBox().br, vec4(0, 0, 0, 0.5));

        gpuProgramConstantBundle->add("textDiffuseColor", GpuProgramConstant(vec4(0.5, 0.5, 0.6, 0.9)));
        gpuProgram->use();
        fontTexture->useForUnit(0);
        textLayout->render();

//        GFXSYS.renderFullScreenTexturedQuad();
        gpuProgram->unuse();

        GFXSYS.show();
        fpsCounter.addFrame();
    }

    GameStateTest::GameStateTest() {
    }

    shared_ptr<Texture> TtfFont::getTexture() {

        GLuint textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlas_.atlasWidth, atlas_.atlasHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, atlas_.atlasTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

        return std::shared_ptr<Texture>(new Texture(textureId, atlas_.atlasWidth, atlas_.atlasHeight));
    }

    bool TtfFont::hasChar(char characterToRender) {
        return atlas_.containsGlyphFor(characterToRender);
    }

    bool GlyphAtlas::containsGlyphFor(char characterToRender) {
        return glyphs.lower_bound(static_cast<uint16>(characterToRender)) != glyphs.end();
    }

    int TtfFont::getKerning(char previousChar, const char& currentChar) {
        if (FT_HAS_KERNING(face)) {
            FT_UInt previousGlyphIndex = FT_Get_Char_Index(face, static_cast<FT_ULong>(previousChar));
            FT_UInt currentGlyphIndex = FT_Get_Char_Index(face, static_cast<FT_ULong>(currentChar));

            if (previousGlyphIndex != 0 && currentGlyphIndex != 0) {
                FT_Vector kerningInfo;
                FT_Error error = FT_Get_Kerning(face, previousGlyphIndex, currentGlyphIndex, FT_KERNING_DEFAULT, &kerningInfo);
                if (!error) {
                    return kerningInfo.x / 64.0; // 2^6 = 64
                }
            }
        }
        return 0;
    }

    int TtfFont::getCharWidth(const char& character) {
        return atlas_.glyphs.at(static_cast<uint16>(character)).advance;
    }

    void TextLayout::wrapLastChars(int howManyChars) {

    }
} // namespace Acidrain
