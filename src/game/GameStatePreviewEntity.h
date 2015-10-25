#pragma once

#include <GameState.h>
#include <string>

namespace Acidrain {

    using namespace std;

    class Stardust;

    class GameStatePreviewEntity : public GameState<Stardust> {
    public:
        static GameStatePreviewEntity& instance();

        void previewEntity(string entityName);

        virtual void onEnter(Stardust* game) override;

        virtual void onExit(Stardust* game) override;

        virtual void update(Stardust* game, float elapsedSeconds) override;

        virtual void render(Stardust* game, float alpha) override;

    private:
        string entityName;
    };

} // namespace Acidrain