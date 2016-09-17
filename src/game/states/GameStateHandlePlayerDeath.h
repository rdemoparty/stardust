#pragma once

#include <GameState.h>

namespace Acidrain {

    class Stardust;

    class GameStateHandlePlayerDeath : public GameState<Stardust> {
    public:
        static GameStateHandlePlayerDeath& instance();

        virtual const char* name() const override {return "Handle Player Death";}

        virtual void onEnter(Stardust* game) override;
        virtual void update(Stardust* game, float elapsedSeconds) override;

    private:
        GameStateHandlePlayerDeath();
    };
} // namespace Acidrain