#include <Sprite.h>

namespace Acidrain {

    void SpriteSheet::addSprite(int top, int left, int width, int height) {
        SpriteInfo info;

        Box b = Box(width, height);

        info.pixelCoords = b;
        info.pixelCoords.translate(glm::vec2(left, top) + glm::vec2(width, height) / 2.0f);

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

} // namespace Acidrain

