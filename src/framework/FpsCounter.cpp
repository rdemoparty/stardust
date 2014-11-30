#include <FpsCounter.h>

namespace Acidrain {

    void FpsCounter::update(float elapsedSeconds) {
        timeAccumulator += elapsedSeconds;
        if (timeAccumulator > 1.0f) {
            fps = currentFps;
            timeAccumulator = 0;
            currentFps = 0;
        }
    }

    void FpsCounter::addFrame() {
        currentFps++;
    }

    int FpsCounter::getFps() const {
        return fps;
    }
}