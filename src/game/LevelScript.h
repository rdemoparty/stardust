#pragma once

#include <string>
#include <vector>
#include <glm/vec2.hpp>

namespace Acidrain {

    using namespace std;
    using namespace glm;

    class Scene;

    struct LevelScriptEntry {
        string recipeName;
        vec2 position;
        int layer;
    };

    class LevelScript {
    public:

        LevelScript(Scene* scene);

        virtual ~LevelScript();

        void load(string scriptURI);

        void reset();

        void update(float elapsedSeconds);

    private:

        Scene* scene;
        float timeUntilNextSpawn;
        vector<LevelScriptEntry> events;

        void addEvent(LevelScriptEntry entry);
    };
} // namespace Acidrain