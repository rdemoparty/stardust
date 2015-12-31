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
#include <CommandLineParser.h>

DEFINE_bool(ogl_debug, g, "Whether to enable OGL debug output or not", false)

namespace Acidrain {

    static std::map<GLenum, std::string> OGL_DEBUG_SOURCE_MAP = {
            {GL_DEBUG_SOURCE_API, "API"},
            {GL_DEBUG_SOURCE_WINDOW_SYSTEM, "WINDOW_SYSTEM"},
            {GL_DEBUG_SOURCE_SHADER_COMPILER, "SHADER_COMPILER"},
            {GL_DEBUG_SOURCE_THIRD_PARTY, "THIRD_PARTY"},
            {GL_DEBUG_SOURCE_APPLICATION, "APPLICATION"},
            {GL_DEBUG_SOURCE_OTHER, "OTHER"}
    };

    static std::map<GLenum, std::string> OGL_DEBUG_TYPE_MAP = {
            {GL_DEBUG_TYPE_ERROR, "ERROR"},
            {GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, "DEPRECATED_BEHAVIOR"},
            {GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, "UNDEFINED_BEHAVIOR"},
            {GL_DEBUG_TYPE_PORTABILITY, "PORTABILITY"},
            {GL_DEBUG_TYPE_PERFORMANCE, "PERFORMANCE"},
            {GL_DEBUG_TYPE_MARKER, "MARKER"},
            {GL_DEBUG_TYPE_PUSH_GROUP, "PUSH_GROUP"},
            {GL_DEBUG_TYPE_POP_GROUP, "POP_GROUP"},
            {GL_DEBUG_TYPE_OTHER, "OTHER"},
    };

    static std::map<GLenum, std::string> OGL_DEBUG_SEVERITY_MAP = {
            {GL_DEBUG_SEVERITY_HIGH, "HIGH"},
            {GL_DEBUG_SEVERITY_MEDIUM, "MEDIUM"},
            {GL_DEBUG_SEVERITY_LOW, "LOW"},
            {GL_DEBUG_SEVERITY_NOTIFICATION, "NOTIFICATION"}
    };

    static void openGlDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
        if (severity != GL_DEBUG_SEVERITY_NOTIFICATION && severity != GL_DEBUG_SEVERITY_LOW) {
            LOG(WARNING) << "OGL >> " << OGL_DEBUG_SOURCE_MAP[source] << " [" << OGL_DEBUG_TYPE_MAP[type] << ", " << OGL_DEBUG_SEVERITY_MAP[severity] << "] " << std::string(message, length);
        }
    }

    Window::Window(int w, int h, bool vsyncEnabled, WindowType t)
            : width_(w), height_(h), type_(t) {

        LOG(INFO) << "Attempting to create window of " << w << "x" << h << ". Vsync enabled: " << vsyncEnabled;

        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        // Set up OGL 3.2 core profile. No more deprecated shit
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        // Set up context debug flag
        if (FLAG_ogl_debug) {
            int result;
            result = SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
            LOG(INFO) << "Setting up GL_CONTEXT_DEBUG_FLAG yielded " << result;
        }

//        // setup anti aliasing
//        int multiSampleBuffers = 1;
//        int multiSampleSamples = 4;
//        LOG(INFO) << "Attempting to set multi-sample buffers to " << multiSampleBuffers << " and multi-sample samples to " << multiSampleSamples;
//        result = SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, multiSampleBuffers);
//        LOG(INFO) << "setting multisample buffers returned " << result;
//        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, multiSampleSamples);
//        LOG(INFO) << "setting multisample samples returned " << result;
////        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);


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
        glEnable(GL_MULTISAMPLE);

//        int queriedMultiSampleBuffers;
//        int queriedMultiSampleSamples;
//        result = SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &queriedMultiSampleBuffers);
//        LOG(INFO) << "getting multisample buffers returned " << result;
//        result = SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &queriedMultiSampleSamples);
//        LOG(INFO) << "getting multisample buffers returned " << result;
//        LOG(INFO) << "Querying previously set values. Got multi-sample buffers set to " << queriedMultiSampleBuffers << " and multi-sample samples set to " << queriedMultiSampleSamples;


#ifndef __APPLE__
        LOG(TRACE) << "Initializing glew";
        // Thanks, derhass!
        // [17:49] <derhass> my thought is that you run into the GLEW bug with core profiles
        // [17:50] <benishor> derhass: sounds reasonable, if there is such a bug. any way to fix it?
        // [17:50] <derhass> benishor: did you set glewExpreimental = GL_TRUE before the glewInit()?
        // [17:50] <benishor> no, I did not
        // [17:50] <benishor> should I have?
        // [17:50] <derhass> yes
        // [17:51] <derhass> this is the "workaround" for GLEW in core profiles
        // [17:53] <derhass> note that glewExperimental is just a HACK
        // [17:53] <derhass> it will a) trigger a GL error itself during glewInit (which you should read away with glGetError)
        // [17:54] <derhass> and it will totally screw up the detection of the available extensions
        // [17:54] <derhass> benishor: http://stackoverflow.com/questions/22619406/why-does-glew-say-that-i-dont-have-extensions/22619644#22619644
        glewExperimental = GL_TRUE;
        GLenum err = glewInit();
        glGetError(); // see above
        if (GLEW_OK != err) {
            LOG(FATAL) << "Failed to initialize glew. Reason: " << glewGetErrorString(err);
        }
#endif

        if (GLEW_VERSION_1_1) {
            LOG(INFO) << "OpenGL Info";
            LOG(INFO) << "\tVersion: " << glGetString(GL_VERSION);
            LOG(INFO) << "\tVendor: " << glGetString(GL_VENDOR);
            LOG(INFO) << "\tRenderer: " << glGetString(GL_RENDERER);
            LOG(INFO) << "\tShading: " << glGetString(GL_SHADING_LANGUAGE_VERSION);

//            std::stringstream extensions;
//            int n;
//            glGetIntegerv(GL_NUM_EXTENSIONS, &n);
//            for (int i = 0; i < n; i++) {
//                extensions << glGetStringi(GL_EXTENSIONS, i) << " ";
//            }
//            LOG(INFO) << "\tExtensions: " << extensions.str();
        }


        // In an initialization routine
        if (FLAG_ogl_debug) {
            if (glDebugMessageCallback) {
                LOG(INFO) << "Installing OpenGL debug callback";
                glDebugMessageCallback(openGlDebugCallback, nullptr);
                glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
                glEnable(GL_DEBUG_OUTPUT);
                glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
                LOG(INFO) << "OpenGL debug callback installed";
            } else {
                LOG(WARNING) << "glDebugMessageCallback not available";
            }
        }

        vSync(vsyncEnabled);
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

    bool Window::vSync() const {
        return vSync_;
    }

    void Window::vSync(bool state) {
        vSync_ = state;
        SDL_GL_SetSwapInterval(vSync_ ? 1 : 0);
    }
} // namespace Acidrain
