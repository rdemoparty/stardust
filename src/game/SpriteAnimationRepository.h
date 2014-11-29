#pragma once
#include <string>
#include <memory>
#include <map>

#define ANIMREPO Acidrain::SpriteAnimationRepository::getInstance()

namespace Acidrain {

    using namespace std;

    class Animation;
    class AnimationData;
    class SpriteSheet;

    class SpriteAnimationRepository {
    public:
        static SpriteAnimationRepository& getInstance();

        void initialize(std::string filename);

        Animation* newAnimation(std::string name);

    private:
        SpriteAnimationRepository();
        map<string, AnimationData*> animationData;
        map<string, SpriteSheet*> spriteSheets;
    };

}
