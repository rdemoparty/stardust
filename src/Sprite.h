#pragma once

#include <memory>
#include <vector>
#include <Box.h>
#include <Texture.h>

namespace Acidrain {

    using namespace std;

    struct SpriteInfo {
        Box texCoords;
        Box pixelCoords;
        int width;
        int height;
    };

    struct SpriteSheet {
        shared_ptr <Texture> texture;
        vector <SpriteInfo> sprites;

        void addSprite(int top, int left, int width, int height);
        void autoAdd(int width, int height, int strideX=0, int strideY=0);
    };

    struct Sprite {
        shared_ptr <SpriteSheet> spriteSheet;
        int spriteIndex;
    };
} // namespace Acidrain