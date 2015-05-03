#pragma once

#include <string>
#include <vector>
#include <queue>
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

        bool isFinished();

        virtual void reset();

        virtual void update(float elapsedSeconds);

    protected:
        Scene* scene;

    private:
        float offset;
        float pixelsToScrollPerSecond = 100;

        vector<LevelScriptEntry> events;
        queue<LevelScriptEntry> remainingEvents;
        void addEvent(LevelScriptEntry entry);
    };
} // namespace Acidrain