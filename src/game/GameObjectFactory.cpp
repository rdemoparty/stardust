#include <GameObjectFactory.h>
#include <InputProvider.h>
#include <GameObject.h>
#include <MovementStrategyLinear.h>
#include <MovementStrategySinusoidal.h>
#include <MovementStrategyPlayer.h>

namespace Acidrain {
    GameObjectFactory::GameObjectFactory() {
        input = shared_ptr<InputProvider>(new InputProvider());
    }

    GameObjectFactory::~GameObjectFactory() {
    }

    GameObject *GameObjectFactory::player(vec2 position) {
        shared_ptr<MovementStrategyPlayer> playerMovement = shared_ptr<MovementStrategyPlayer>(new MovementStrategyPlayer(input, 300.0f));

        GameObject *player = new GameObject();
        player->rotation = M_PI;
        player->position = position;
        player->setMovementController(playerMovement);
        return player;
    }

    GameObject *GameObjectFactory::enemy(vec2 position) {
        shared_ptr<MovementStrategyLinear> mc = shared_ptr<MovementStrategyLinear>(new MovementStrategyLinear(vec2(0, 1), 100.0f));
        GameObject *enemy = new GameObject();
        enemy->position = position;
        enemy->setMovementController(mc);
        return enemy;
    }

    GameObject *GameObjectFactory::enemy2(vec2 position) {
        shared_ptr<MovementStrategySinusoidal> mc = shared_ptr<MovementStrategySinusoidal>(new MovementStrategySinusoidal(2.0f));
        GameObject *enemy = new GameObject();
        enemy->position = position;
        enemy->setMovementController(mc);
        return enemy;
    }
}