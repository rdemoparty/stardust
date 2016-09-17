#pragma once

#include <GameState.h>

namespace Acidrain {

    class Stardust;

    class GameStateShowLevelStats : public GameState<Stardust> {
    public:
        static GameStateShowLevelStats& instance();

        virtual const char* name() const override { return "Show Level Stats"; }

        virtual void onEnter(Stardust* game) override;
        virtual void onExit(Stardust* game) override;
        virtual void update(Stardust* game, float elapsedSeconds) override;
        virtual void render(Stardust* game, float alpha) override;

    private:
        GameStateShowLevelStats();
    };
} // namespace Acidrain