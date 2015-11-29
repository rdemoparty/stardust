#pragma once

#define SCRIPTSYS Acidrain::ScriptingSystem::getInstance()

namespace Acidrain {

    class ScriptingSystem {
    public:
        static ScriptingSystem& getInstance();
        ~ScriptingSystem();

        void init();

    private:
        ScriptingSystem();
    };
}