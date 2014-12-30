#include <GameStateMenu.h>
#include <Stardust.h>
#include <Font.h>
#include <GfxSystem.h>
#include <InputProvider.h>
#include <Math.h>
#include <AudioSystem.h>
#include <GameStatePlayLevel.h>

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

    static int selectedIndex = 0;

    static float menuTime = 0;

    static vec4 OPTION_COLOR = vec4(1, 1, 1, 0.8f);

    static vec4 SELECTED_OPTION_COLOR = vec4(1, 0, 0, 0.8f);

    shared_ptr<Sound> menuOptionChangeSound;
    shared_ptr<Sound> menuOptionRejectedSound;
    shared_ptr<Sound> menuOptionAcceptedSound;

    void GameStateMenu::onEnter(Stardust* game) {
        if (!menuFont)
            menuFont = make_shared<Font>("fonts/Neo Sans Pro Bold.ttf", 40.0f);

        if (!menuOptionChangeSound)
            menuOptionChangeSound = AUDIOSYS.loadSound("sounds/menu/option-changed.ogg");

        if (!menuOptionRejectedSound)
            menuOptionRejectedSound = AUDIOSYS.loadSound("sounds/menu/option-rejected.ogg");

        if (!menuOptionAcceptedSound)
            menuOptionAcceptedSound = AUDIOSYS.loadSound("sounds/menu/option-accepted.ogg");

        GFXSYS.setClearColor(vec3(0, 0, 0));

        for (int i = 0; i < MENU_OPTION_COUNT; i++) {
            menuOptionPosX[i] = menuOptionPosXOld[i] = 1100.0f;
        }
        menuTime = 0;
    }

    void GameStateMenu::onExit(Stardust* game) {
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
            menuOptionChangeSound->play(-1, 0);
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
                    menuOptionPosX[i] = (1.0f - easeOutCubic(t)) * 1000.0f + 10;
                }
            } else {
                for (int i = 0; i < MENU_OPTION_COUNT; i++) {
                    menuOptionPosXOld[i] = menuOptionPosX[i] = 10;
                }
            }
        }
    }

    void GameStateMenu::render(Stardust* game, float alpha) {
        GFXSYS.clearScreen();

        GFXSYS.setTransparencyMode(TransparencyMode::Additive);
        for (int i = 0; i < MENU_OPTION_COUNT; i++) {
            menuFont->print(
                    glm::mix(menuOptionPosXOld[i], menuOptionPosX[i], alpha),
                    80 * i + 100,
                    menuOptions[i],
                    selectedIndex == i ? SELECTED_OPTION_COLOR : OPTION_COLOR
            );
        }
        GFXSYS.show();
    }

    void GameStateMenu::handleMenuSelection(Stardust* game, int selectedIndex) {
        switch (selectedIndex) {
            case 0:
                menuOptionAcceptedSound->play(-1, 0);
                game->fsm->changeState(&GameStatePlayLevel::instance());
                break;
            case 6:
                menuOptionAcceptedSound->play(31, 0);
                while (Mix_Playing(31)) {
                    // do nothing
                }
                game->quitGame = true;
                break;
            default:
                menuOptionRejectedSound->play(-1, 0);
                break;
        }
    }
} // namespace Acidrain