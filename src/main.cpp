#include <Timer.h>
#include <Stardust.h>
#include <FileSystem.h>
#include <GfxSystem.h>
#include <easylogging++.h>

_INITIALIZE_EASYLOGGINGPP

using namespace Acidrain;

int main(int argc, char** argv) {

    el::Loggers::addFlag(el::LoggingFlag::HierarchicalLogging);
    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
    el::Loggers::setLoggingLevel(el::Level::Debug);

    const int GAME_LOGICAL_RESOLUTION_X = 1024;
    const int GAME_LOGICAL_RESOLUTION_Y = 768;

    EVENTSYS.init();
    GFXSYS.init(GAME_LOGICAL_RESOLUTION_X, GAME_LOGICAL_RESOLUTION_Y);
    FILESYS.init("../data");

    Timer timer;
    Stardust game;

    double dt = 1.0 / 30.0;
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

    return 0;
}
