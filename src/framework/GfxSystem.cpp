#include <GfxSystem.h>
#include <GLheaders.h>
#include <Window.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include <FileSystem.h>
#include <easylogging++.h>
#include <CommandLineParser.h>

DEFINE_int(width, w, "The physical window width", -1)
DEFINE_int(height, h, "The physical window height", -1)
DEFINE_bool(fullscreen, f, "Whether the window is fullscreen or not", false)
DEFINE_bool(vsync, v, "Whether vsync is enabled or not", false)

namespace Acidrain {

    GfxSystem& GfxSystem::getInstance() {
        static GfxSystem instance;
        return instance;
    }

    void GfxSystem::init(const int desiredWidth, const int desiredHeight) {
        SDL_DisplayMode displayMode = establishDisplayMode();

        window = make_shared<Window>(
                displayMode.w,
                displayMode.h,
                FLAG_vsync,
                FLAG_fullscreen ?
                        WindowType::Fullscreen :
                        WindowType::Windowed
        );

        const int windowWidth = window->width();
        const int windowHeight = window->height();

        const float windowAspectRatio = windowWidth / static_cast<float>(windowHeight);
        const float wantedAspectRatio = desiredWidth / static_cast<float>(desiredHeight);

        if (wantedAspectRatio < windowAspectRatio) {
            height = windowHeight;
            width = (int) (height * wantedAspectRatio);
        } else {
            width = windowWidth;
            height = (int) (width / wantedAspectRatio);
        }

        offsetX = (windowWidth - width) / 2;
        offsetY = (windowHeight - height) / 2;

        // clear screen before setting scissors so we remove the possible garbage
        glClearColor(0, 0, 0, 1);
        setClearColor(vec3(0));
        clearScreen();

        glViewport(offsetX, offsetY, width, height);
        glScissor(offsetX, offsetY, width, height);

        glEnable(GL_SCISSOR_TEST);
        glDisable(GL_DEPTH_TEST);

        glMatrixMode(GL_PROJECTION);
        glOrtho(0, desiredWidth, desiredHeight, 0, 0, 1);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // trick for exact pixelization
//        glTranslatef(0.375, 0.375, 0);
    }

    SDL_DisplayMode GfxSystem::establishDisplayMode() {
        SDL_DisplayMode displayMode;
        displayMode.w = FLAG_width;
        displayMode.h = FLAG_height;

        if (displayMode.w == -1 || displayMode.h == -1) {
            LOG(INFO) << "Window size not specified. Proceeding with autodetection";

            if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0)
                LOG(FATAL) << "SDL_GetCurrentDisplayMode failed: " << SDL_GetError();

            LOG(INFO) << "Got current display mode of " << displayMode.w << "x" << displayMode.h;
        }

