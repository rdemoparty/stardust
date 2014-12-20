#pragma once

#include <chrono>

namespace Acidrain {

    class Timer {
    public:

        Timer();

        double secondsSinceStart();

        float lap();

    private:
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point lastLapTime;
    };

} // namespace Acidrain