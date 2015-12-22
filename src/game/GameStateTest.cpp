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
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <GpuProgram.h>
#include <GpuProgramConstants.h>
#include <Vbo.h>

#include <ft2build.h>
#include <ftglyph.h>
#include <ftoutln.h>
#include <fttrigon.h>
#include FT_FREETYPE_H
#include FT_STROKER_H
#include FT_LCD_FILTER_H

#define CHECK_GL_ERROR() _check_gl_error(__FILE__,__LINE__)

namespace Acidrain {

    static Script* script;
    static GLuint fontTextureId;
    static TTF_Font* font;
    static shared_ptr<GpuProgram> gpuProgram;
    static shared_ptr<GpuProgramConstantBundle> gpuProgramConstantBundle;
    static shared_ptr<Texture> fontTexture;
    static FT_Library library;
    static FT_Face face;

    struct GlyphInfo {
        int x;
        int y;
        int w;
        int h;
        int advance;
        int bitmapLeft;
        int bitmapTop;
        int bearingX;
        int bearingY;
    };

    struct GlyphAtlas {

        GlyphAtlas() {
            atlasTexture = new unsigned char[atlasWidth * atlasHeight * 4];
            memset(atlasTexture, 0, (size_t) (atlasWidth * atlasHeight * 4));
        }

        void enlargeAtlas() {
            int newAtlasWidth = atlasWidth * 2;
            int newAtlasHeight = atlasHeight * 2;

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

        void add(uint16 asciiChar, FT_GlyphSlot glyphSlot, FT_Bitmap outlineBitmap) {

//            FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

            // establish glyph location in atlas
            SDL_Rect srcRect;
            srcRect.x = 0;
            srcRect.y = 0;
            srcRect.w = glyphSlot->bitmap.width;
            srcRect.h = glyphSlot->bitmap.rows;

            while (!hasRoomFor(srcRect.w, srcRect.h)) {
                enlargeAtlas();
            }

            SDL_Rect dstRect;
            if ((currentX + srcRect.w) >= atlasWidth) {
                cout << "No more room on row. Switching to next row" << endl;
                currentX = 0;
                currentY += maxHeightOnLastRow;
                maxHeightOnLastRow = 0;
            }
            maxHeightOnLastRow = std::max(maxHeightOnLastRow, srcRect.h);

            dstRect.x = currentX;
            dstRect.y = currentY;
            dstRect.w = srcRect.w;
            dstRect.h = srcRect.h;

            glyphs[asciiChar] = GlyphInfo{
                    dstRect.x,
                    dstRect.y,
                    dstRect.w,
                    dstRect.h,
                    (int) (glyphSlot->advance.x / 64.0),
                    glyphSlot->bitmap_left,
                    glyphSlot->bitmap_top,
                    (int) (glyphSlot->metrics.horiBearingX / 64.0),
                    (int) (glyphSlot->metrics.horiBearingY / 64.0)
            };


            // copy outline glyph
//            int outlineSize = 1;
//            FT_Stroker stroker;
//            FT_Stroker_New(library, &stroker);
//            FT_Stroker_Set(stroker, outlineSize * 64, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
//
//            FT_Glyph bitmapGlyph;
//            FT_Get_Glyph(glyphSlot, &bitmapGlyph);
//            FT_Glyph_Stroke(&bitmapGlyph, stroker, 1 /* delete the original glyph */ );
//            FT_Stroker_Done(stroker);
//            FT_Glyph_To_Bitmap(&bitmapGlyph, FT_RENDER_MODE_NORMAL, 0, 1); // no translation, destroy original image


//
//            // render outlined glyph
            for (int y = 0; y < outlineBitmap.rows; y++) {
                int sourceOffset = y * outlineBitmap.pitch;
                int destinationOffset = (dstRect.y + y) * atlasWidth + dstRect.x;
                unsigned char* sourcePointer = &outlineBitmap.buffer[sourceOffset];
                for (int x = 0; x < srcRect.w; x++) {
                    unsigned char alphaComponent = *sourcePointer++;
                    atlasTexture[(destinationOffset * 4 + 1)] = alphaComponent;
                    destinationOffset++;
                }
            }
//
//            FT_Done_Glyph(bitmapGlyph);


//             copy rendered glyph
            for (int y = 0; y < glyphSlot->bitmap.rows; y++) {
                int sourceOffset = y * glyphSlot->bitmap.pitch;
                int destinationOffset = (dstRect.y + y) * atlasWidth + dstRect.x;
                unsigned char* sourcePointer = &glyphSlot->bitmap.buffer[sourceOffset];
                for (int x = 0; x < srcRect.w; x++) {
                    unsigned char alphaComponent = *sourcePointer++;
                    atlasTexture[(destinationOffset * 4 + 0)] = alphaComponent;
//                    atlasTexture[(destinationOffset * 4 + 1)] = 0;
                    atlasTexture[(destinationOffset * 4 + 2)] = 0;
                    atlasTexture[(destinationOffset * 4 + 3)] = alphaComponent;
                    destinationOffset++;
                }
            }



            currentX += srcRect.w;
        }

        void dump() {
            cout << "atlas size is " << atlasWidth << "x" << atlasHeight << endl;
            ofstream file("font.raw");
            file.write((const char*) atlasTexture, atlasWidth * atlasHeight * 4);
            file.close();
        }

        int atlasWidth = 16;
        int atlasHeight = 16;
        int currentX = 0;
        int currentY = 0;
        int maxHeightOnLastRow = 0;
        unsigned char* atlasTexture;
        map<uint16, GlyphInfo> glyphs;
    };

