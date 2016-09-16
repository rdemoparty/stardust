#pragma once

#include <GameState.h>

namespace Acidrain {

    class Stardust;

    class GameStateCutSceneBeforeLevel : public GameState<Stardust> {
    public:
        static GameStateCutSceneBeforeLevel& instance();

        virtual void onEnter(Stardust* game) override;
        virtual void onExit(Stardust* game) override;
        virtual void update(Stardust* game, float elapsedSeconds) override;
        virtual void render(Stardust* game, float alpha) override;

    private:
        GameStateCutSceneBeforeLevel();
    };

} // namespace Acidrain