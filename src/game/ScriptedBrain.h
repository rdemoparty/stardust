#pragma once

#include <string>
#include <lua.hpp>

namespace Acidrain {

    using namespace std;

    class GameObject;

    class ScriptedBrain {
    public:

        explicit ScriptedBrain(std::string brainFilename);

        virtual ~ScriptedBrain();

        void onSpawn(GameObject* gameObject);

        void onUpdate(GameObject* gameObject, float elapsedSeconds);

        void onDeath(GameObject* gameObject);

    private:
        lua_State* L;

        void initializeLuaContext(string& brainFilename);

        void registerExports();

        void callGenericEntityFunction(GameObject* gameObject, string functionName);
    };

}