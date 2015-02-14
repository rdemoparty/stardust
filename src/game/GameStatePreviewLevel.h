#pragma once

#include <GameState.h>
#include <string>

namespace Acidrain {

    using namespace std;

    class Stardust;

    class GameStatePreviewLevel : public GameState<Stardust> {

    public:

        static GameStatePreviewLevel& instance();

        void preview(Stardust* game);

        virtual void onEnter(Stardust* game) override;

        virtual void onExit(Stardust* game) override;

        virtual void update(Stardust* game, float elapsedSeconds) override;

        virtual void render(Stardust* game, float alpha) override;

    private:
    };

} // namespace Acidrain