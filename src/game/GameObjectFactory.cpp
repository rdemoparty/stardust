#include <GameObjectFactory.h>
#include <InputProvider.h>
#include <GameObject.h>
#include <MovementStrategyLinear.h>
#include <MovementStrategySinusoidal.h>
#include <MovementStrategyPlayer.h>
#include <Animation.h>
#include <SpriteAnimationRepository.h>

namespace Acidrain {
    GameObjectFactory::GameObjectFactory() {
        input = shared_ptr<InputProvider>(new InputProvider());
    }

    GameObjectFactory::~GameObjectFactory() {
    }

    GameObject* GameObjectFactory::createPlayer(vec2 position) {
        shared_ptr<MovementStrategyPlayer> playerMovement = make_shared<MovementStrategyPlayer>(input, 300.0f);

        GameObject* object = new GameObject();
        object->animation = ANIMREPO.newAnimation("enemy2");
        object->animation->start();
        object->size = vec2(64, 64);
        object->rotation = (float) M_PI;
        object->position = position;
        object->setMovementController(playerMovement);
        return object;
    }

    GameObject* GameObjectFactory::createEnemy(vec2 position) {
        shared_ptr<MovementStrategyLinear> mc = make_shared<MovementStrategyLinear>(vec2(0, 1), 100.0f);
        GameObject* object = new GameObject();
        object->animation = ANIMREPO.newAnimation("enemy2");
        object->animation->start();
        object->size = vec2(64, 64);
        object->position = position;
        object->setMovementController(mc);
        return object;
    }

    GameObject* GameObjectFactory::createExplosion(vec2 position) {
        shared_ptr<MovementStrategySinusoidal> mc = make_shared<MovementStrategySinusoidal>(2.0f);
        GameObject* object = new GameObject();
        object->animation = ANIMREPO.newAnimation("explosion");
        object->animation->start();
        object->size = object->animation->getSprite().getSize();
        object->position = position;
        object->setMovementController(mc);
        return object;
    }

    GameObject* GameObjectFactory::createLaser() {
        shared_ptr<MovementStrategyLinear> mc = make_shared<MovementStrategyLinear>(vec2(0, -1), 300.0f);

        GameObject* object = new GameObject();
        object->animation = ANIMREPO.newAnimation("laser");
        object->animation->start();
        object->size = object->animation->getSprite().getSize();
        object->setMovementController(mc);
        return object;
    }
}