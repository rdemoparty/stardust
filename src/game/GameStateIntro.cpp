#include <GameStateIntro.h>
#include <GfxSystem.h>
#include <Font.h>
#include <Stardust.h>
#include <MathSupport.h>
#include <GameStateMenu.h>
#include <InputProvider.h>
#include <TextLayout.h>

namespace Acidrain {

    GameStateIntro& GameStateIntro::instance() {
        static GameStateIntro instance;
        return instance;
    }

    void GameStateIntro::onEnter(Stardust* game) {
        if (!titleFont)
            titleFont = make_shared<Font>("fonts/Neo Sans Pro Bold.ttf", 90.0f);

        if (!titleLayout) {
            Box constraint = Box(200, 300, 1024, 768);
            titleLayout = shared_ptr<TextLayout>(new TextLayout("B A C K F I R E", constraint, titleFont.get()));
        }


        GFXSYS.setClearColor(vec3(0, 0, 0));
//        GFXSYS.setClearColor(vec3(0.1 * 2, 0.1 * 2, 0.12 * 2));
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
        if (introTime < 2.0f) {
            alpha = easeInQuintic(introTime / 2.0f);
        } else if (introTime < 3.0f) {
            alpha = 1.0f;
        }

//        introTime += elapsedSeconds;
//        if (introTime < 5.0f) {
//            if (introTime < 2.0f) {
//                alpha = easeInQuintic(introTime / 2.0f);
//            } else if (introTime < 3.0f) {
//                alpha = 1.0f;
//            } else {
//                alpha = 1.0f - easeInOutQuintic((introTime - 3.0f) / 2.0f);
//            }
//        } else {
//            game->fsm->changeState(&GameStateMenu::instance());
//        }
    }

    void GameStateIntro::render(Stardust* game, float renderAlpha) {
        GFXSYS.clearScreen();
        GFXSYS.setTransparencyMode(TransparencyMode::Transparent);

        float a = alpha * renderAlpha + oldAlpha * (1.0f - renderAlpha);
        titleLayout->render(vec4(1, 1, 1, a), vec4(0, 0, 0, 0.0));

        GFXSYS.show();
    }
}
