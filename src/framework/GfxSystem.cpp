#include <GfxSystem.h>
#include <GLheaders.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include "FileSystem.h"

namespace Acidrain {

    GfxSystem& GfxSystem::getInstance() {
        static GfxSystem instance;
        return instance;
    }

    void GfxSystem::init(const int desiredWidth, const int desiredHeight) {

        SDL_DisplayMode desktopDisplayMode;
        if (SDL_GetDesktopDisplayMode(0, &desktopDisplayMode) != 0) {
            SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
            exit(1);
        }

        window = std::shared_ptr<Window>(new Window(desktopDisplayMode.w, desktopDisplayMode.h, WindowType::Fullscreen));

        const int windowWidth = window->width();
        const int windowHeight = window->height();

        const float windowAspectRatio = windowWidth / static_cast<float>(windowHeight);
        const float wantedAspectRatio = desiredWidth / static_cast<float>(desiredHeight);

        if (wantedAspectRatio < windowAspectRatio) {
            height = windowHeight;
            width = height * wantedAspectRatio;
        } else {
            width = windowWidth;
            height = width / wantedAspectRatio;
        }

        offsetX = (windowWidth - width) / 2;
        offsetY = (windowHeight - height) / 2;

        // clear screen before setting scissors so we remove the possible garbage
        glClearColor(0, 0, 0, 1);
        setClearColor(glm::vec3(0));
        clearScreen();

        glViewport(offsetX, offsetY, width, height);
        glScissor(offsetX, offsetY, width, height);

        glEnable(GL_SCISSOR_TEST);
        glDisable(GL_DEPTH_TEST);

        glMatrixMode(GL_PROJECTION);
//        glOrtho(0, desiredWidth, 0, desiredHeight, 0, 1);
        glOrtho(0, desiredWidth, desiredHeight, 0, 0, 1);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // trick for exact pixelization
        glTranslatef(0.375, 0.375, 0);
    }

    std::shared_ptr<Texture> GfxSystem::loadTexture(const std::string& filename) {
        return loadTexture(filename.c_str());
    }

    std::shared_ptr<Texture> GfxSystem::loadTexture(const char* filename) {
        std::string content = FILESYS.getFileContent(filename);
        int w, h, comp;
        unsigned char* image = stbi_load_from_memory((stbi_uc const*) content.c_str(), content.size(), &w, &h, &comp, STBI_rgb_alpha);

        std::cout << "Loading texture " << filename << " with size: " << w << "x" << h << std::endl;
        std::shared_ptr<Texture> result = std::shared_ptr<Texture>(new Texture(w, h, image));
        stbi_image_free(image);

        return result;
    }


    void GfxSystem::setClearColor(const glm::vec3& color) {
        clearScreenColor = color;
        glClearColor(clearScreenColor.r, clearScreenColor.g, clearScreenColor.b, 0.0f);
    }

    void GfxSystem::clearScreen() {
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void GfxSystem::show() {
        if (window)
            window->present();
    }

    void GfxSystem::drawCircle(const glm::vec2& center, float radius, const glm::vec4& color) {
        const int SEGMENTS = 20;
        glLineWidth(3);
        glEnable(GL_LINE_SMOOTH);
        glBegin(GL_LINE_STRIP);
        glColor4f(color.r, color.g, color.b, color.a);
        for (int i = 0; i <= SEGMENTS; i++) {
            glVertex2d(sin(i * 2 * M_PI / SEGMENTS) * radius + center.x, cos(i * 2 * M_PI / SEGMENTS) * radius + center.y);
        }
        glEnd();
    }

    void GfxSystem::drawFilledRectangle(const glm::vec2& topLeft, const glm::vec2& bottomRight, const glm::vec4& color) {
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
        sprite.spriteSheet->texture->use();

        const SpriteInfo& spriteInfo = sprite.spriteSheet->sprites[sprite.spriteIndex];

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
        }
    }



} // namespace Acidrain

