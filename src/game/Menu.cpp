#include <Menu.h>
#include <InputProvider.h>
#include <Version.h>
#include <AudioSystem.h>
#include <GfxSystem.h>
#include <MathSupport.h>

namespace Acidrain {

    Menu::Menu(shared_ptr<MenuConfig> config) {
        this->config = config;
        reset();
    }

    void Menu::addItem(MenuItem item) {
        item.pos.x = item.oldPos.x = 1100.0f;
        items.push_back(item);
    }

    int Menu::getSelectedIndex() const { return selectedIndex; }

    void Menu::addOnChangeListener(MenuOptionChangeListener* listener) {
        listeners.push_back(listener);
    };

    void Menu::update(float elapsedSeconds) {
        optionChanged = false;
        if (INPUT.isKeyJustPressed(SDL_SCANCODE_DOWN)) {
            selectedIndex++;
            repeatDirection = 1;
            repeatCounter = config->SECONDS_UNTIL_FIRST_REPEAT;
            optionChanged = true;
        } else if (INPUT.isKeyJustPressed(SDL_SCANCODE_UP)) {
            selectedIndex--;
            repeatDirection = -1;
            repeatCounter = config->SECONDS_UNTIL_FIRST_REPEAT;
            optionChanged = true;
        } else if ((INPUT.isKeyDown(SDL_SCANCODE_DOWN) && repeatDirection == 1) ||
                   (INPUT.isKeyDown(SDL_SCANCODE_UP) && repeatDirection == -1)
                ) {
            repeatCounter -= elapsedSeconds;
            if (repeatCounter < 0.0f) {
                selectedIndex += repeatDirection;
                repeatCounter = config->SECONDS_UNTIL_NEXT_REPEATS;
                optionChanged = true;
            }
        }

        if (optionChanged) {
            selectedIndex = (int) (selectedIndex % items.size());
            if (selectedIndex < 0)
                selectedIndex += items.size();
            AUDIOSYS.playSound(config->SOUND_OPTION_CHANGE.c_str(), AudioGroup::byName("UI"), 80);
        }

        if (INPUT.isKeyJustPressed(SDL_SCANCODE_RETURN)) {
//                handleMenuSelection(game, selectedIndex);
        } else {
            // menu animation
            menuTime += elapsedSeconds;
            const float MENU_INTRO_TIME = config->SECONDS_UNTIL_FIRST_REPEAT;
            if (menuTime <= MENU_INTRO_TIME) {
                float t = menuTime / MENU_INTRO_TIME;
                for (auto& item : items) {
                    item.oldPos.x = item.pos.x;
                    item.pos.x = (1.0f - easeOutCubic(t)) * 1000.0f + 100;
                }
            } else {
                for (auto& item : items) {
                    item.oldPos.x = item.pos.x = 100;
                }
            }
        }
    };

    void Menu::render(float alpha) {
        GFXSYS.setTransparencyMode(TransparencyMode::Additive);
        int i = 0;
        for (auto& item : items) {
            config->menuFont->print(
                    glm::mix(item.oldPos.x, item.pos.x, alpha),
                    80 * i + 100,
                    item.title.c_str(),
                    selectedIndex == i ? config->SELECTED_OPTION_COLOR : config->OPTION_COLOR
            );
            i++;
        }

        config->versionFont->print(800, 700, STARDUST_VERSION, vec4(1, 1, 1, 1));
    }

    void Menu::reset() {
        menuTime = 0;
        selectedIndex = 0;

        repeatDirection = 0;
        repeatCounter = config->SECONDS_UNTIL_FIRST_REPEAT;
        optionChanged = false;

        for (auto& item : items)
            item.oldPos.x = item.pos.x = 1100.0f;
    }

} // namespace Acidrain