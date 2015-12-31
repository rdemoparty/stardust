#pragma once

#include <memory>
#include <Texture.h>
#include <glm/vec3.hpp>
#include <map>
#include <string>
#include <Entity.h>
#include <SDL_video.h>

#define GFXSYS Acidrain::GfxSystem::getInstance()

namespace Acidrain {

    using namespace std;
    using namespace glm;

    class Window;
    class Vbo;
    class GpuProgram;
    class GpuProgramConstantBundle;

    enum class TransparencyMode {
        Opaque,
        Transparent,
        Additive,
        Special
    };

    class GfxSystem {
    public:
        static GfxSystem& getInstance();

        ~GfxSystem();

        void init(const int windowWidth, const int windowHeight, bool fullscreen, bool vsync, const int desiredWidth, const int desiredHeight);

        void resizeDisplayTo(int width, int height, bool fullscreen);

        shared_ptr<Texture> loadTexture(const char* filename);

        shared_ptr<Texture> loadTexture(const string& filename);

        void setTransparencyMode(TransparencyMode mode);

        void drawLine(const vec2& from, const vec2& to, const vec4& color);

        void drawCircle(const vec2& center, float radius, const vec4& color);

        void drawFilledRectangle(const vec2& topLeft, const vec2& bottomRight, const vec4& color);

        void drawSprite(const Sprite& sprite, const vec2& position);

        void drawSprite(const Sprite& sprite, const vec2& position, const vec4& color);

        void renderFullScreenTexturedQuad();

        void setClearColor(const vec3& color);

        void clearScreen();

        void show();

        void setViewport();

        void raiseWindow() const;

        int windowWidth() const;

        int windowHeight() const;

        int drawableWidth() const;

        int drawableHeight() const;

        int logicalWidth() const;

        int logicalHeight() const;

        int getOffsetX() const;

        int getOffsetY() const;

        bool isFullscreen() const;

        bool isVSyncOn() const;

        void setVSync(bool state);
    private:

        void initializeFullScreenQuadVbo();

        shared_ptr<Window> window;
        shared_ptr<Vbo> fullScreenQuadVbo;

        shared_ptr<GpuProgram> spriteShader;
        shared_ptr<GpuProgram> solidShader;
        shared_ptr<GpuProgramConstantBundle> shaderConstants;

        int width, height;
        int offsetX, offsetY;
        int desiredWidth, desiredHeight;

        vec3 clearScreenColor = vec3(0);
    };
} // namespace Acidrain

