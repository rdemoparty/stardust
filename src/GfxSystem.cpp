#include <GfxSystem.h>
#include <GLheaders.h>
#include <SDL_surface.h>
#include <SDL2/SDL_image.h>

namespace Acidrain {

    GfxSystem &GfxSystem::getInstance() {
        static GfxSystem instance;
        return instance;
    };

    void GfxSystem::init(const int windowWidth, const int windowHeight, const int desiredWidth, const int desiredHeight) {

        const float windowAspectRatio = windowWidth / static_cast<float>(windowHeight);
        const float wantedAspectRatio = desiredWidth / static_cast<float>(desiredHeight);

        if (wantedAspectRatio < windowAspectRatio) {
            height = windowHeight;
            width = height * wantedAspectRatio;
        } else {
            width = windowWidth;
            height = width / wantedAspectRatio;
        }

        offsetX = (windowWidth - width) / 2;
        offsetY = (windowHeight - height) / 2;

        glViewport(offsetX, offsetY, width, height);
        glScissor(offsetX, offsetY, width, height);

        glEnable(GL_SCISSOR_TEST);
        glDisable(GL_DEPTH_TEST);

        glMatrixMode(GL_PROJECTION);
        glOrtho(0, desiredWidth, 0, desiredHeight, 0, 1);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // trick for exact pixelization
        glTranslatef(0.375, 0.375, 0);
    };


    std::shared_ptr<Texture> GfxSystem::loadTexture(const char *filename) {
        SDL_Surface *image = IMG_Load(filename);
        std::shared_ptr<Texture> result = std::shared_ptr<Texture>(new Texture(image->w, image->h, reinterpret_cast<unsigned char *>(image->pixels)));
        SDL_FreeSurface(image);
        return result;
    }
} // namespace Acidrain

