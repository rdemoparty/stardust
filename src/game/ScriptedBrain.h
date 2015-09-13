#pragma once

#include <string>
#include <memory>
#include <lua.hpp>

namespace Acidrain {

    using namespace std;

    class GameObject;
    class Scene;

    class ScriptedBrain {
    public:
        explicit ScriptedBrain(const string& brainFilename);

        virtual ~ScriptedBrain();

        /**
        * Callback invoked when the game object is added to the scene. Do care to note that
        * at this time the object is not in scene's active object list but is added to the
        * list of newly created object. They will become "active" only next update frame.
        *
        *
        * This is called after the object has been initialized with the settings from the recipe
        * so it's a good extension point to add supplementary attributes.
        */
        void onSpawn(GameObject* gameObject);

        void onUpdate(GameObject* gameObject, float elapsedSeconds);

        void onDamage(GameObject* gameObject, float damage, GameObject* damageInflicter);

        void onDeath(GameObject* gameObject);

    private:
        lua_State* L;

        string brainName;

        void initializeLuaContext(const string& brainFilename);

        void registerExports();
    };

}