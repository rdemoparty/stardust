#pragma once

#define USERPREFS Acidrain::UserPreferences::getInstance()

namespace Acidrain {

    class UserPreferences {
    public:
        static UserPreferences& getInstance();

        int width;
        int height;
        bool fullscreen;
        bool vSync;

        void save();
    private:
        UserPreferences();

        void load();

    };

} // end of namespace Acidrain
