#include <GameStateIntro.h>
#include <GfxSystem.h>
#include <Font.h>
#include <Stardust.h>
#include <Math.h>
#include <GameStateMenu.h>
#include <InputProvider.h>

namespace Acidrain {

    GameStateIntro& GameStateIntro::instance() {
        static GameStateIntro instance;
        return instance;
    }

    void GameStateIntro::onEnter(Stardust* game) {
        if (!titleFont)
            titleFont = make_shared<Font>("fonts/Neo Sans Pro Bold.ttf", 90.0f);

        GFXSYS.setClearColor(vec3(0, 0, 0));
        introTime = 0;
        oldAlpha = alpha = 0;
    }

    void GameStateIntro::onExit(Stardust* game) {
    }

    void GameStateIntro::update(Stardust* game, float elapsedSeconds) {
        if (INPUT.isKeyJustPressed(SDL_SCANCODE_ESCAPE)) {
            game->fsm->changeState(&GameStateMenu::instance());
            return;
        }

        oldAlpha = alpha;

        introTime += elapsedSeconds;
        if (introTime < 5.0f) {
            if (introTime < 2.0f) {
                alpha = easeInQuintic(introTime / 2.0f);
            } else if (introTime < 3.0f) {
                alpha = 1.0f;
            } else {
                alpha = 1.0f - easeInOutQuintic((introTime - 3.0f) / 2.0f);
            }
        } else {
            game->fsm->changeState(&GameStateMenu::instance());
        }
    }

    void GameStateIntro::render(Stardust* game, float renderAlpha) {
        GFXSYS.clearScreen();
        GFXSYS.setTransparencyMode(TransparencyMode::Additive);

        float a = alpha * renderAlpha + oldAlpha * (1.0f - renderAlpha);
        titleFont->print(200, 300, "B A C K F I R E", vec4(1, 1, 1, a));

        GFXSYS.show();
    }
}