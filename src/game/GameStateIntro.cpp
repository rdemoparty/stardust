#include <GameStateIntro.h>
#include <GfxSystem.h>
#include <Font.h>
#include <Stardust.h>
#include <MathSupport.h>
#include <GameStateMenu.h>
#include <InputProvider.h>

namespace Acidrain {

    GameStateIntro& GameStateIntro::instance() {
        static GameStateIntro instance;
        return instance;
    }

    static shared_ptr<SpriteSheet> splashSheet;

    void GameStateIntro::onEnter(Stardust* game) {
        if (!splashSheet) {
            splashSheet = shared_ptr<SpriteSheet>(new SpriteSheet());
            splashSheet->texture = GFXSYS.loadTexture("sprites/splash.png");
            splashSheet->autoAdd(1024, 768);
        }

        if (!titleFont) {
            titleFont = make_shared<Font>("fonts/Neo Sans Pro Bold.ttf", 100.0f);
        }


//        GFXSYS.setClearColor(vec3(0, 0, 0));
        GFXSYS.setClearColor(vec3(0.1 * 2, 0.1 * 2, 0.12 * 2));
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
        } else if (introTime < 10.0f) {
            float it = introTime - 5.0f;
            if (it < 2.0f) {
                alpha = easeInQuintic(it / 2.0f);
            } else if (it < 3.0f) {
                alpha = 1.0f;
            } else {
                alpha = 1.0f - easeInOutQuintic((it - 3.0f) / 2.0f);
            }
        } else {
            game->fsm->changeState(&GameStateMenu::instance());
        }

    }

    void GameStateIntro::render(Stardust* game, float renderAlpha) {
        GFXSYS.clearScreen();

        float a = alpha * renderAlpha + oldAlpha * (1.0f - renderAlpha);
        if (introTime < 5.0f) {
            GFXSYS.setTransparencyMode(TransparencyMode::Additive);
            glColor4f(1, 1, 1, a);

            Sprite sprite(splashSheet.get(), 0);
            GFXSYS.drawSprite(sprite, vec2(0, 0));

        } else {
            GFXSYS.setTransparencyMode(TransparencyMode::Transparent);

            string gameTitle = "B A C K F I R E";
            FontPrintStyle printStyle = FontPrintStyle::SHADOW;
            vec2 textExtents = titleFont->getTextExtent(gameTitle, printStyle);

            titleFont->print((1024 - textExtents.x) / 2,
                             (768 - textExtents.y) / 2,
                             gameTitle,
                             vec4(1, 1, 1, a),
                             vec4(0, 0, 0, 0.4),
                             printStyle);
        }

        GFXSYS.show();
    }
}
