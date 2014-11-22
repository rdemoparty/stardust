#pragma once

#include <SDL2/SDL.h>

namespace Acidrain {

    enum class WindowType {
        Windowed,
        Fullscreen
    };

    class Window {
    public:
        Window(int width, int height, WindowType type);

        virtual ~Window();

        void present();

        int width();

        int height();

    private:

        int width_;
        int height_;
        WindowType type;

        SDL_Window* displayWindow;
        SDL_GLContext glContext;
    };

} // namespace Acidrain
