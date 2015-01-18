#include <easylogging++.h>
#include <LevelScript.h>
#include <Scene.h>
#include <FileSystem.h>
#include <json11.hpp>

namespace Acidrain {

    using namespace json11;

    // TODO: make me case insensitive indeed
    inline bool icompare(const string& a, const string& b) {
        return a == b;
    }

    LevelScript::LevelScript(Scene* s) : scene(s) {
    }

    LevelScript::~LevelScript() {
    }

    void LevelScript::reset() {
        timeUntilNextSpawn = 5;
    }

    void LevelScript::update(float elapsedSeconds) {
        timeUntilNextSpawn -= elapsedSeconds;
        if (timeUntilNextSpawn < 0) {
            GameObject* enemy = scene->createByName("enemy");
            enemy->currentState.position = vec2(rand() % 1024, -64);
            scene->add(enemy);
            timeUntilNextSpawn = rand() % 3 + 1;
        }
    }

    LevelScriptEntry readEvent(const Json& element) {
        LevelScriptEntry result;

        for (auto& e : element.object_items()) {
            const string& param = e.first;
            if (icompare(param, "recipe"))
                result.recipeName = e.second.string_value();
            else if (icompare(param, "x"))
                result.position.x = (float) e.second.number_value();
            else if (icompare(param, "y"))
                result.position.y = (float) e.second.number_value();
            else if (icompare(param, "layer"))
                result.layer = e.second.int_value();
            else {
                LOG(WARNING) << "Unknown level script event attribute \"" << param << "\"";
            }
        }
        return result;
    }

    void LevelScript::load(string scriptURI) {
        LOG(INFO) << "Loading game objects repository from \"" << scriptURI << "\"";
        string content = FILESYS.getFileContent(scriptURI);

        string parseError;
        auto json = Json::parse(content, parseError);
        if (parseError.empty())
            for (auto& k : json["events"].array_items())
                addEvent(readEvent(k));
        else
            LOG(FATAL) << "Error while parsing JSON content from " << scriptURI << ". Error: " << parseError;
    }

    void LevelScript::addEvent(LevelScriptEntry entry) {
        events.push_back(entry);
    }
}