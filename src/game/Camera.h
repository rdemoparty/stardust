#pragma once

#include <glm/mat4x4.hpp>

namespace Acidrain {

    using namespace glm;

    class Camera {
    public:

        void shake(float strength);

        void update(float elapsedSeconds);

        const mat4 getShakeMatrix() const;

    private:
        float shakeFactor;
        mat4 shakeMatrix;
    };

}