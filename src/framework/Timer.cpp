#include <Timer.h>

namespace Acidrain {

    using namespace std::chrono;

    Timer::Timer() {
        startTime = high_resolution_clock::now();
        lastLapTime = startTime;
    }

    double Timer::secondsSinceStart() {
        high_resolution_clock::time_point currentTime = high_resolution_clock::now();
        duration<double> elapsedSeconds = currentTime - startTime;
        return elapsedSeconds.count();
    }

    float Timer::lap() {
        high_resolution_clock::time_point currentTime = high_resolution_clock::now();
        duration<float> elapsedSeconds = currentTime - lastLapTime;
        lastLapTime = currentTime;
        return elapsedSeconds.count();
    }

} // namespace Acidrain

