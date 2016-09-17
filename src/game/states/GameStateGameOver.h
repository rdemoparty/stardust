#pragma once

#include <GameState.h>

namespace Acidrain {

    class Stardust;

    class GameStateGameOver : public GameState<Stardust> {
    public:
        static GameStateGameOver& instance();

        virtual const char* name() const override {return "GameOver";}

        virtual void onEnter(Stardust* game) override;
        virtual void onExit(Stardust* game) override;
        virtual void update(Stardust* game, float elapsedSeconds) override;
        virtual void render(Stardust* game, float alpha) override;

    private:
        GameStateGameOver();
    };
} // namespace Acidrain