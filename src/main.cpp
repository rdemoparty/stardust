#include "Window.h"
#include "Timer.h"
#include "Stardust.h"
#include <iostream>
#include <GfxSystem.h>

using namespace Acidrain;

int main() {

    Window window(1920, 1080, WindowType::Fullscreen);

    GFXSYS.init(window.width(), window.height(), 1024, 768);

    Timer timer;
    Stardust game;

    while (!window.shouldQuit() && !game.shouldQuit()) {
        game.process(timer.secondsSinceStart());
        window.present();
    }

    return 0;
}