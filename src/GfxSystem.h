#pragma once
#include <memory>
#include <Texture.h>

#define GFXSYS Acidrain::GfxSystem::getInstance()

namespace Acidrain {

    class GfxSystem {
    public:

        static GfxSystem &getInstance();

        void init(const int windowWidth, const int windowHeight, const int desiredWidth, const int desiredHeight);

        std::shared_ptr<Texture> loadTexture(const char* filename);

    private:

        int width, height;
        int offsetX, offsetY;
    };
} // namespace Acidrain

