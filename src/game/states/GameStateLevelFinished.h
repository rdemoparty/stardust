#pragma once

#include <GameState.h>

namespace Acidrain {

    class Stardust;

    class GameStateLevelFinished : public GameState<Stardust> {
    public:
        static GameStateLevelFinished& instance();

        virtual const char* name() const override { return "Level Finished"; }

        virtual void onEnter(Stardust* game) override;
        virtual void update(Stardust* game, float elapsedSeconds) override;

    private:
        GameStateLevelFinished();
    };
} // namespace Acidrain