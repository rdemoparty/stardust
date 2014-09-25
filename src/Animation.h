#pragma once

#include <Sprite.h>

namespace Acidrain {

    enum class AnimationLoopType {
        NONE,
        FORWARD,
        REVERSE,
        PING_PONG
    };

    enum class AnimationDirection {
        LEFT,
        RIGHT
    };

    struct AnimationData {
        std::vector<Sprite> frames;
        float frameTimeInMilliseconds;
        AnimationLoopType loopType;
    };

    class Animation {
    public:

        Animation(const AnimationData* animData);

        void start();

        void stop();

        void reset();

        bool isFinished() const;

        bool isPaused() const;

        const Sprite &getSprite() const;

        void update(float dt);

    protected:
        bool isFrameIndexOutOfRange() const;

        AnimationDirection nextFrameDirection() const;

    private:
        const AnimationData* animationData;

        float frameTimeAccumulator = 0;
        AnimationDirection pingPongDirection = AnimationDirection::RIGHT;
        bool paused = true;
        bool animationFinished = false;
        int currentFrameIndex = 0;

    };

} // namespace Acidrain