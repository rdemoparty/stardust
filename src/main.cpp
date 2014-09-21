#include "Window.h"
#include "Timer.h"
#include "Stardust.h"

using namespace Acidrain;

int main() {
    Window window(1024, 768, WindowType::Windowed);
    Timer timer;
    Stardust game;

    while (!window.shouldQuit() && !game.shouldQuit()) {
        game.process(timer.secondsSinceStart());
        window.present();
    }

    return 0;
}