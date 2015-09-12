#pragma once

#include <memory>

namespace Acidrain {
    using namespace std;

    class GameObject;
    class ScriptedBrain;

    class GameObjectFactory {
    public:

        explicit GameObjectFactory();
        virtual ~GameObjectFactory();

        void initialize(string repository);
        GameObject* createByName(const string& name);
        shared_ptr<ScriptedBrain> getBrain(const string& brainName);

    private:
        class impl;
        unique_ptr<impl> pimpl;
    };

} // namespace Acidrain