#include "Timer.h"
#include "Stardust.h"
#include "FileSystem.h"
#include "EventSystem.h"
#include <iostream>
#include <GfxSystem.h>

using namespace Acidrain;

const int GAME_LOGICAL_RESOLUTION_X = 1024;
const int GAME_LOGICAL_RESOLUTION_Y = 768;

int main() {
    
    EVENTSYS.init();
    GFXSYS.init(GAME_LOGICAL_RESOLUTION_X, GAME_LOGICAL_RESOLUTION_Y);
    FILESYS.init("./data");

    Timer timer;
    Stardust game;

    while (!game.shouldQuit()) {
        EVENTSYS.update();
        game.process(timer.lap());
        GFXSYS.show();
    }

    return 0;
}