        return displayMode;
    }

    shared_ptr<Texture> GfxSystem::loadTexture(const string& filename) {
        return loadTexture(filename.c_str());
    }

    shared_ptr<Texture> GfxSystem::loadTexture(const char* filename) {
        string content = FILESYS.getFileContent(filename);
        int w, h, comp;
        unsigned char* image = stbi_load_from_memory((stbi_uc const*) content.c_str(), (int) content.size(), &w, &h, &comp, STBI_rgb_alpha);

        // pre-multiply RGB with alpha
        for (int y = 0; y < h; y++)
            for (int x = 0; x < w; x++) {
                image[(x + y * w) * 4 + 0] = (image[(x + y * w) * 4 + 0] * image[(x + y * w) * 4 + 3]) >> 8;
                image[(x + y * w) * 4 + 1] = (image[(x + y * w) * 4 + 1] * image[(x + y * w) * 4 + 3]) >> 8;
                image[(x + y * w) * 4 + 2] = (image[(x + y * w) * 4 + 2] * image[(x + y * w) * 4 + 3]) >> 8;
            }

        LOG(INFO) << "Loading texture \"" << filename << "\" (" << w << "x" << h << ")";

        shared_ptr<Texture> result = make_shared<Texture>(w, h, image);
        stbi_image_free(image);
        LOG(TRACE) << "Texture loaded ok.";
        return result;
    }

    void GfxSystem::setClearColor(const vec3& color) {
        clearScreenColor = color;
        glClearColor(clearScreenColor.r, clearScreenColor.g, clearScreenColor.b, 0.0f);
    }

    void GfxSystem::clearScreen() {
        // clear whole screen with bands color
        glViewport(0, 0, window->width(), window->height());
        glScissor(0, 0, window->width(), window->height());

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        // clear only drawable area with set colour
        glViewport(offsetX, offsetY, width, height);
        glScissor(offsetX, offsetY, width, height);

        glClearColor(clearScreenColor.r, clearScreenColor.g, clearScreenColor.b, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void GfxSystem::show() {
        if (window)
            window->present();
    }

    void GfxSystem::drawCircle(const vec2& center, float radius, const vec4& color) {
        const int SEGMENTS = 20;
        glLineWidth(2);
        glEnable(GL_LINE_SMOOTH);
        glBegin(GL_LINE_STRIP);
        {
            glColor4f(color.r, color.g, color.b, color.a);
            for (int i = 0; i <= SEGMENTS; i++)
                glVertex2d(sin(i * 2 * M_PI / SEGMENTS) * radius + center.x, cos(i * 2 * M_PI / SEGMENTS) * radius + center.y);
        }
        glEnd();
    }

    void GfxSystem::drawFilledRectangle(const vec2& topLeft, const vec2& bottomRight, const vec4& color) {
        glBegin(GL_QUADS);
        {
            glColor4f(color.r, color.g, color.b, color.a);
            glVertex2d(topLeft.x, topLeft.y);
            glVertex2d(bottomRight.x, topLeft.y);
            glVertex2d(bottomRight.x, bottomRight.y);
            glVertex2d(topLeft.x, bottomRight.y);
        }
        glEnd();
    }

    void GfxSystem::drawSprite(const Sprite& sprite, const vec2& position) {
        glEnable(GL_TEXTURE_2D);
        sprite.getTexture()->use();

        const SpriteInfo& spriteInfo = sprite.getSpriteInfo();

        glBegin(GL_QUADS);
        {
            glTexCoord2f(spriteInfo.texCoords.left(), spriteInfo.texCoords.bottom());
            glVertex2f(position.x, position.y);

            glTexCoord2f(spriteInfo.texCoords.left(), spriteInfo.texCoords.top());
            glVertex2f(position.x, spriteInfo.height + position.y);

            glTexCoord2f(spriteInfo.texCoords.right(), spriteInfo.texCoords.top());
            glVertex2f(position.x + spriteInfo.width, spriteInfo.height + position.y);

            glTexCoord2f(spriteInfo.texCoords.right(), spriteInfo.texCoords.bottom());
            glVertex2f(position.x + spriteInfo.width, position.y);
        }
        glEnd();
    }

    void GfxSystem::setTransparencyMode(TransparencyMode mode) {
        switch (mode) {
            case TransparencyMode::Opaque:
                glDisable(GL_BLEND);
                break;
            case TransparencyMode::Transparent:
                glEnable(GL_BLEND);
                glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
                glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
                break;
            case TransparencyMode::Additive:
                glEnable(GL_BLEND);
                glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
                glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ZERO);
                break;
            case TransparencyMode::Special:
                glEnable(GL_BLEND);
//                glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
                glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
                break;
        }
    }

    int GfxSystem::windowWidth() const {
        return window->width();
    }

    int GfxSystem::windowHeight() const {
        return window->height();
    }

    int GfxSystem::drawableWidth() const {
        return width;
    }

    int GfxSystem::drawableHeight() const {
        return height;
    }

    void GfxSystem::drawLine(const vec2& from, const vec2& to, const vec4& color) {
        glLineWidth(2);
        glEnable(GL_LINE_SMOOTH);
        glBegin(GL_LINES);
        {
            glColor4f(color.r, color.g, color.b, color.a);
            glVertex2d(from.x, from.y);

            glColor4f(color.r, color.g, color.b, color.a);
            glVertex2d(to.x, to.y);
        }
        glEnd();

    }
} // namespace Acidrain
