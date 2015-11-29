#include <AudioScriptExporter.h>
#include <easylogging++.h>
#include <ScriptUtils.h>
#include <AudioSystem.h>

namespace Acidrain {

    AudioScriptExporter::AudioScriptExporter() {
    }

    AudioScriptExporter& AudioScriptExporter::getInstance() {
        static AudioScriptExporter instance;
        return instance;
    }

    static int playSound(lua_State* L) {
        const char* const URI = lua_tostring(L, 1);
        const char* const AUDIO_GROUP = lua_tostring(L, 2);
        AUDIOSYS.playSound(URI, AUDIO_GROUP);
        return 0; // arguments pushed on stack
    }

    static int playSong(lua_State* L) {
        const char* const uri = lua_tostring(L, 1);
        AUDIOSYS.playMusic(uri);
        return 0; // arguments pushed on stack
    }

    void AudioScriptExporter::exportToScript(lua_State* L) const {
        lua_register(L, "playSound", playSound);
        lua_register(L, "playSong", playSong);
    }
}