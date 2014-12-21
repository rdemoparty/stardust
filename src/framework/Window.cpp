#ifdef __APPLE__
    #define NO_SDL_GLEXT
#endif

#include <Window.h>
#include <GLheaders.h>
//#include <SDL2/SDL_opengl.h>
#include <iostream>

namespace Acidrain {

    Window::Window(int w, int h, WindowType t)
            : width_(w), height_(h), type(t) {

        // Setup OpenGL version. on OSX this has the effect of not drawing anything with them shaders. Need to investigate further
#ifndef __APPLE__
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
//        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif

        // setup stencil buffer
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        // use double buffering
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        // setup depth buffer
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        // setup anti aliasing
        // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);


        Uint32 windowFlags = SDL_WINDOW_OPENGL;
        if (type == WindowType::Fullscreen)
            windowFlags |= SDL_WINDOW_FULLSCREEN;

        displayWindow = SDL_CreateWindow("Stardust",
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                width_,
                height_,
                windowFlags);
        glContext = SDL_GL_CreateContext(displayWindow);

#ifndef __APPLE__
        GLenum err = glewInit();
        if (GLEW_OK != err) {
            std::cerr << "Error " << glewGetErrorString(err) << std::endl;
            exit(1);
        }
#endif

        SDL_GL_SetSwapInterval(0);
    }

    Window::~Window() {
        // restore our video mode
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(displayWindow);
        SDL_Quit();
    }

    void Window::present() {
        SDL_GL_SwapWindow(displayWindow);
    }

    int Window::width() {
        return width_;
    }

    int Window::height() {
        return height_;
    }
} // namespace Acidrain
