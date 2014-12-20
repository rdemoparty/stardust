#include <Timer.h>
#include <Stardust.h>
#include <FileSystem.h>
#include <GfxSystem.h>

using namespace Acidrain;

int main(int argc, char** argv) {

    const int GAME_LOGICAL_RESOLUTION_X = 1024;
    const int GAME_LOGICAL_RESOLUTION_Y = 768;

    EVENTSYS.init();
    GFXSYS.init(GAME_LOGICAL_RESOLUTION_X, GAME_LOGICAL_RESOLUTION_Y);
    FILESYS.init("../data");

    Timer timer;
    Stardust game;

    double dt = 1.0 / 30.0;
    double accumulator = 0;

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
