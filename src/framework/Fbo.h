#pragma once

#include <GLheaders.h>
#include <Texture.h>
#include <memory>

namespace Acidrain {

    using namespace std;

    class Fbo {
    public:
        explicit Fbo(int width, int height);
        virtual ~Fbo();

        void use();
        void unuse();

        int getWidth() const;
        int getHeight() const;

        GLuint getColorBufferId() const;
        GLuint getDepthBufferId() const;

        shared_ptr<Texture> getTexture();
    private:
        int width;
        int height;

        GLuint frameBufferId;
        GLuint colorBufferId;
        GLuint depthBufferId;
    };

} // namespace Acidrain