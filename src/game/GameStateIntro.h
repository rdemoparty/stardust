#pragma once
#include <GameState.h>
#include <memory>

namespace Acidrain {

    using namespace std;

    class Font;
    class Stardust;

    class GameStateIntro : public GameState<Stardust> {
    public:
        static GameStateIntro& instance();

        virtual void onEnter(Stardust* game) override;

        virtual void onExit(Stardust* game) override;

        virtual void update(Stardust* game, float elapsedSeconds) override;

        virtual void render(Stardust* game, float alpha) override;

    private:
        shared_ptr<Font> titleFont;
        float introTime = 0;
        float alpha = 0;
        float oldAlpha = 0;
    };
}