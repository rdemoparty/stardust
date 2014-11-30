#pragma once

#include <memory>
#include <Texture.h>
#include <Window.h>
#include <glm/vec3.hpp>
#include <map>
#include <string>
#include <Entity.h>

#define GFXSYS Acidrain::GfxSystem::getInstance()

namespace Acidrain {

    enum class TransparencyMode {
        Opaque,
        Transparent,
        Additive
    };

    class GfxSystem {
    public:

        static GfxSystem& getInstance();

        void init(const int desiredWidth, const int desiredHeight);

        std::shared_ptr<Texture> loadTexture(const char* filename);

        std::shared_ptr<Texture> loadTexture(const std::string& filename);

        void setTransparencyMode(TransparencyMode mode);

        void drawCircle(const glm::vec2& center, float radius, const glm::vec4& color);

        void drawFilledRectangle(const glm::vec2& topLeft, const glm::vec2& bottomRight, const glm::vec4& color);

        void drawSprite(const Sprite& sprite, const vec2& position);

        void setClearColor(const glm::vec3& color);

        void clearScreen();

        void show();

        int windowWidth() const;

        int windowHeight() const;

        int drawableWidth() const;

        int drawableHeight() const;

    private:
        std::shared_ptr<Window> window;
        int width, height;
        int offsetX, offsetY;
        glm::vec3 clearScreenColor = glm::vec3(0);
    };
} // namespace Acidrain

