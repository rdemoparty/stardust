#include <Camera.h>
#include <Randomizer.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Acidrain {

    static Randomizer randomizer;

    void Camera::update(float elapsedSeconds) {
        // dampen camera shake factor
        if (shakeFactor > 0) {
            shakeFactor -= 10.0 * elapsedSeconds;
            if (shakeFactor < 0) {
                shakeFactor = 0;
            }
        }

        shakeMatrix = mat4(1);
        if (shakeFactor > 0) {
            vec3 shakeVector = vec3(
                    shakeFactor * randomizer.randomSignedUnit(),
                    shakeFactor * randomizer.randomSignedUnit(),
                    0
            );
            shakeMatrix = translate(shakeMatrix, shakeVector);
        }
    }

    void Camera::shake(float strength) {
        shakeFactor += strength;
        if (shakeFactor > maxShakeFactor)
            shakeFactor = maxShakeFactor;
    }

    const mat4 Camera::getShakeMatrix() const {
        return shakeMatrix;
    }
}