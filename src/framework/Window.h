#pragma once

#include <SDL.h>

namespace Acidrain {

    enum class WindowType {
        Windowed,
        Fullscreen
    };

    class Window {
    public:
        Window(int width, int height, bool vsyncEnabled, WindowType type);

        virtual ~Window();

        void resize(int width, int height, WindowType type);

        void present();

        int width() const;

        int height() const;

        WindowType type() const;

        bool vSync() const;

        void vSync(bool state);

        void raise();

    private:

        int width_;
        int height_;
        WindowType type_;
        bool vSync_;

        SDL_Window* displayWindow;
        SDL_GLContext glContext;
    };

} // namespace Acidrain
