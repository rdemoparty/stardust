#include <easylogging++.h>
#include <SpriteAnimationRepository.h>
#include <json11.hpp>
#include <Animation.h>
#include <FileSystem.h>
#include <GfxSystem.h>
#include <stdexcept>

namespace Acidrain {

    using namespace std;
    using namespace json11;

    SpriteAnimationRepository& SpriteAnimationRepository::getInstance() {
        static SpriteAnimationRepository instance;
        return instance;
    }


    AnimationLoopType animationLoopType(string name) {
        if (name == "NONE")
            return AnimationLoopType::NONE;
        else if (name == "FORWARD")
            return AnimationLoopType::FORWARD;
        else if (name == "REVERSE")
            return AnimationLoopType::REVERSE;
        else if (name == "PING_PONG")
            return AnimationLoopType::PING_PONG;
        return AnimationLoopType::NONE;
    }

    void SpriteAnimationRepository::initialize(std::string filename) {
        LOG(INFO) << "Loading sprite animation repository from " << filename;
        string animationContent = FILESYS.getFileContent(filename);

        string parseError;
        auto json = Json::parse(animationContent, parseError);
        if (!parseError.empty()) {
            LOG(FATAL) << "Error while parsing JSON content from " << filename << ". Error: " << parseError;
        } else {
            for (auto& k : json["spriteSheets"].array_items()) {
                SpriteSheet* sheet = new SpriteSheet();
                sheet->texture = GFXSYS.loadTexture(k["texture"].string_value());
                sheet->autoAdd(k["spriteWidth"].int_value(), k["spriteHeight"].int_value());
                spriteSheets[k["name"].string_value()] = sheet;
            }

            for (auto& k : json["animations"].array_items()) {
                AnimationData* animData = new AnimationData();
                animationData[k["name"].string_value()] = animData;

                animData->frameTimeInMilliseconds = (float) k["frameTimeInMilliseconds"].number_value();
                animData->loopType = animationLoopType(k["loopType"].string_value());

                for (auto& f : k["frames"].array_items()) {
                    string spriteSheetName = f["spriteSheet"].string_value();
                    if (spriteSheets.find(spriteSheetName) == spriteSheets.end())
                        LOG(FATAL) << "Animation " << k["name"].string_value() << " refers to unexistent spritesheet \"" << spriteSheetName << "\"";

                    int index = f["index"].int_value();
                    int indexTo = f["indexTo"].int_value();

                    if (indexTo > index) {
                        for (int i = index; i <= indexTo; i++)
                            animData->frames.push_back(Sprite(spriteSheets[spriteSheetName], i));
                    } else {
                        animData->frames.push_back(Sprite(spriteSheets[spriteSheetName], index));
                    }
                }
            }
        }
    }

    SpriteAnimationRepository::SpriteAnimationRepository() {
    }

    Animation* SpriteAnimationRepository::newAnimation(std::string name) {
        if (animationData.find(name) == animationData.end()) {
            LOG(ERROR) << "No animation data with name " << name;
            throw runtime_error("Cannot find animation data with name " + name);
        } else {
            return new Animation(animationData[name]);
        }
    }
}