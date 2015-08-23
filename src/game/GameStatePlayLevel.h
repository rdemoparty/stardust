#pragma once

#include <GameState.h>
#include <type_traits>

namespace Acidrain {

    using namespace std;

    class Stardust;

    class GameStatePlayLevel : public GameState<Stardust> {

    public:

        static GameStatePlayLevel& instance();

        virtual void onEnter(Stardust* game) override;

        virtual void onExit(Stardust* game) override;

        virtual void update(Stardust* game, float elapsedSeconds) override;

        virtual void render(Stardust* game, float alpha) override;

    private:
        void handleGameEvents(const Stardust *game) const;
    };

} // namespace Acidrain