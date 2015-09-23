#include <GameStateOptions.h>
#include <Stardust.h>
#include <Font.h>
#include <GfxSystem.h>
#include <InputProvider.h>
#include <GameStateMenu.h>
#include <Menu.h>
#include <easylogging++.h>
#include <SDL_video.h>

namespace Acidrain {

    struct MenuEntryResolutionItem {
        MenuEntryResolutionItem(int w, int h) {
            this->width = w;
            this->height = h;

            stringstream ss;
            ss << width << "x" << height;
            description = ss.str();
        }

        int width;
        int height;
        string description;
    };

    class MenuEntryResolution : public MenuEntry {
    public:
        MenuEntryResolution();

        virtual string getTitle() override;

        virtual string getCurrentValue() override;

        virtual void select() override;

        virtual void selectNextValue() override;

        virtual void selectPreviousValue() override;

    private:
        vector<MenuEntryResolutionItem> values;
        int selectedValue = 0;
    };

    GameStateOptions& GameStateOptions::instance() {
        static GameStateOptions instance;
        return instance;
    }

    GameStateOptions::GameStateOptions() {
        shared_ptr<MenuConfig> menuConfig = shared_ptr<MenuConfig>(new MenuConfig(
                make_shared<Font>("fonts/Neo Sans Pro Bold.ttf", 40.0f),
                make_shared<Font>("fonts/Neo Sans Pro Bold.ttf", 10.0f)
        ));
        menu = make_shared<Menu>(menuConfig);
        setupMenu();
    }


    void GameStateOptions::setupMenu() const {
        menu->addEntry(make_shared<MenuEntryResolution>());
    }

    void GameStateOptions::onEnter(Stardust* game) {
        GFXSYS.setClearColor(vec3(0, 0, 0));
        menu->reset();
    }

    void GameStateOptions::onExit(Stardust* game) {
    }

    void GameStateOptions::update(Stardust* game, float elapsedSeconds) {
        if (INPUT.isKeyJustPressed(SDL_SCANCODE_ESCAPE)) {
            game->fsm->changeState(&GameStateMenu::instance());
            return;
        }
        menu->update(elapsedSeconds);
    }

    void GameStateOptions::render(Stardust* game, float alpha) {
        GFXSYS.clearScreen();
        menu->render(alpha);
        GFXSYS.show();
    }

    // -----------------------------------------------------------------------------------------

    MenuEntryResolution::MenuEntryResolution() {
        int displayIndex = 0;
        int numModes = SDL_GetNumDisplayModes(displayIndex);
        for (int modeIndex = 0; modeIndex < numModes; modeIndex++) {
            SDL_DisplayMode mode;
            if (SDL_GetDisplayMode(displayIndex, modeIndex, &mode) == 0) {
                if (mode.w >= 1024) {
                    values.push_back(MenuEntryResolutionItem(mode.w, mode.h));
                }
            } else {
                LOG(ERROR) << "Failed to get display mode " << modeIndex << " for display index " << displayIndex;
            }
        }
    }

    string MenuEntryResolution::getTitle() {
        return "Resolution";
    }

    string MenuEntryResolution::getCurrentValue() {
        return values.at(selectedValue).description;
    }

    void MenuEntryResolution::select() {
        MenuEntryResolutionItem selectedItem = values.at(selectedValue);
        GFXSYS.resizeDisplayTo(selectedItem.width, selectedItem.height, false);
    }

    void MenuEntryResolution::selectNextValue() {
        selectedValue++;
        selectedValue %= values.size();
    }

    void MenuEntryResolution::selectPreviousValue() {
        selectedValue--;
        if (selectedValue < 0)
            selectedValue += values.size();
    }
} // namespace Acidrain
