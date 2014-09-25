#include "Window.h"
#include "Timer.h"
#include "Stardust.h"
#include "FileSystem.h"
#include "EventSystem.h"
#include <iostream>
#include <GfxSystem.h>

using namespace Acidrain;

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);

    Window window(1920, 1080, WindowType::Fullscreen);

    GFXSYS.init(window.width(), window.height(), 1024, 768);
    FILESYS.init("./data");

    Timer timer;
    Stardust game;

    while (!game.shouldQuit()) {
        EVENTSYS.update();
        game.process(timer.lap());
        window.present();
    }

    return 0;
}