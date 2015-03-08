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
        offset = 0;
    }

    void LevelScript::update(float elapsedSeconds) {
        offset += elapsedSeconds * pixelsToScrollPerSecond;
        for (auto& e : events) {
            if (!e.created && e.position.y <= offset) {
                GameObject* entity = scene->createByName(e.recipeName);
                entity->currentState.position.x = e.position.x;
                entity->currentState.position.y = e.position.y - offset;
                entity->previousState = entity->currentState;
                scene->add(entity);

                e.created = true;
            }
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
        LOG(INFO) << "Loading level script from \"" << scriptURI << "\"";
        string content = FILESYS.getFileContent(scriptURI);

        string parseError;
        auto json = Json::parse(content, parseError);
        if (parseError.empty()) {
            for (auto& e : json.object_items()) {
                const string& param = e.first;
                if (icompare(param, "events"))
                    for (auto& k : e.second.array_items())
                        addEvent(readEvent(k));
                else if (icompare(param, "speed"))
                    pixelsToScrollPerSecond = (float) e.second.number_value();
                else if (icompare(param, "name"))
                    LOG(INFO) << "Read level name " << e.second.string_value();
                else if (icompare(param, "length"))
                    LOG(INFO) << "Read level length " << e.second.int_value();
                else {
                    LOG(WARNING) << "Unknown level script attribute \"" << param << "\"";
                }
            }
        } else {
            LOG(FATAL) << "Error while parsing JSON content from " << scriptURI << ". Error: " << parseError;
        }
    }

    void LevelScript::addEvent(LevelScriptEntry entry) {
        events.push_back(entry);
    }
}