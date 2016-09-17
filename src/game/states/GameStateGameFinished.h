#pragma once

#include <GameState.h>

namespace Acidrain {

    class Stardust;

    class GameStateGameFinished : public GameState<Stardust> {
    public:
        static GameStateGameFinished& instance();

        virtual const char* name() const override {return "GameFinished";}

        virtual void onEnter(Stardust* game) override;
        virtual void onExit(Stardust* game) override;
        virtual void update(Stardust* game, float elapsedSeconds) override;
        virtual void render(Stardust* game, float alpha) override;

    private:
        GameStateGameFinished();
    };
} // namespace Acidrain