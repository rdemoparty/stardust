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

        void addSlide(Slide* slide);
    };

    struct Slide {
        explicit Slide(shared_ptr<Texture> texture, float seconds);

        void setFadeSeconds(float fadeInTime, float fadeOutTime);
        void addCaption(string content, vec2 position, vec4 color = vec4(1, 1, 1, 1));
        void addCaption(Caption* caption);

        shared_ptr<Texture> texture;
        vector<shared_ptr<Caption>> captions;
        float seconds = 0;
        float fadeInSeconds = 0;
        float fadeOutSeconds = 0;
    };

    struct Caption {
        string content;
        vec2 position = vec2(0, 0);
        vec4 color = vec4(1, 1, 1, 1);
        float secondsFromSlideStart = 0;
        float fadeInSeconds = 0;
        float fadeOutSeconds = 0;
        float displaySeconds = 0;

        static Caption* create(string content);
        Caption* atPosition(vec2 position);
        Caption* withColor(vec4 color);
        Caption* activeForSeconds(float seconds);
        Caption* withDelayFromSlideStart(float seconds);
        Caption* withFading(float fadeInSeconds, float fadeOutSeconds);
    };

    class CutScenePlayer {
    public:
        explicit CutScenePlayer(shared_ptr<CutScene> cutScene, shared_ptr<Font> captionFont);

        void start();
        void skip();
        void update(float dt);
        void render();

        inline bool isFinished() const { return finished; };

    private:
        shared_ptr<CutScene> cutScene;
        shared_ptr<Font> captionFont;
        float frameFadingAlpha;
        float timeInCurrentSlide;
        int currentSlideIndex;
        bool finished;
        bool playing;

        void renderSlideImage(shared_ptr<Texture> texture);
        void renderSlideCaption(shared_ptr<Caption> caption);
        float computeCaptionFadingAlpha(const shared_ptr<Caption>& caption, float secondsSinceCaptionStart) const;
        void computeFrameFadingAlpha(const Slide* slide);
        bool noMoreSlidesAvailable() const;
        bool isCurrentSlideFinished() const;
        void selectNextSlide();
        void finishPlaying();
    };
}