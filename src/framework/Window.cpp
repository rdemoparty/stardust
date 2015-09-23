#ifdef __APPLE__
    #define NO_SDL_GLEXT
#endif
#include <Window.h>
#include <GLheaders.h>
#include <easylogging++.h>

#ifdef _WIN32
	#include <SDL_syswm.h>
	#include <windows.h>
#endif

namespace Acidrain {

    Window::Window(int w, int h, bool vsyncEnabled, WindowType t)
            : width_(w), height_(h), type_(t) {

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
        if (type_ == WindowType::Fullscreen)
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

    void Window::resize(int width, int height, WindowType type) {
        this->width_ = width;
        this->height_ = height;
        this->type_ = type;
        SDL_DestroyWindow(displayWindow);

        Uint32 windowFlags = SDL_WINDOW_OPENGL;
        if (type == WindowType::Fullscreen)
            windowFlags |= SDL_WINDOW_FULLSCREEN;

        displayWindow = SDL_CreateWindow("Stardust",
                                         SDL_WINDOWPOS_UNDEFINED,
                                         SDL_WINDOWPOS_UNDEFINED,
                                         width_,
                                         height_,
                                         windowFlags);

        SDL_GL_MakeCurrent(displayWindow, glContext);
    }

    void Window::present() {
        SDL_GL_SwapWindow(displayWindow);
    }

    int Window::width() const {
        return width_;
    }

    int Window::height() const {
        return height_;
    }

    #if defined _WIN32 || defined _WIN64
    // Thank you nekitu for finding this hack!
    void ActivateWindowHandle(HWND hWnd) {
        DWORD threadId1 = GetWindowThreadProcessId(GetForegroundWindow(), NULL);
        DWORD threadId2 = GetWindowThreadProcessId(hWnd, NULL);

        if (threadId1 != threadId2) {
            AttachThreadInput(threadId1, threadId2, 1);
            SetForegroundWindow(hWnd);
            AttachThreadInput(threadId1, threadId2, 0);
        } else {
            SetForegroundWindow(hWnd);
        }

        if (IsIconic(hWnd)) {
            ShowWindow(hWnd, SW_RESTORE);
        } else {
            ShowWindow(hWnd, SW_SHOW);
        }
    }
    #endif

    void Window::raise() {
        #if defined _WIN32 || defined _WIN64
            struct SDL_SysWMinfo wmInfo;
            SDL_VERSION(&wmInfo.version);
            SDL_GetWindowWMInfo(displayWindow, &wmInfo);

            HWND hWnd = wmInfo.info.win.window;

            // The section below raise the window in Wine. It is not needed for true windows behaviour
            // but it is nice to also have it if running in Wine. One thing worth noting is that
            // we do not get window focus in Wine.
            SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
            SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
            SwitchToThisWindow(hWnd, TRUE);

            // This makes it raise and get focus in Windows
            ActivateWindowHandle(hWnd);
        #else
            SDL_RaiseWindow(displayWindow);
        #endif
    }

    WindowType Window::type() const {
        return type_;
    }
} // namespace Acidrain
