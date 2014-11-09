#include "Timer.h"
#include "Stardust.h"
#include "FileSystem.h"
#include "EventSystem.h"
#include <iostream>
#include <GfxSystem.h>

using namespace Acidrain;

const int GAME_LOGICAL_RESOLUTION_X = 1024;
const int GAME_LOGICAL_RESOLUTION_Y = 768;

int main(int argc, char** argv) {

    EVENTSYS.init();
    GFXSYS.init(GAME_LOGICAL_RESOLUTION_X, GAME_LOGICAL_RESOLUTION_Y);
    FILESYS.init("./data");

    Timer timer;
    Stardust game;

    float accumulator = 0;
    int fps = 0;

    while (!game.shouldQuit()) {
        EVENTSYS.update();
        float elapsedSeconds = timer.lap();
        game.process(elapsedSeconds);
        fps++;
        accumulator += elapsedSeconds;
        if (accumulator >= 1.0f) {
            std::cout << "fps: " << fps << std::endl;
            accumulator = 0;
            fps = 0;
        }
        GFXSYS.show();
    }

    return 0;
}
