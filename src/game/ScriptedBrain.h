#pragma once

#include <string>
#include <lua.hpp>
#include <memory>

namespace Acidrain {

    using namespace std;

    class GameObject;

    class Scene;

    class ScriptedBrain {
    public:

        explicit ScriptedBrain(std::string brainFilename);

        virtual ~ScriptedBrain();

        void injectScene(Scene* scene);

        void onSpawn(GameObject* gameObject);

        void onUpdate(GameObject* gameObject, float elapsedSeconds);

        void onDamage(GameObject* gameObject, float damage, GameObject* damageInflicter);

        void onDeath(GameObject* gameObject);

    private:
        lua_State* L;

        std::string brainName;

        void initializeLuaContext(string& brainFilename);

        void registerExports();
    };

}