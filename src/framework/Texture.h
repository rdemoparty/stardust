#pragma once

#include <GLheaders.h>

namespace Acidrain {

    class Texture {
    public:
        Texture(int width, int height, unsigned char* buffer);
        Texture(GLuint textureId, int width, int height);
        virtual ~Texture();

        int getWidth() const;
        int getHeight() const;

        void use() const;
        void useForUnit(int unit) const;
        void unuse() const;

    private:

        int width;
        int height;
        GLuint textureId = 0;
        bool destroyable;
    };

} // namespace Acidrain
