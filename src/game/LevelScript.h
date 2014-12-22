#pragma once

namespace Acidrain {

    class Scene;

    class LevelScript {
    public:

        LevelScript(Scene* scene);

        virtual ~LevelScript();

        void reset();

        void update(float elapsedSeconds);

    private:
        Scene* scene;
        float timeUntilNextSpawn;
    };
}