#include <Sprite.h>

namespace Acidrain {

    void SpriteSheet::addSprite(int top, int left, int width, int height) {
        SpriteInfo info;

        Box b = Box(width, height);

        info.pixelCoords = b;
        info.pixelCoords.translate(vec2(left, top) + vec2(width, height) / 2.0f);

        info.texCoords = info.pixelCoords;
        info.texCoords.normalizeForSize(texture->getWidth(), texture->getHeight());

        info.width = width;
        info.height = height;

        sprites.push_back(info);
    }

    void SpriteSheet::autoAdd(int width, int height, int strideX, int strideY) {
        int y = strideY;
        while (y <= (texture->getHeight() - height)) {
            int x = strideX;
            while (x <= (texture->getWidth() - width)) {
                addSprite(y, x, width, height);
                x += width + strideX;
            }
            y += height + strideY;
        }
    }

    Texture* Sprite::getTexture() const {
        return spriteSheet->texture.get();
    }

    vector<vec2> Sprite::getTexCoords() const {
        const Box& texCoords = spriteSheet->sprites[spriteIndex].texCoords;
        return vector<vec2> {
                vec2(texCoords.left(), texCoords.top()),
                vec2(texCoords.right(), texCoords.top()),
                vec2(texCoords.right(), texCoords.bottom()),
                vec2(texCoords.left(), texCoords.bottom())
        };
    }

    SpriteInfo& Sprite::getSpriteInfo() const {
        return spriteSheet->sprites[spriteIndex];
    }

    vec2 Sprite::getSize() const {
        const SpriteInfo& info = spriteSheet->sprites[spriteIndex];
        return vec2(
                info.width,
                info.height
        );
    }
} // namespace Acidrain

