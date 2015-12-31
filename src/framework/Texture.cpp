#include <Texture.h>

namespace Acidrain {

    Texture::Texture(int w, int h, unsigned char* buffer)
            : width(w), height(h), destroyable(true) {

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // only set this when we do have mipmaps
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                width, height,
                0, GL_RGBA, GL_UNSIGNED_BYTE,
                buffer);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    Texture::Texture(GLuint textId, int w, int h)
            : width(w), height(h), textureId(textId), destroyable(false) {
    }

    Texture::~Texture() {
        if (destroyable)
            glDeleteTextures(1, &textureId);
    }

    int Texture::getWidth() const {
        return width;
    }

    int Texture::getHeight() const {
        return height;
    }

    void Texture::use() const {
        glBindTexture(GL_TEXTURE_2D, textureId);
    }

    void Texture::useForUnit(int unit) const {
//        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0 + unit);
        use();
    }

    void Texture::unuse() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

} // namespace Acidrain
