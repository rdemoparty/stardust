#pragma once

namespace Acidrain {

    class FpsCounter {
    public:

        void update(float elapsedSeconds);

        void addFrame();

        int getFps() const;

    private:

        float timeAccumulator = 0.0f;

        int currentFps = 0;

        int fps = 0;
    };

}