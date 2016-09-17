#pragma once

#include <GameState.h>

namespace Acidrain {

    class Stardust;

    class GameStateNextLevel : public GameState<Stardust> {
    public:
        static GameStateNextLevel& instance();

        virtual const char* name() const override { return "Next Level"; }
        virtual void update(Stardust* game, float elapsedSeconds) override;

    private:
        GameStateNextLevel();
    };
} // namespace Acidrain