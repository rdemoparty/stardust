#include <GameStateOptions.h>
#include <Stardust.h>
#include <Font.h>
#include <GfxSystem.h>
#include <InputProvider.h>
#include <GameStateMenu.h>
#include <Menu.h>
#include <easylogging++.h>
#include <SDL_video.h>
#include <UserPreferences.h>

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
        virtual bool isCurrentValueApplied() override;
        virtual void selectNextValue() override;
        virtual void selectPreviousValue() override;

    private:
        string title = "Resolution";
        vector<MenuEntryResolutionItem> values;
        int selectedValue = 0;
        int appliedValue = 0;
    };


    struct MenuEntryFullscreenItem {
        MenuEntryFullscreenItem(bool fullscreen) {
            this->state = fullscreen;
            description = fullscreen ? "on" : "off";
        }

        bool state;
        string description;
    };

    class MenuEntryFullscreen : public MenuEntry {
    public:
        MenuEntryFullscreen();

        virtual string getTitle() override;
        virtual string getCurrentValue() override;
        virtual void select() override;
        virtual bool isCurrentValueApplied() override;
        virtual void selectNextValue() override;
        virtual void selectPreviousValue() override;

    private:
        string title = "Fullscreen";
        vector<MenuEntryFullscreenItem> values;
        int selectedValue = 0;
        int appliedValue = 0;
    };


    struct MenuEntryVSyncItem {
        MenuEntryVSyncItem(bool vsync) {
            this->state = vsync;
            description = vsync ? "on" : "off";
        }

        bool state;
        string description;
    };

    class MenuEntryVSync : public MenuEntry {
    public:
        MenuEntryVSync();

        virtual string getTitle() override;
        virtual string getCurrentValue() override;
        virtual void select() override;
        virtual bool isCurrentValueApplied() override;
        virtual void selectNextValue() override;
        virtual void selectPreviousValue() override;

    private:
        string title = "VSync";
        vector<MenuEntryVSyncItem> values;
        int selectedValue = 0;
        int appliedValue = 0;
    };


    // ----------------------------------------------------------------------

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
        menu->addEntry(make_shared<MenuEntryFullscreen>());
        menu->addEntry(make_shared<MenuEntryVSync>());
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

        // set current value
        for (size_t i = 0; i < values.size(); i++) {
            MenuEntryResolutionItem entry = values.at(i);
            if (entry.width == GFXSYS.windowWidth() && entry.height == GFXSYS.windowHeight()) {
                selectedValue = i;
                appliedValue = i;
                break;
            }
        }
    }

    string MenuEntryResolution::getTitle() {
        return title;
    }

    string MenuEntryResolution::getCurrentValue() {
        return values.at(selectedValue).description;
    }

    void MenuEntryResolution::select() {
        if (appliedValue != selectedValue) {
            MenuEntryResolutionItem selectedItem = values.at(selectedValue);
            GFXSYS.resizeDisplayTo(selectedItem.width, selectedItem.height, GFXSYS.isFullscreen());
            USERPREFS.width = selectedItem.width;
            USERPREFS.height = selectedItem.height;
            USERPREFS.save();

            appliedValue = selectedValue;
        }
    }

    bool MenuEntryResolution::isCurrentValueApplied() {
        return appliedValue == selectedValue;
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

    // ------------------------------------------------------------------------

    MenuEntryFullscreen::MenuEntryFullscreen() {
        values.push_back(MenuEntryFullscreenItem(true));
        values.push_back(MenuEntryFullscreenItem(false));

        selectedValue = GFXSYS.isFullscreen() ? 0 : 1;
        appliedValue = selectedValue;
    }

    string MenuEntryFullscreen::getTitle() {
        return title;
    }

    string MenuEntryFullscreen::getCurrentValue() {
        return values.at(selectedValue).description;
    }

    void MenuEntryFullscreen::selectNextValue() {
        selectedValue++;
        selectedValue %= values.size();
    }

    void MenuEntryFullscreen::selectPreviousValue() {
        selectedValue--;
        if (selectedValue < 0)
            selectedValue += values.size();
    }

    void MenuEntryFullscreen::select() {
        if (selectedValue != appliedValue) {
            bool fullscreen = values.at(selectedValue).state;
            GFXSYS.resizeDisplayTo(GFXSYS.windowWidth(), GFXSYS.windowHeight(), fullscreen);
            USERPREFS.fullscreen = fullscreen;
            USERPREFS.save();

            appliedValue = selectedValue;
        }
    }

    bool MenuEntryFullscreen::isCurrentValueApplied() {
        return appliedValue == selectedValue;
    }

    // ------------------------------------------------------------------------

    MenuEntryVSync::MenuEntryVSync() {
        values.push_back(MenuEntryVSyncItem(true));
        values.push_back(MenuEntryVSyncItem(false));

        selectedValue = GFXSYS.isVSyncOn() ? 0 : 1;
        appliedValue = selectedValue;
    }

    string MenuEntryVSync::getTitle() {
        return title;
    }

    string MenuEntryVSync::getCurrentValue() {
        return values.at(selectedValue).description;
    }

    void MenuEntryVSync::selectNextValue() {
        selectedValue++;
        selectedValue %= values.size();
    }

    void MenuEntryVSync::selectPreviousValue() {
        selectedValue--;
        if (selectedValue < 0)
            selectedValue += values.size();
    }

    void MenuEntryVSync::select() {
        if (selectedValue != appliedValue) {
            bool state = values.at(selectedValue).state;
            GFXSYS.setVSync(state);
            USERPREFS.vsync = state;
            USERPREFS.save();

            appliedValue = selectedValue;
        }
    }

    bool MenuEntryVSync::isCurrentValueApplied() {
        return appliedValue == selectedValue;
    }

} // namespace Acidrain
