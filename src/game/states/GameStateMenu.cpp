#include <GameStateMenu.h>
#include <Stardust.h>
#include <Font.h>
#include <GfxSystem.h>
#include <InputProvider.h>
#include <MathSupport.h>
#include <AudioSystem.h>
#include <GameStateNewGame.h>
#include <GameStateOptions.h>
#include <GameStateCredits.h>
#include <Version.h>
#include <GameSession.h>
#include <TextLayout.h>


namespace Acidrain {
    GameStateMenu& GameStateMenu::instance() {
        static GameStateMenu instance;
        return instance;
    }

    static const int MENU_OPTION_COUNT = 7;
    static const char* menuOptions[MENU_OPTION_COUNT] = {
            "LAUNCH",
            "RESUME",
            "ARCHIVES",
            "ACHIEVEMENTS",
            "OPTIONS",
            "CREDITS",
            "EXIT"
    };

    static float menuOptionPosX[MENU_OPTION_COUNT] = {
            0, 0, 0, 0, 0, 0, 0
    };

    static float menuOptionPosXOld[MENU_OPTION_COUNT] = {
            0, 0, 0, 0, 0, 0, 0
    };

    static shared_ptr<TextLayout> textLayouts[MENU_OPTION_COUNT];

    static int selectedIndex = 0;

    static float menuTime = 0;

    static vec4 OPTION_COLOR = vec4(1, 1, 1, 1.0f);

    static vec4 SELECTED_OPTION_COLOR = vec4(1, 0, 0, 1.0f);

    const char* SOUND_OPTION_REJECTED = "sounds/menu/option-rejected.ogg";
    const char* SOUND_OPTION_ACCEPTED = "sounds/menu/option-accepted.ogg";
    const char* SOUND_OPTION_CHANGE = "sounds/menu/option-changed.ogg";


    void GameStateMenu::onEnter(Stardust* game) {
        FontRenderStyle renderStyle;
        renderStyle.outlineSize = 3;

        if (!menuFont) {
            menuFont = make_shared<Font>("fonts/Neo Sans Pro Bold.ttf", 40.0f, renderStyle);
        }

        if (!versionFont) {
            versionFont = make_shared<Font>("fonts/Neo Sans Pro Bold.ttf", 10.0f, renderStyle);
        }

        GFXSYS.setClearColor(vec3(0, 0, 0));
        for (int i = 0; i < MENU_OPTION_COUNT; i++) {
            menuOptionPosX[i] = menuOptionPosXOld[i] = 1100.0f;
            if (!textLayouts[i]) {
                textLayouts[i] = shared_ptr<TextLayout>(new TextLayout(string(menuOptions[i]), Box(0, 0, 1024, 768), menuFont.get()));
            }
        }
        menuTime = 0;

        AUDIOSYS.playMusic("menu3.ogg");
    }

    void GameStateMenu::onExit(Stardust* game) {
//        AUDIOSYS.stopSounds({"UI"});
    }

    static const float SECONDS_UNTIL_FIRST_REPEAT = 0.3f;
    static const float SECONDS_UNTIL_NEXT_REPEATS = 0.05f;

    static int repeatDirection = 0;
    static float repeatCounter = SECONDS_UNTIL_FIRST_REPEAT;
    static bool optionChanged = false;


    void GameStateMenu::update(Stardust* game, float elapsedSeconds) {
        optionChanged = false;
        if (INPUT.isKeyJustPressed(SDL_SCANCODE_DOWN)) {
            selectedIndex++;
            repeatDirection = 1;
            repeatCounter = SECONDS_UNTIL_FIRST_REPEAT;
            optionChanged = true;
        } else if (INPUT.isKeyJustPressed(SDL_SCANCODE_UP)) {
            selectedIndex--;
            repeatDirection = -1;
            repeatCounter = SECONDS_UNTIL_FIRST_REPEAT;
            optionChanged = true;
        } else if ((INPUT.isKeyDown(SDL_SCANCODE_DOWN) && repeatDirection == 1) || (INPUT.isKeyDown(SDL_SCANCODE_UP) && repeatDirection == -1)
                ) {
            repeatCounter -= elapsedSeconds;
            if (repeatCounter < 0.0f) {
                selectedIndex += repeatDirection;
                repeatCounter = SECONDS_UNTIL_NEXT_REPEATS;
                optionChanged = true;
            }
        }

        if (optionChanged) {
            selectedIndex = selectedIndex % MENU_OPTION_COUNT;
            if (selectedIndex < 0)
                selectedIndex += MENU_OPTION_COUNT;
            AUDIOSYS.playSound(SOUND_OPTION_CHANGE, AudioGroup::byName("UI"), 80);
        }

        if (INPUT.isKeyJustPressed(SDL_SCANCODE_RETURN)) {
            handleMenuSelection(game, selectedIndex);
        } else {
            // menu animation
            menuTime += elapsedSeconds;
            const float MENU_INTRO_TIME = SECONDS_UNTIL_FIRST_REPEAT;
            if (menuTime <= MENU_INTRO_TIME) {
                float t = menuTime / MENU_INTRO_TIME;
                for (int i = 0; i < MENU_OPTION_COUNT; i++) {
                    menuOptionPosXOld[i] = menuOptionPosX[i];
                    menuOptionPosX[i] = (1.0f - easeOutCubic(t)) * 1000.0f + 100;
                }
            } else {
                for (int i = 0; i < MENU_OPTION_COUNT; i++) {
                    menuOptionPosXOld[i] = menuOptionPosX[i] = 100;
                }
            }
        }
    }

    void GameStateMenu::render(Stardust* game, float alpha) {
        GFXSYS.clearScreen();

        GFXSYS.setTransparencyMode(TransparencyMode::Transparent);
        for (int i = 0; i < MENU_OPTION_COUNT; i++) {
            float x = glm::mix(menuOptionPosXOld[i], menuOptionPosX[i], alpha);
            float y = 80 * i + 100;
            vec4 textColor = selectedIndex == i ? SELECTED_OPTION_COLOR : OPTION_COLOR;
            vec4 outlineColor = vec4(0.1, 0.1, 0.1, 0.1);
            menuFont->print(x, y, string(menuOptions[i]), textColor, outlineColor, FontPrintStyle::OUTLINE);
        }

        versionFont->print(800, 700, STARDUST_VERSION, vec4(1, 1, 1, 1), vec4(0.1, 0.1, 0.1, 0.9), FontPrintStyle::SHADOW);

        GFXSYS.show();
    }

    void GameStateMenu::handleMenuSelection(Stardust* game, int selectedIndex) {
        switch (selectedIndex) {
            case 0:
                AUDIOSYS.playSound(SOUND_OPTION_ACCEPTED, AudioGroup::byName("UI"));
                game->gameSession->reset();
                game->fsm->changeState(&GameStateNewGame::instance());
                break;
            case 4:
                AUDIOSYS.playSound(SOUND_OPTION_ACCEPTED, AudioGroup::byName("UI"));
                game->fsm->changeState(&GameStateOptions::instance());
                break;
            case 5:
                AUDIOSYS.playSound(SOUND_OPTION_ACCEPTED, AudioGroup::byName("UI"));
                game->fsm->changeState(&GameStateCredits::instance());
                break;
            case 6:
                AUDIOSYS.playSound(SOUND_OPTION_ACCEPTED, AudioGroup::byName("UI"));
                game->quitGame = true;
                break;
            default:
                AUDIOSYS.playSound(SOUND_OPTION_REJECTED, AudioGroup::byName("UI"));
                break;
        }
    }
} // namespace Acidrain
