#include <Font.h>
#include <FileSystem.h>
#include <easylogging++.h>
#include <GpuProgram.h>
#include <GpuProgramConstants.h>
#include <glm/gtc/matrix_transform.hpp> // for ortho()
#include <GfxSystem.h>

#include <ft2build.h>
#include <ftglyph.h>
#include <ftoutln.h>
#include <fttrigon.h>
#include <ftmodapi.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_STROKER_H
#include FT_LCD_FILTER_H
#include FT_GLYPH_H
#include FT_TRUETYPE_IDS_H

namespace Acidrain {

    static FT_Library FREETYPE_LIBRARY_HANDLE = nullptr;
    static int FONT_INSTANCE_COUNT = 0;

    Font::Font(const string& fontFile, int pointSize, FontRenderStyle renderStyle) {
        this->renderStyle = renderStyle;

        // initialize shader
        gpuProgram = make_shared<GpuProgram>(
                FILESYS.getFileContent("shaders/font.vs.glsl"),
                FILESYS.getFileContent("shaders/font.compatible.ps.glsl")
        );

        gpuProgramConstantBundle = make_shared<GpuProgramConstantBundle>();

        gpuProgramConstantBundle->add("orthoMatrix",
                                      GpuProgramConstant(ortho(0.0f, 1024.0f * TEXT_DOWNSAMPLE_FACTOR, 768.0f * TEXT_DOWNSAMPLE_FACTOR, 0.0f, 0.0f, 1.0f)));

        int textureSamplerIndex = 0;
        gpuProgramConstantBundle->add("diffuseSampler", GpuProgramConstant(textureSamplerIndex));

        gpuProgram->addConstants(gpuProgramConstantBundle.get());

        FT_Error error;
        if (FREETYPE_LIBRARY_HANDLE == nullptr) {
            error = FT_Init_FreeType(&FREETYPE_LIBRARY_HANDLE);
            if (error) {
                LOG(ERROR) << "Failed to initialize freetype lib";
            } else {
                LOG(INFO) << "Freetype initialized OK";
            }
        }
        FONT_INSTANCE_COUNT++;

        string absoluteFontFile = FILESYS.absolutePath(fontFile);
        error = FT_New_Face(FREETYPE_LIBRARY_HANDLE, absoluteFontFile.c_str(), 0, &face);
        if (error) {
            LOG(ERROR) << "Failed to load freetype face from " << absoluteFontFile;
        } else {
            LOG(INFO) << "Loaded font " << fontFile;
        }

        error = FT_Set_Char_Size(
                face,    /* handle to face object           */
                0,       /* char_width in 1/64th of points  */
                (FT_F26Dot6) (pointSize * TEXT_DOWNSAMPLE_FACTOR * 64),   /* char_height in 1/64th of points */
                0,     /* horizontal device resolution    */
                0);   /* vertical device resolution      */
        if (error) {
            LOG(ERROR) << "Failed to set char size";
        }
        fontMetrics.setFrom(face);

        buildGlyphAtlas(renderStyle);
    }

    Font::~Font() {
        FONT_INSTANCE_COUNT--;
        if (FONT_INSTANCE_COUNT == 0 && FREETYPE_LIBRARY_HANDLE != nullptr) {
            FT_Done_Library(FREETYPE_LIBRARY_HANDLE);
        }
    }

