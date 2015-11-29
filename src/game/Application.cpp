#include <easylogging++.h>
#include <CommandLineParser.h>
#include <Application.h>
#include <FileSystem.h>
#include <GfxSystem.h>
#include <AudioSystem.h>
#include <Version.h>
#include <UserPreferences.h>
#include <InputProvider.h>
#include <GameStateEditor.h>
#include <GameStateIntro.h>
#include <GameStateTest.h>
#include <ScriptingSystem.h>
#include <Timer.h>
#include <Stardust.h>

_INITIALIZE_EASYLOGGINGPP

DEFINE_string(log_level, l, "The log level", "debug")
DEFINE_bool(editor, e, "Whether to enter editor mode or not", false)
DEFINE_bool(test, t, "Whether to enter test mode or not", false)

namespace Acidrain {

    using namespace std;

    el::Level fromString(string levelName) {
        if (levelName == "debug")
            return el::Level::Debug;
        else if (levelName == "trace")
            return el::Level::Trace;
        else if (levelName == "info")
            return el::Level::Info;
        else if (levelName == "warning")
            return el::Level::Warning;
        else if (levelName == "warn")
            return el::Level::Warning;
        else if (levelName == "error")
            return el::Level::Error;
        else if (levelName == "fatal")
            return el::Level::Fatal;
        else
            return el::Level::Info;
    }


    Application::Application(int argc, char** argv) {
        ParseCommandLineFlags(argc, argv);
        configureLogging();
        initializeSystems();
    }

    Application::~Application() {
    }

    void Application::configureLogging() {
        el::Loggers::addFlag(el::LoggingFlag::HierarchicalLogging);
        el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
        el::Loggers::setLoggingLevel(fromString(FLAG_log_level));
    }

    void Application::initializeSystems() {

        const int GAME_LOGICAL_RESOLUTION_X = 1024;
        const int GAME_LOGICAL_RESOLUTION_Y = 768;

        LOG(INFO) << "STARDUST " << STARDUST_VERSION << " (" << STARDUST_TIMESTAMP << ")";

        EVENTSYS.init();
        USERPREFS.init();
        FILESYS.init(FLAG_data_dir);
        AUDIOSYS.init();
        GFXSYS.init(USERPREFS.width,
                    USERPREFS.height,
                    USERPREFS.fullscreen,
                    USERPREFS.vsync,
                    GAME_LOGICAL_RESOLUTION_X,
                    GAME_LOGICAL_RESOLUTION_Y);
        INPUT;
        SCRIPTSYS.init();
    }

    void Application::run() {
        timer = make_shared<Timer>();
        game = make_shared<Stardust>();

        if (FLAG_test) {
            game->fsm->setGlobalState(&GameStateTest::instance());
        } else if (FLAG_editor) {
            game->fsm->setGlobalState(&GameStateEditor::instance());
        } else {
            game->fsm->changeState(&GameStateIntro::instance());
        }

        double dt = 1.0 / 20.0;
        double accumulator = 0;

        LOG(INFO) << "Starting up application at logical resolution " << GFXSYS.logicalWidth() << "x" << GFXSYS.logicalHeight() << " with physics time step of " << dt << " seconds";

        while (!game->shouldQuit()) {
            float frameTime = timer->lap();
            if (frameTime > 0.25)
                frameTime = 0.25;

            accumulator += frameTime;
            while (accumulator >= dt) {
                game->update((float) dt);
                accumulator -= dt;
            }

            game->render((float) (accumulator / dt));
        }
    }
} // end of namespace Acidrain