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
            if (selectedIndex < 0)
                selectedIndex += entries.size();
            selectedIndex = (int) (selectedIndex % entries.size());
            AUDIOSYS.playSound(config->SOUND_OPTION_CHANGE.c_str(), AudioGroup::byName("UI"), 80);
        }

        shared_ptr<MenuEntry> selectedEntry = entries.at(selectedIndex);
        if (INPUT.isKeyJustPressed(SDL_SCANCODE_RIGHT)) {
            selectedEntry->selectNextValue();
            AUDIOSYS.playSound(config->SOUND_OPTION_CHANGE.c_str(), AudioGroup::byName("UI"), 80);
        } else if (INPUT.isKeyJustPressed(SDL_SCANCODE_LEFT)) {
            selectedEntry->selectPreviousValue();
            AUDIOSYS.playSound(config->SOUND_OPTION_CHANGE.c_str(), AudioGroup::byName("UI"), 80);
        }

        if (INPUT.isKeyJustPressed(SDL_SCANCODE_RETURN)) {
            selectedEntry->select();
        }

        // menu animation
        menuTime += elapsedSeconds;
        const float MENU_INTRO_TIME = config->SECONDS_UNTIL_FIRST_REPEAT;
        if (menuTime <= MENU_INTRO_TIME) {
            float t = menuTime / MENU_INTRO_TIME;
            for (auto& entry : entries) {
                entry->oldPos.x = entry->pos.x;
                entry->pos.x = (1.0f - easeOutCubic(t)) * 1000.0f + 100;
            }
        } else {
            for (auto& entry : entries)
                entry->oldPos.x = entry->pos.x = 100;
        }
    }

    void Menu::render(float alpha) {
        GFXSYS.setTransparencyMode(TransparencyMode::Additive);
        int i = 0;
        for (auto& entry : entries) {
            bool isSelected = selectedIndex == i;
            renderEntry(alpha, i, entry, isSelected);
            i++;
        }

        config->versionFont->print(800, 700, STARDUST_VERSION, vec4(1, 1, 1, 1));
    }

    void Menu::renderEntry(float alpha, int i, shared_ptr<MenuEntry>& entry, bool isSelected) const {
        config->menuFont->print(
                glm::mix(entry->oldPos.x, entry->pos.x, alpha),
                80 * i + 100,
                entry->getTitle(),
                isSelected ? config->SELECTED_OPTION_COLOR : config->OPTION_COLOR
        );
        config->menuFont->print(
                glm::mix(entry->oldPos.x, entry->pos.x, alpha) + 500,
                80 * i + 100,
                entry->getCurrentValue(),
                entry->isCurrentValueApplied() ? config->APPLIED_VALUE_COLOR : config->NOT_APPLIED_VALUE_COLOR
        );
    }

    void Menu::reset() {
        menuTime = 0;
        selectedIndex = 0;

        repeatDirection = 0;
        repeatCounter = config->SECONDS_UNTIL_FIRST_REPEAT;
        optionChanged = false;

        for (auto& entry : entries)
            entry->pos.x = entry->oldPos.x = 1100.0f;
    }

    void Menu::addEntry(shared_ptr<MenuEntry> entry) {
        entry->pos.x = entry->oldPos.x = 1100.0f;
        entries.push_back(entry);
    }
} // namespace Acidrain