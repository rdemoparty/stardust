#pragma once

#include <GameState.h>

namespace Acidrain {

    class Stardust;

    class GameStateTest : public GameState<Stardust> {
    public:
        static GameStateTest& instance();

        virtual void onEnter(Stardust* game) override;
        virtual void onExit(Stardust* game) override;
        virtual void update(Stardust* game, float elapsedSeconds) override;
        virtual void render(Stardust* game, float alpha) override;

    private:
        GameStateTest();
    };

} // namespace Acidrain