    struct TtfFontMetrics {
        void setFrom(TTF_Font* font) {
            height = TTF_FontHeight(font);
            lineSkip = TTF_FontLineSkip(font);
            ascent = TTF_FontAscent(font);
            descent = TTF_FontDescent(font);
            family = string(TTF_FontFaceFamilyName(font));
        }

        int height;
        int lineSkip;
        int ascent;
        int descent;
        string family;
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
                int outlineSize = 2;
                FT_Stroker stroker;
                error = FT_Stroker_New(library, &stroker);
                FT_Stroker_Set(stroker, outlineSize * 64, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);

                FT_Glyph bitmapGlyph;
                FT_Get_Glyph(face->glyph, &bitmapGlyph);
                FT_Glyph_Stroke(&bitmapGlyph, stroker, 1 /* delete the original glyph */ );
                FT_Stroker_Done(stroker);
                FT_Glyph_To_Bitmap(&bitmapGlyph, FT_RENDER_MODE_NORMAL, 0, 1); // no translation, destroy original image

                // render normal glyph
                error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
                if (error) {
                    LOG(ERROR) << "Failed to render glyph for slot " << face->glyph;
                    continue;
                }

//                face->glyph->bitmap = ((FT_BitmapGlyph) bitmapGlyph)->bitmap;

                atlas.add(asciiChar, face->glyph, ((FT_BitmapGlyph) bitmapGlyph)->bitmap);
            }
            atlas.dump();
        }


        TtfFont(const string& fontFile, int pointSize) {
            if (TTF_Init() == -1) {
                LOG(ERROR) << "Failed to initialize SDL_ttf";
                return;
            }

            font = TTF_OpenFont(fontFile.c_str(), pointSize);
            if (font == nullptr) {
                LOG(ERROR) << "Failed to open font " << fontFile;
                return;
            }

            TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
//            TTF_SetFontStyle(font, TTF_STYLE_BOLD);
            TTF_SetFontKerning(font, 0);
            TTF_SetFontHinting(font, 0);
//            TTF_SetFontOutline(font, 1);

            fontMetrics.setFrom(font);


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


            buildGlyphAtlas();
        }

        void addCharToVbo(const char& charToRender, const vec4& color) {
            GlyphInfo glyphInfo = atlas.glyphs[charToRender];

            if (previousChar != 0) {
                penPosition.x += atlas.glyphs[previousChar].advance;

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

            double textureNormalizationFactor = 1.0 / (double) atlas.atlasWidth;
            double x, y, w, h;
            x = glyphInfo.x * textureNormalizationFactor;
            y = glyphInfo.y * textureNormalizationFactor;
            w = glyphInfo.w * textureNormalizationFactor;
            h = glyphInfo.h * textureNormalizationFactor;

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

        shared_ptr<Texture> getTexture();
    private:

        char previousChar;
        Vbo vbo;
        GlyphAtlas atlas;
        TtfFontMetrics fontMetrics;
        vec2 penPosition;
        TTF_Font* font;
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

    TtfFont* newFont;
//    TtfFont newFont(string("../data/fonts/FreeSans.ttf"), 20);

    void GameStateTest::onEnter(Stardust* game) {
        string fontName = "../data/fonts/Neo Sans Pro Bold.ttf";
        int fontSize = 50;

        FT_Error error = FT_Init_FreeType(&library);
        if (error) {
            LOG(ERROR) << "Failed to initialize freetype lib";
        } else {
            LOG(INFO) << "Freetype initialized OK";
        }


        newFont = new TtfFont(fontName, fontSize);
        fontTexture = newFont->getTexture();

        error = FT_New_Face(library,
                            fontName.c_str(),
                            0,
                            &face);
        if (error) {
            LOG(ERROR) << "Failed to load freetype face";
        } else {
            LOG(INFO) << "Freetype face loaded OK";
        }

        error = FT_Set_Char_Size(
                face,    /* handle to face object           */
                0,       /* char_width in 1/64th of points  */
                fontSize * 64,   /* char_height in 1/64th of points */
                0,     /* horizontal device resolution    */
                0);   /* vertical device resolution      */
        if (error) {
            LOG(ERROR) << "Failed to set char size";
        } else {
            LOG(INFO) << "Char size set OK";
        }

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

        gpuProgram->addConstants(gpuProgramConstantBundle.get());
    }

    void GameStateTest::onExit(Stardust* game) {
        delete script;
    }

    float xPos = 0;

    void GameStateTest::update(Stardust* game, float elapsedSeconds) {
        DialogRepository::getInstance().updateAll(elapsedSeconds);

        if (INPUT.isKeyJustPressed(SDL_SCANCODE_ESCAPE)) {
            game->quitGame = true;
        }

//        xPos -= elapsedSeconds * 100;

    }

    void GameStateTest::render(Stardust* game, float alpha) {
        GFXSYS.setClearColor(vec3(0.1, 0.1, 0.12));
        GFXSYS.clearScreen();

//        DialogRepository::getInstance().renderAll();

        GFXSYS.setTransparencyMode(TransparencyMode::Transparent);

        gpuProgram->use();
        fontTexture->useForUnit(0);

        newFont->print(xPos, 100, "Adrian Scripca\ne cel mai tare\nprogramator pe care il stiu.", vec4(1, 0.5, 0.2, 0.9));
//        newFont->print(100, 200, "BackFire is a nice Game", vec4(1, 0.5, 0.2, 0.9));
//        GFXSYS.renderFullScreenTexturedQuad();
        gpuProgram->unuse();

        GFXSYS.show();
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

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlas.atlasWidth, atlas.atlasHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, atlas.atlasTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

        return std::shared_ptr<Texture>(new Texture(textureId, atlas.atlasWidth, atlas.atlasHeight));
    }
} // namespace Acidrain
