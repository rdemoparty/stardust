#include <Timer.h>
#include <Stardust.h>
#include <FileSystem.h>
#include <GfxSystem.h>
#include <AudioSystem.h>
#include <easylogging++.h>
#include <game/Level.h>
#include <CommandLineParser.h>
#include <GameStateEditor.h>
#include <GameStateIntro.h>
#include <Version.h>
#include <UserPreferences.h>

_INITIALIZE_EASYLOGGINGPP

namespace Acidrain {
    DEFINE_string(log_level, l, "The log level", "debug")
    DEFINE_bool(editor, e, "Whether to enter editor mode or not", false)
}

using namespace Acidrain;

void configureLogging();

int main(int argc, char** argv) {
    try {
        ParseCommandLineFlags(argc, argv);
        configureLogging();

        const int GAME_LOGICAL_RESOLUTION_X = 1024;
        const int GAME_LOGICAL_RESOLUTION_Y = 768;

        LOG(INFO) << "STARDUST " << STARDUST_VERSION << " (" << STARDUST_TIMESTAMP << ")";

        EVENTSYS.init();
        USERPREFS.init();
        FILESYS.init(FLAG_data_dir);
        AUDIOSYS.init();
        GFXSYS.init(GAME_LOGICAL_RESOLUTION_X, GAME_LOGICAL_RESOLUTION_Y);

        Timer timer;
        Stardust game;

        if (FLAG_editor) {
            game.fsm->setGlobalState(&GameStateEditor::instance());
        } else {
            game.fsm->changeState(&GameStateIntro::instance());
        }

        double dt = 1.0 / 20.0;
        double accumulator = 0;

        LOG(INFO) << "Starting up application at logical resolution " <<
                GAME_LOGICAL_RESOLUTION_X << "x" << GAME_LOGICAL_RESOLUTION_Y
                << " with physics time step of " << dt << " seconds";

        while (!game.shouldQuit()) {
            float frameTime = timer.lap();
            if (frameTime > 0.25)
                frameTime = 0.25;

            accumulator += frameTime;
            while (accumulator >= dt) {
                game.update((float) dt);
                accumulator -= dt;
            }

            game.render((float) (accumulator / dt));
        }
        LOG(INFO) << "Application finished. Exiting main()";
    } catch (std::exception* e) {
        LOG(FATAL) << e->what();
    }
    return 0;
}

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

void configureLogging() {
    el::Loggers::addFlag(el::LoggingFlag::HierarchicalLogging);
    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
    el::Loggers::setLoggingLevel(fromString(FLAG_log_level));
}
