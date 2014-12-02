#include <PlayerShip.h>
#include <InputProvider.h>
#include <Scene.h>
#include <MovementStrategy.h>

namespace Acidrain {

    PlayerShip::PlayerShip() {
        input = make_shared<InputProvider>();
    }

    void PlayerShip::update(float elapsedSeconds) {
        if (input->isKeyJustPressed(SDL_SCANCODE_SPACE))
            fireWeapons(true);
        else if (input->isKeyJustReleased(SDL_SCANCODE_SPACE))
            fireWeapons(false);

        if (movementController)
            movementController->update(elapsedSeconds);

        confinePlayerToVisibleArea();

        // update xform
        Entity::update(elapsedSeconds);
        updateWeapons(elapsedSeconds);

        input->copyNewStateToOldState();
    }

    void PlayerShip::confinePlayerToVisibleArea() {
        scene->confineEntityInVisibleArea(this);
    }
}