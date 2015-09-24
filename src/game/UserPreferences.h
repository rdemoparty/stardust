#pragma once

#define USERPREFS Acidrain::UserPreferences::getInstance()

namespace Acidrain {

    class UserPreferences {
    public:
        static UserPreferences& getInstance();

        int width;
        int height;
        bool fullscreen;
        bool vsync;

        void init();
        void save();
    private:
        UserPreferences();

        void load();
        void setDefaults();
    };

} // end of namespace Acidrain
