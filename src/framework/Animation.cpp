#include <Animation.h>

namespace Acidrain {

    Animation::Animation(const AnimationData *animData)
            : animationData(animData) {
    }

    void Animation::start() {
        paused = false;
    }

    void Animation::stop() {
        paused = true;
    }

    void Animation::reset() {
        unsigned long lastFrame = animationData->frames.size() - 1;
        unsigned long firstFrame = 0;
        currentFrameIndex = animationData->loopType == AnimationLoopType::REVERSE ? lastFrame : firstFrame;
        frameTimeAccumulator = 0;
        pingPongDirection = AnimationDirection::RIGHT;
        animationFinished = false;
    }

    bool Animation::isFinished() const {
        return animationFinished;
    }

    bool Animation::isPaused() const {
        return paused;
    }

    Sprite Animation::getSprite() const {
        return animationData->frames.at(currentFrameIndex);
    }

    void Animation::update(float dt) {
        if (paused || animationFinished || animationData->frames.empty())
            return;

        frameTimeAccumulator += dt * 1000.0f;

        while (frameTimeAccumulator > animationData->frameTimeInMilliseconds) {
            frameTimeAccumulator -= animationData->frameTimeInMilliseconds;
            currentFrameIndex += nextFrameDirection() == AnimationDirection::RIGHT ? 1 : -1;

            if (isFrameIndexOutOfRange()) {
                if (currentFrameIndex < 0) {
                    switch (animationData->loopType) {
                        case AnimationLoopType::PING_PONG:
                            currentFrameIndex = (int) (1 % animationData->frames.size());
                            pingPongDirection = AnimationDirection::RIGHT;
                            break;
                        case AnimationLoopType::REVERSE:
                            currentFrameIndex += animationData->frames.size();
                            break;
                        default:
                            // throw exception, invalid state
                            break;
                    }
                } else {
                    switch (animationData->loopType) {
                        case AnimationLoopType::NONE:
                            animationFinished = true;
                            currentFrameIndex = (int) (animationData->frames.size() - 1); // leave frame index valid at last frame
                            frameTimeAccumulator = 0;
                            break;
                        case AnimationLoopType::PING_PONG:
                            currentFrameIndex = (int) (animationData->frames.size() > 1
                                                                ? animationData->frames.size() - 2
                                                                : 0);
                            pingPongDirection = AnimationDirection::LEFT;
                            break;
                        case AnimationLoopType::FORWARD:
                            currentFrameIndex = (int) (currentFrameIndex % animationData->frames.size());
                            break;
                        default:
                            // throw exception, invalid state
                            break;
                    }
                }
            }
        }
    }

    bool Animation::isFrameIndexOutOfRange() const {
        return (currentFrameIndex < 0 || currentFrameIndex >= static_cast<int>(animationData->frames.size()));
    }

    AnimationDirection Animation::nextFrameDirection() const {
        if (animationData->loopType == AnimationLoopType::REVERSE || (animationData->loopType == AnimationLoopType::PING_PONG && pingPongDirection == AnimationDirection::LEFT)) {
            return AnimationDirection::LEFT;
        } else {
            return AnimationDirection::RIGHT;
        }
    }

}