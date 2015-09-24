#include <UserPreferences.h>
#include <json11.hpp>
#include <fstream>
#include <easylogging++.h>
#include <SDL.h>
#include <SDL_video.h>
#include <CommandLineParser.h>

DEFINE_int(width, w, "The physical window width", -1)
DEFINE_int(height, h, "The physical window height", -1)
DEFINE_bool(fullscreen, f, "Whether the window is fullscreen or not", true)
DEFINE_bool(vsync, v, "Whether vsync is enabled or not", false)

namespace Acidrain {
    using namespace std;
    using namespace json11;

    const char* PATH_FROM_USER_HOME = "config.ini";

    UserPreferences& UserPreferences::getInstance() {
        static UserPreferences instance;
        return instance;
    }

    UserPreferences::UserPreferences() {
        // TODO: create user preferences file with defaults if does not exist
    }

    void UserPreferences::load() {
        std::ifstream file(PATH_FROM_USER_HOME);
        if (file.good()) {
            string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());

            string errorDescription;
            auto json = Json::parse(content, errorDescription);

            if (!errorDescription.empty()) {
                LOG(ERROR) << "Read empty content from user preferences file";
            }

            width = json["width"].int_value();
            height = json["height"].int_value();
            fullscreen = json["fullscreen"].bool_value();
            vsync = json["vsync"].bool_value();
        } else {
            LOG(WARNING) << "Failed to open user preferences file " << PATH_FROM_USER_HOME;
            setDefaults();
            save();
        }

        if (IS_SPECIFIED(width)) {
            width = FLAG_width;
            LOG(INFO) << "Overriding user preferences setting for window width with argument provided value of " << width;
        }

        if (IS_SPECIFIED(height)) {
            height = FLAG_height;
            LOG(INFO) << "Overriding user preferences setting for window height with argument provided value of " << height;
        }

        if (IS_SPECIFIED(vsync)) {
            vsync = FLAG_vsync;
            LOG(INFO) << "Overriding user preferences setting for vsync with argument provided value of " << vsync;
        }

        if (IS_SPECIFIED(fullscreen)) {
            fullscreen = FLAG_fullscreen;
            LOG(INFO) << "Overriding user preferences setting for fullscreen with argument provided value of " << fullscreen;
        }
    }

    void UserPreferences::save() {
        LOG(INFO) << "Saving user preferences to " << PATH_FROM_USER_HOME;
        Json preferences = Json::object({
                                                {"width",      width},
                                                {"height",     height},
                                                {"fullscreen", fullscreen},
                                                {"vsync",      vsync},
                                        });

        ofstream file;
        file.open(PATH_FROM_USER_HOME);
        file << preferences.dump();
        file.close();
    }

    void UserPreferences::init() {
        LOG(INFO) << "Initializing user preferences from file " << PATH_FROM_USER_HOME;
        load();
    }

    void UserPreferences::setDefaults() {
        LOG(INFO) << "Setting default user preferences";

        LOG(INFO) << "Autodetecting window size";
        SDL_DisplayMode displayMode;
        if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0)
            LOG(FATAL) << "SDL_GetCurrentDisplayMode failed: " << SDL_GetError();

        width = displayMode.w;
        height = displayMode.h;
        fullscreen = true;
        vsync = true;
    }
} // end of namespace Acidrain
