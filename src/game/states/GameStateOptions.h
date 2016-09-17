#pragma once

#include <GameState.h>
#include <memory>

namespace Acidrain {

    using namespace std;

    class Stardust;
    class Menu;

    class GameStateOptions : public GameState<Stardust> {
    public:
        static GameStateOptions& instance();

        virtual const char* name() const override { return "Options"; }

        virtual void onEnter(Stardust* game) override;
        virtual void onExit(Stardust* game) override;
        virtual void update(Stardust* game, float elapsedSeconds) override;
        virtual void render(Stardust* game, float alpha) override;

    private:
        GameStateOptions();
        shared_ptr<Menu> menu;
        void setupMenu() const;
    };

} // namespace Acidrain