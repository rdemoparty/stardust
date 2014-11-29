#pragma once

#include <memory>
#include <vector>
#include <Box.h>
#include <Texture.h>

namespace Acidrain {

    using namespace std;
    using namespace glm;

    struct SpriteInfo {
        Box texCoords;
        Box pixelCoords;
        int width;
        int height;
    };

    struct SpriteSheet {
        shared_ptr<Texture> texture;
        vector<SpriteInfo> sprites;

        void addSprite(int top, int left, int width, int height);

        void autoAdd(int width, int height, int strideX = 0, int strideY = 0);
    };

    class Sprite {
    public:

        Sprite()
                : spriteSheet(nullptr), spriteIndex(0) {
        }

        Sprite(SpriteSheet *sprSheet, int index)
                : spriteSheet(sprSheet), spriteIndex(index) {
        }

        Texture *getTexture() const;

        vector<vec2> getTexCoords() const;

        SpriteInfo &getSpriteInfo() const;

        vec2 getSize() const;

    private:
        SpriteSheet *spriteSheet;
        int spriteIndex;
    };
} // namespace Acidrain