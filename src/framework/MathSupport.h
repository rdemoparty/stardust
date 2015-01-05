#pragma once

#include <math.h>

/**
* Implementation of Robert Penner's easing functions
*/
namespace Acidrain {

    inline float easeInCubic(float t) {
        return pow(t, 3.0f);
    }

    inline float easeOutCubic(float t) {
        return 1.0f - easeInCubic(1.0f - t);
    }

    inline float easeInOutCubic(float t) {
        if (t < 0.5f)
            return easeInCubic(t * 2.0f) / 2.0f;
        else
            return 1.0f - easeInCubic((1.0f - t) * 2) / 2.0f;
    }

    inline float easeInQuintic(float t) {
        return pow(t, 5.0f);
    }

    inline float easeOutQuintic(float t) {
        return 1.0f - easeInQuintic(1.0f - t);
    }

    inline float easeInOutQuintic(float t) {
        if (t < 0.5f)
            return easeInQuintic(t * 2.0f) / 2.0f;
        else
            return 1.0f - easeInQuintic((1.0f - t) * 2) / 2.0f;
    }

    inline float easeInQuad(float t) {
        return t * t;
    }

    inline float easeOutQuad(float t) {
        return 1.0f - easeInQuad(1.0f - t);
    }

} // namespace Acidrain