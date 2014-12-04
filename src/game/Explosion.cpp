#include <Explosion.h>
#include <Animation.h>

namespace Acidrain {

    Explosion::Explosion() {
    }

    void Explosion::update(float elapsedSeconds) {
        GameObject::update(elapsedSeconds);
        if (animation->isFinished()) {
            state.isDead = true;
            state.deathReason = EntityDeathReason::JobFinished;
        }
    }
}