#pragma once

#include <LevelScript.h>

namespace Acidrain {

    class LevelScriptPreviewEntity : public LevelScript {
    public:

        LevelScriptPreviewEntity(string entityName, Scene* scene);

        virtual ~LevelScriptPreviewEntity();

        void update(float elapsedSeconds) override;

    private:
        string entityName;
    };
} // namespace Acidrain