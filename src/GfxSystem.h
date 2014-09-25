#pragma once
#include <memory>
#include <Texture.h>
#include <glm/vec3.hpp>

#define GFXSYS Acidrain::GfxSystem::getInstance()

namespace Acidrain {

    class GfxSystem {
    public:

        static GfxSystem &getInstance();

        void init(const int windowWidth, const int windowHeight, const int desiredWidth, const int desiredHeight);

        std::shared_ptr<Texture> loadTexture(const char* filename);

        std::shared_ptr<Texture> loadTexture(const std::string& filename);

        void setClearColor(const glm::vec3& color);

        void clearScreen();

    private:

        int width, height;
        int offsetX, offsetY;
        glm::vec3 clearScreenColor = glm::vec3(0);
    };
} // namespace Acidrain

