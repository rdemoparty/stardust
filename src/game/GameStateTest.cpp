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
#include FT_FREETYPE_H

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
    };

    struct GlyphAtlas {

        GlyphAtlas() {
            atlasTexture = new unsigned char[atlasWidth * atlasHeight * 4];
            memset(atlasTexture, 0, (size_t) (atlasWidth * atlasHeight * 4));
        }

        void enlargeAtlas(int fontHeight) {
            int newAtlasWidth = atlasWidth * 2;
            int newAtlasHeight = atlasHeight * 2;

            unsigned char* newAtlasTexture = new unsigned char[newAtlasWidth * newAtlasHeight * 4];
            memset(newAtlasTexture, 0, (size_t) (newAtlasWidth * newAtlasHeight * 4));

            // relocate glyphs
            currentX = currentY = 0;

            for (auto& kv : glyphs) {
                int newX = currentX;
                int newY = currentY;

                bool noMoreRoomOnRow = (newX + kv.second.w) >= newAtlasWidth;
                if (noMoreRoomOnRow) {
                    newX = 0;
                    newY += fontHeight;
                }

                for (int y = 0; y < kv.second.h; y++) {
                    unsigned char* sourcePointer = &atlasTexture[((y + kv.second.y) * atlasWidth + kv.second.x) * 4];
                    unsigned char* destinationPointer = &newAtlasTexture[((y + newY) * newAtlasWidth + newX) * 4];
                    memcpy(destinationPointer, sourcePointer, (size_t) (kv.second.w * 4));
                }


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
                    currentY + height :
                    currentY;
            return (y + height) < atlasHeight;
        }

        void add(uint16 asciiChar, TTF_Font* font) {
            int fontHeight = TTF_FontHeight(font);

            SDL_Color white = {1, 1, 1, 1};
            SDL_Surface* glyphSurface = TTF_RenderGlyph_Blended(font, asciiChar, white);
            if (glyphSurface != nullptr) {
                int minX, maxX, minY, maxY, advance;
                TTF_GlyphMetrics(font, asciiChar, &minX, &maxX, &minY, &maxY, &advance);

                SDL_Rect srcRect;
                srcRect.x = 0;
                srcRect.y = 0;
                srcRect.w = glyphSurface->w;
                srcRect.h = glyphSurface->h;

                while (!hasRoomFor(srcRect.w, fontHeight)) {
                    enlargeAtlas(fontHeight);
                }

                SDL_Rect dstRect;
                if ((currentX + srcRect.w) >= atlasWidth) {
                    currentX = 0;
                    currentY += fontHeight;
                }

                dstRect.x = currentX;
                dstRect.y = currentY;
                dstRect.w = glyphSurface->w;
                dstRect.h = glyphSurface->h;

                glyphs[asciiChar] = GlyphInfo{
                        dstRect.x,
                        dstRect.y,
                        dstRect.w,
                        dstRect.h,
                        advance
                };

                for (int y = 0; y < srcRect.h; y++) {
                    int sourceOffset = y * srcRect.w;
                    int destinationOffset = (dstRect.y + y) * atlasWidth + dstRect.x;
                    unsigned int* sourcePointer = &((unsigned int*) glyphSurface->pixels)[sourceOffset];
                    for (int x = 0; x < srcRect.w; x++) {
                        unsigned int pixel = *sourcePointer++;
                        unsigned char alphaComponent = (unsigned char) ((pixel >> 24) & 0xff);
                        atlasTexture[(destinationOffset * 4 + 0)] = 0;
                        atlasTexture[(destinationOffset * 4 + 1)] = 0;
                        atlasTexture[(destinationOffset * 4 + 2)] = 0;
                        atlasTexture[(destinationOffset * 4 + 3)] = alphaComponent;
                        destinationOffset++;
                    }
                }

                SDL_FreeSurface(glyphSurface);

                currentX += srcRect.w;
            }
        }

        void dump() {

//            for (auto& kv : glyphs) {
//                GlyphInfo& glyphInfo = kv.second;
//                for (int i = 0; i < glyphInfo.w; i++) {
//                    int offset = (glyphInfo.y*atlasWidth + i + glyphInfo.x) * 4;
//                    atlasTexture[offset + 0] = 255;
//                    atlasTexture[offset + 1] = 255;
//                    atlasTexture[offset + 2] = 255;
//                    atlasTexture[offset + 3] = 255;
//                }
//
//                for (int i = 0; i < glyphInfo.h; i++) {
//                    int offset = ((glyphInfo.y + i)*atlasWidth + glyphInfo.x) * 4;
//                    atlasTexture[offset + 0] = 255;
//                    atlasTexture[offset + 1] = 255;
//                    atlasTexture[offset + 2] = 255;
//                    atlasTexture[offset + 3] = 255;
//                }
//            }

            cout << "atlas size is " << atlasWidth << "x" << atlasHeight << endl;
            ofstream file("font.raw");
            file.write((const char*) atlasTexture, atlasWidth * atlasHeight * 4);
            file.close();
        }

        int atlasWidth = 16;
        int atlasHeight = 16;
        int currentX = 0;
        int currentY = 0;
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

        void addGlyphToAtlas(uint16 asciiChar) {
            atlas.add(asciiChar, font);
        }

        void buildGlyphAtlas() {
            for (uint8 glyph = 32; glyph < 127; ++glyph) {
                uint16 asciiChar = static_cast<uint16>(glyph);
                if (TTF_GlyphIsProvided(font, asciiChar)) {
                    addGlyphToAtlas(glyph);
                } else {
                    LOG(ERROR) << "Font " << fontMetrics.family << " does not have glyph " << asciiChar << " provided";
                }
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
            buildGlyphAtlas();
        }

        void addCharToVbo(const char& charToRender, const vec4& color) {
            GlyphInfo glyphInfo = atlas.glyphs[charToRender];

            if (previousChar != 0) {
                penPosition.x += atlas.glyphs[previousChar].advance;

                if (FT_HAS_KERNING(face)) {
                    FT_UInt previousGlyphIndex = FT_Get_Char_Index(face, previousChar);
                    FT_UInt currentGlyphIndex = FT_Get_Char_Index(face, charToRender);

//                    LOG(INFO) << "Attempting to retrieve kerning info between glyph indices " << previousGlyphIndex << " and " << currentGlyphIndex;

                    if (previousGlyphIndex != 0 && currentGlyphIndex != 0) {

                        FT_Vector kerningInfo;
                        FT_Error error = FT_Get_Kerning(face, previousGlyphIndex, currentGlyphIndex, FT_KERNING_DEFAULT, &kerningInfo);
                        if (error) {
                            LOG(ERROR) << "Failed to get kerning info between " << to_string(previousChar) << " and " << to_string(charToRender);
                        } else {
                            penPosition.x += kerningInfo.x >> 6; // 2^6 = 64
                            if (kerningInfo.x != 0) {
                                LOG(INFO) << "Kerning info between [" << previousChar << "] and [" << charToRender << "] is " << (kerningInfo.x >> 6);
                            }
                        }
                    }
                } else {

                }


//                int kerning = TTF_GetFontKerningSize(font, previousChar, charToRender);
//                penPosition.x += kerning;
//                cout << "Kerning between (" << to_string(previousChar) << ", " << to_string(charToRender) << ") = " << kerning << endl;
            }

            int penX = round(penPosition.x);
            int penY = round(penPosition.y);
            vector<vec2> verts = {
                    {penX,               penY},
                    {penX + glyphInfo.w, penY},
                    {penX + glyphInfo.w, penY + glyphInfo.h},
                    {penX,               penY + glyphInfo.h}
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
                addCharToVbo(*it, color);
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
        string fontName = "../data/fonts/arial.ttf";
        int fontSize = 16;

        newFont = new TtfFont(fontName, fontSize);
        fontTexture = newFont->getTexture();

        FT_Error error = FT_Init_FreeType(&library);
        if (error) {
            LOG(ERROR) << "Failed to initialize freetype lib";
        } else {
            LOG(INFO) << "Freetype initialized OK";
        }

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



//        if (TTF_Init() == -1) {
//            LOG(ERROR) << "Failed to initialize sdl ttf";
//        }
//        font = TTF_OpenFont("../data/fonts/DejaVuSans.ttf", 200);
//        if (font == nullptr) {
//            LOG(ERROR) << "Failed to open font";
//        }
//
//        TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
//        TTF_SetFontOutline(font, 1);
//
//        SDL_Color textColor;
//        textColor.r = 40;
//        textColor.g = 128;
//        textColor.b = 30;
//        textColor.a = 128;
//        SDL_Surface* surface = TTF_RenderUTF8_Blended(font, "Hello world, this is Tron!", textColor);
//        IMG_SavePNG(surface, "../printed.png");
//
//        glGenTextures(1, &fontTextureId);
//        CHECK_GL_ERROR();
//        glBindTexture(GL_TEXTURE_2D, fontTextureId);
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
//        CHECK_GL_ERROR();
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//        fontTexture = shared_ptr<Texture>(new Texture(fontTextureId, surface->w, surface->h));
//
//        SDL_FreeSurface(surface);

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

    void GameStateTest::update(Stardust* game, float elapsedSeconds) {
        DialogRepository::getInstance().updateAll(elapsedSeconds);

        if (INPUT.isKeyJustPressed(SDL_SCANCODE_ESCAPE)) {
            game->quitGame = true;
        }
    }

    void GameStateTest::render(Stardust* game, float alpha) {
        GFXSYS.clearScreen();

//        DialogRepository::getInstance().renderAll();

        GFXSYS.setTransparencyMode(TransparencyMode::Transparent);

        gpuProgram->use();
        fontTexture->useForUnit(0);

        newFont->print(0, 0, "OWVATAWVTijLorem ipsum dolor sit amet, consectetur adipiscing elit. Mauris sit amet eros sem. Integer porttitor ac nunc et consequat.", vec4(1, 0.5, 0.2, 0.9));
//        GFXSYS.renderFullScreenTexturedQuad();
        gpuProgram->unuse();

//        glEnable(GL_TEXTURE_2D);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, fontTexture);
//        glColor4f(1, 1, 1, 1);
//        glBegin(GL_QUADS);
//        {
//            const float scale = 1.f;
//
//            glTexCoord2f(0, 0);
//            glVertex2f(0, 0);
//
//            glTexCoord2f(1, 0);
//            glVertex2f(1024/2, 0);
//
//            glTexCoord2f(1, 1);
//            glVertex2f(1024/2, 768/2);
//
//            glTexCoord2f(0, 1);
//            glVertex2f(0, 768/2);
//        }
//        glEnd();


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

        // for versions up to 3
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

        return std::shared_ptr<Texture>(new Texture(textureId, atlas.atlasWidth, atlas.atlasHeight));
    }
} // namespace Acidrain
