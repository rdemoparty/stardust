#pragma once

#include <string>

#define USERPREFS Acidrain::UserPreferences::getInstance()

namespace Acidrain {

    using namespace std;

    class UserPreferences {
    public:
        static UserPreferences& getInstance();

        int width;
        int height;
        bool fullscreen;
        bool vsync;

        void init();

        void save();

        string getFullPathToPreferencesFile();

        string getFullPathToPreferencesFolder();

    private:
        UserPreferences();

        void load();

        void setDefaults();
    };

} // end of namespace Acidrain
