#include "Window.h"
#include "Timer.h"
#include "Stardust.h"
#include "FileSystem.h"
#include <iostream>
#include <GfxSystem.h>

using namespace Acidrain;

int main() {

    Window window(1920, 1080, WindowType::Fullscreen);

    GFXSYS.init(window.width(), window.height(), 1024, 768);
//    FILESYS.init(FILESYS.getExeDir(FILESYS.getExePath()) + "/data");
    FILESYS.init("./data");

    Timer timer;
    Stardust game;

    while (!window.shouldQuit() && !game.shouldQuit()) {
        game.process(timer.lap());
        window.present();
    }

    return 0;
}