#pragma once

#include <GameState.h>

namespace Acidrain {
    class Stardust;

    class GameStateNewGame : public GameState<Stardust> {
    public:
        static GameStateNewGame& instance();

        virtual const char* name() const override { return "New Game"; }
        virtual void update(Stardust* game, float elapsedSeconds) override;

    private:
        GameStateNewGame();
    };
} // namespace Acidrain