    void Font::buildGlyphAtlas(FontRenderStyle fontRenderStyle) {
        for (uint16 asciiChar = 32; asciiChar < 127; ++asciiChar) {

            FT_UInt glyphIndex = FT_Get_Char_Index(face, asciiChar);
            if (glyphIndex == 0) {
                LOG(ERROR) << "Font " << fontMetrics.family << " does not have glyph [" << asciiChar << "] provided";
                continue;
            }

            unsigned char lcd_weights[10];

            lcd_weights[0] = 0x10;
            lcd_weights[1] = 0x40;
            lcd_weights[2] = 0x70;
            lcd_weights[3] = 0x40;
            lcd_weights[4] = 0x10;

            int flags = FT_LOAD_FORCE_AUTOHINT;
            FT_Library_SetLcdFilter(FREETYPE_LIBRARY_HANDLE, FT_LCD_FILTER_LIGHT);
            flags |= FT_LOAD_TARGET_LCD;
            FT_Library_SetLcdFilterWeights(FREETYPE_LIBRARY_HANDLE, lcd_weights);

            FT_Error error = FT_Load_Glyph(face, glyphIndex, flags);
            if (error) {
                LOG(ERROR) << "Failed to load glyph [" << asciiChar << "] for font " << fontMetrics.family;
                continue;
            }

            // render outline if needed, overwriting the normal glyph but maintaining the sizes
            int outlineSize = (int) (renderStyle.outlineSize * TEXT_DOWNSAMPLE_FACTOR);
            FT_Stroker stroker;
            FT_Stroker_New(FREETYPE_LIBRARY_HANDLE, &stroker);
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

            atlas_.add(asciiChar, face->glyph, ((FT_BitmapGlyph) bitmapGlyph)->bitmap, fontRenderStyle);
        }

        GLuint textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlas_.width(), atlas_.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, atlas_.texture());

        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        texture_ = shared_ptr<Texture>(new Texture(textureId, atlas_.width(), atlas_.height()));
    }

    void Font::addCharToVbo(const char& charToRender, const vec4& color) {
        GlyphInfo glyphInfo = atlas_.glyph(charToRender);

        if (previousChar != 0) {
            penPosition.x += atlas_.glyph(previousChar).advance;

            if (FT_HAS_KERNING(face)) {
                FT_UInt previousGlyphIndex = FT_Get_Char_Index(face, (FT_ULong) previousChar);
                FT_UInt currentGlyphIndex = FT_Get_Char_Index(face, (FT_ULong) charToRender);

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
        vector <vec2> verts = {
                {penX + glyphInfo.bitmapLeft,               penY - glyphInfo.bitmapTop},
                {penX + glyphInfo.bitmapLeft + glyphInfo.w, penY - glyphInfo.bitmapTop},
                {penX + glyphInfo.bitmapLeft + glyphInfo.w, penY + glyphInfo.h - glyphInfo.bitmapTop},
                {penX + glyphInfo.bitmapLeft,               penY + glyphInfo.h - glyphInfo.bitmapTop}
        };

        double textureXNormalizationFactor = 1.0 / (double) atlas_.width();
        double textureYNormalizationFactor = 1.0 / (double) atlas_.height();
        double x, y, w, h;
        x = glyphInfo.x * textureXNormalizationFactor;
        y = glyphInfo.y * textureYNormalizationFactor;
        w = glyphInfo.w * textureXNormalizationFactor;
        h = glyphInfo.h * textureYNormalizationFactor;

        vector <vec2> texs = {
                {x,     y},
                {x + w, y},
                {x + w, y + h},
                {x,     y + h}
        };

        vbo.addQuad(verts, texs, color);

        previousChar = charToRender;
    }

    void Font::print(float x, float y, const string& text, const vec4& color) {

        // creates a vbo, sets texture, shader, renders
        penPosition = vec2(x, y) + vec2(0, (fontMetrics.ascent - atlas_.padding()) / TEXT_DOWNSAMPLE_FACTOR);
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

        gpuProgram->use();
        texture()->useForUnit(0);
        vbo.draw();
        gpuProgram->unuse();
        GFXSYS.setViewport();
    }

    const FontMetrics& Font::metrics() const {
        return fontMetrics;
    }

    const GlyphAtlas& Font::atlas() const {
        return atlas_;
    }

    const shared_ptr<Texture> Font::texture() const {
        return texture_;
    }

    bool Font::hasChar(char characterToRender) const {
        return atlas_.containsGlyphFor(characterToRender);
    }

    int Font::getKerning(char previousChar, const char& currentChar) const {
        if (FT_HAS_KERNING(face)) {
            FT_UInt previousGlyphIndex = FT_Get_Char_Index(face, static_cast<FT_ULong>(previousChar));
            FT_UInt currentGlyphIndex = FT_Get_Char_Index(face, static_cast<FT_ULong>(currentChar));

            if (previousGlyphIndex != 0 && currentGlyphIndex != 0) {
                FT_Vector kerningInfo;
                FT_Error error = FT_Get_Kerning(face, previousGlyphIndex, currentGlyphIndex, FT_KERNING_DEFAULT, &kerningInfo);
                if (!error) {
                    return (int) (kerningInfo.x / 64.0); // 2^6 = 64
                }
            }
        }
        return 0;
    }

    int Font::getCharWidth(const char& character) const {
        return atlas_.glyph(character).advance;
    }

    const vec2 Font::getLastCharPosition() {
        return penPosition;
    }
}
