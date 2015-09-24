#include <UserPreferences.h>
#include <json11.hpp>
#include <fstream>

namespace Acidrain {
    using namespace std;
    using namespace json11;

    const char* PATH_FROM_USER_HOME = "config.ini";

    UserPreferences& UserPreferences::getInstance() {
        static UserPreferences instance;
        return instance;
    }

    UserPreferences::UserPreferences() {
    }

    void UserPreferences::load() {
    }

    void UserPreferences::save() {
        Json preferences = Json::object({
                                                {"width",      width},
                                                {"height",     height},
                                                {"fullscreen", fullscreen},
                                                {"vsync",      vSync},
                                        });

        ofstream file;
        file.open(PATH_FROM_USER_HOME);
        file << preferences.dump();
        file.close();
    }
} // end of namespace Acidrain
