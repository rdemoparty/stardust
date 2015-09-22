#pragma once

#include <string>
#include <vector>
#include <queue>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <memory>
#include <Font.h>

namespace Acidrain {

    using namespace std;
    using namespace glm;

    struct MenuConfig {
        MenuConfig(shared_ptr<Font> menuFont, shared_ptr<Font> versionFont) {
            this->menuFont = menuFont;
            this->versionFont = versionFont;
        }

        vec4 OPTION_COLOR = vec4(1.0f, 1.0f, 1.0f, 0.8f);
        vec4 SELECTED_OPTION_COLOR = vec4(1.0f, 0.0f, 0.0f, 0.8f);

        string SOUND_OPTION_REJECTED = "sounds/menu/option-rejected.ogg";
        string SOUND_OPTION_ACCEPTED = "sounds/menu/option-accepted.ogg";
        string SOUND_OPTION_CHANGE = "sounds/menu/option-changed.ogg";

        float SECONDS_UNTIL_FIRST_REPEAT = 0.3f;
        float SECONDS_UNTIL_NEXT_REPEATS = 0.05f;

        shared_ptr<Font> menuFont;
        shared_ptr<Font> versionFont;
    };

    struct MenuItem {
        int id;
        vec2 pos;
        vec2 oldPos;
        string title;
        string value;
    };

    class MenuOptionChangeListener {
    public:
        virtual void onChange(const MenuItem& item) = 0;
    };

    class Menu {
    public:
        Menu(shared_ptr<MenuConfig> config);

        void addItem(MenuItem item);

        void update(float elapsedSeconds);

        void render(float alpha);

        int getSelectedIndex() const;

        void addOnChangeListener(MenuOptionChangeListener* listener);

        void reset();

    private:
        shared_ptr<MenuConfig> config;
        vector<MenuItem> items;
        vector<MenuOptionChangeListener*> listeners;
        int selectedIndex;
        float menuTime;

        int repeatDirection;
        float repeatCounter;
        bool optionChanged;
    };

} // namespace Acidrain