#include "SpriteAnimationRepository.h"
#include <json11.hpp>
#include <Animation.h>
#include <FileSystem.h>
#include <iostream>
#include <GfxSystem.h>

namespace Acidrain {

    using namespace std;
    using namespace json11;

    SpriteAnimationRepository &SpriteAnimationRepository::getInstance() {
        static SpriteAnimationRepository instance;
        return instance;
    }


    AnimationLoopType animationLoopType(string name) {
        if (name == "NONE") return AnimationLoopType::NONE;
        else if (name == "FORWARD") return AnimationLoopType::FORWARD;
        else if (name == "REVERSE") return AnimationLoopType::REVERSE;
        else if (name == "PING_PONG") return AnimationLoopType::PING_PONG;
    }

    void SpriteAnimationRepository::initialize(std::string filename) {
        string animationContent = FILESYS.getFileContent(filename);

        string parseError;
        auto json = Json::parse(animationContent, parseError);
        if (!parseError.empty()) {
            cout << "Error while parsing " << filename << endl;
        } else {
            for (auto &k : json["spriteSheets"].array_items()) {
                SpriteSheet *sheet = new SpriteSheet();
                sheet->texture = GFXSYS.loadTexture(k["texture"].string_value());
                sheet->autoAdd(k["spriteWidth"].int_value(), k["spriteHeight"].int_value());
                spriteSheets[k["name"].string_value()] = sheet;
            }

            for (auto &k : json["animations"].array_items()) {
                AnimationData *animData = new AnimationData();
                animationData[k["name"].string_value()] = animData;

                animData->frameTimeInMilliseconds = (float) k["frameTimeInMilliseconds"].number_value();
                animData->loopType = animationLoopType(k["loopType"].string_value());

                for (auto &f : k["frames"].array_items()) {
                    animData->frames.push_back(
                            Sprite(spriteSheets[f["spriteSheet"].string_value()],
                                    f["index"].int_value()));
                }
            }
        }
    }

    SpriteAnimationRepository::SpriteAnimationRepository() {
    }

    Animation *SpriteAnimationRepository::newAnimation(std::string name) {
        AnimationData *animData = animationData[name];
        if (animData == nullptr) {
            cout << "Error: cannot retrieve animation data with name [" << name << "]" << endl;
        }
        return new Animation(animData);
    }
}