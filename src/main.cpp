#include <Timer.h>
#include <Stardust.h>
#include <FileSystem.h>
#include <GfxSystem.h>
#include <easylogging++.h>
#include <game/Level.h>
#include <CommandLineParser.h>

_INITIALIZE_EASYLOGGINGPP

using namespace Acidrain;

el::Level fromString(string levelName);

int main(int argc, char** argv) {
    try {
        FLAG_ADD_INTEGER("width,w", "The physical window width", -1);
        FLAG_ADD_INTEGER("height,h", "The physical window height", -1);
        FLAG_ADD_BOOLEAN("fullscreen,f", "Whether the window is fullscreen or not", false);
        FLAG_ADD_BOOLEAN("vsync,v", "Whether vsync is enabled or not", false);
        FLAG_ADD_STRING("log-level,l", "The log level", "debug");

        FLAGS_PARSE(argc, argv);

        el::Loggers::addFlag(el::LoggingFlag::HierarchicalLogging);
        el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
        el::Loggers::setLoggingLevel(fromString(FLAG_AS_STRING("log-level")));

        const int GAME_LOGICAL_RESOLUTION_X = 1024;
        const int GAME_LOGICAL_RESOLUTION_Y = 768;

        EVENTSYS.init();
        GFXSYS.init(GAME_LOGICAL_RESOLUTION_X, GAME_LOGICAL_RESOLUTION_Y);
        FILESYS.init("../data");

        Timer timer;
        Stardust game;

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
                game.update(dt);
                accumulator -= dt;
            }

            game.render(accumulator / dt);
        }
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
