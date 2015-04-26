#ifdef __APPLE__
    #define NO_SDL_GLEXT
#endif
#include <Window.h>
#include <GLheaders.h>
#include <easylogging++.h>

#ifdef _WIN32
	#include <windows.h> // BringWindowToTop()
#endif

namespace Acidrain {

    Window::Window(int w, int h, bool vsyncEnabled, WindowType t)
            : width_(w), height_(h), type(t) {

        LOG(INFO) << "Attempting to create window of " << w << "x" << h << ". Vsync enabled: " << vsyncEnabled;

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
        LOG(TRACE) << "Initializing glew";
        GLenum err = glewInit();
        if (GLEW_OK != err) {
            LOG(FATAL) << "Failed to initialize glew. Reason: " << glewGetErrorString(err);
        }
#endif

        SDL_GL_SetSwapInterval(vsyncEnabled ? 1 : 0);
    }

    Window::~Window() {
        LOG(INFO) << "Destroying window and restoring video mode";
        // restore our video mode
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(displayWindow);
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

    void Window::raise() {
        SDL_RaiseWindow(displayWindow);

        #if defined _WIN32 || defined _WIN64
            LOG(INFO) << "Raising window in Windows";

            BringWindowToTop(GetActiveWindow());
            SetWindowPos(GetActiveWindow(), 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
            SetFocus(GetActiveWindow());
        #endif
    }
} // namespace Acidrain
