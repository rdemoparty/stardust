#include <Timer.h>

namespace Acidrain {

    using namespace std;

    Timer::Timer() {
        startTime = chrono::system_clock::now();
        lastLapTime = startTime;
    }

    double Timer::secondsSinceStart() {
        chrono::time_point<chrono::system_clock> currentTime = chrono::system_clock::now();
        chrono::duration<double> elapsedSeconds = currentTime - startTime;
        return elapsedSeconds.count();
    }

    float Timer::lap() {
        chrono::time_point<chrono::system_clock> currentTime = chrono::system_clock::now();
        chrono::duration<float> elapsedSeconds = currentTime - lastLapTime;
        lastLapTime = currentTime;
        return elapsedSeconds.count();
    }

} // namespace Acidrain

