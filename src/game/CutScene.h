#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <string>
#include <vector>
#include <memory>
#include <Texture.h>

namespace Acidrain {

    using namespace std;
    using namespace glm;

    struct Slide;
    struct Caption;
    class Font;

    struct CutScene {
        string musicUri;
        string speechUri;
        vector<shared_ptr<Slide>> slides;
        bool skippable = true;
    };

    struct Slide {
        explicit Slide(shared_ptr<Texture> texture, float seconds);
        void addCaption(string content, vec2 position, vec4 color = vec4(1, 1, 1, 1));

        shared_ptr<Texture> texture;
        vector<shared_ptr<Caption>> captions;
        float seconds;
    };

    struct Caption {
        string content;
        vec2 position;
        vec4 color;
        float secondsFromSlideStart;
        float fadeInSeconds;
        float fadeOutSeconds;
        float displaySeconds;
    };

    class CutScenePlayer {
    public:
        explicit CutScenePlayer(shared_ptr<CutScene> cutScene);

        void start();
        void update(float dt);
        void render();

        inline bool isFinished() const { return finished; };

    private:

        void renderSlideImage(shared_ptr<Texture> texture);
        void renderSlideCaption(shared_ptr<Caption> caption);

        shared_ptr<Font> captionFont;
        shared_ptr<CutScene> cutScene;
        float timeInCurrentSlide;
        int currentSlideIndex;
        bool finished;
        bool playing;
    };

}