#include <CutScene.h>
#include <GfxSystem.h>
#include <Font.h>
#include <MathSupport.h>
#include <AudioSystem.h>
#include <easylogging++.h>
#include <glm/ext.hpp> // for glm::to_string

namespace Acidrain {

    void CutScene::addSlide(Slide* slide) {
        slides.push_back(shared_ptr<Slide>(slide));
    }

    Slide::Slide(shared_ptr<Texture> texture, float seconds) {
        this->texture = texture;
        this->seconds = seconds;

    }

    void Slide::setFadeSeconds(float fadeInTime, float fadeOutTime) {
        this->fadeInSeconds = fadeInTime;
        this->fadeOutSeconds = fadeOutTime;
    }

    void Slide::addCaption(Caption* caption) {
        captions.push_back(shared_ptr<Caption>(caption));
    }

    void Slide::addCaption(string content, vec2 position, vec4 color) {
        shared_ptr<Caption> caption = shared_ptr<Caption>(new Caption);
        caption->content = content;
        caption->position = position;
        caption->color = color;
        this->captions.push_back(caption);
    }


    Caption* Caption::create(string content) {
        Caption* result = new Caption;
        result->content = content;
        return result;
    }

    Caption* Caption::atPosition(vec2 position) {
        this->position = position;
        return this;
    }

    Caption* Caption::withColor(vec4 color) {
        this->color = color;
        return this;
    }

    Caption* Caption::activeForSeconds(float seconds) {
        this->displaySeconds = seconds;
        return this;
    }

    Caption* Caption::withDelayFromSlideStart(float seconds) {
        this->secondsFromSlideStart = seconds;
        return this;
    }

    Caption* Caption::withFading(float fadeInSeconds, float fadeOutSeconds) {
        this->fadeInSeconds = fadeInSeconds;
        this->fadeOutSeconds = fadeOutSeconds;
        return this;
    }

    // CutSceneRegistry

    map<int, shared_ptr<CutScene>> CutSceneRegistry::cutScenesBeforeLevel;
    map<int, shared_ptr<CutScene>> CutSceneRegistry::cutScenesAfterLevel;

    void CutSceneRegistry::registerCutScene(shared_ptr<CutScene> cutScene, int level, CutScenePosition position) {
        if (position == CutScenePosition::BeforeLevel)
            cutScenesBeforeLevel[level] = cutScene;
        else
            cutScenesAfterLevel[level] = cutScene;
    }

    shared_ptr<CutScene> CutSceneRegistry::getCutScene(int level, CutScenePosition position) {
        return position == CutScenePosition::BeforeLevel ?
            cutScenesBeforeLevel[level] :
            cutScenesAfterLevel[level];
    }

    CutScenePlayer::CutScenePlayer(shared_ptr<CutScene> cs, shared_ptr<Font> font)
            : cutScene(cs),
              captionFont(font),
              timeInCurrentSlide(0),
              currentSlideIndex(0),
              finished(false),
              playing(false) {
    }

    void CutScenePlayer::start() {
        playing = true;

        if (!cutScene->musicUri.empty()) {
            AUDIOSYS.playMusic(cutScene->musicUri.c_str());
        }
        if (!cutScene->speechUri.empty()) {
            AUDIOSYS.playSound(cutScene->speechUri, "PLAYER");
        }
    }

    void CutScenePlayer::skip() {
        finishPlaying();
    }

    void CutScenePlayer::update(float dt) {
        if (finished || !playing) return;

        timeInCurrentSlide += dt;
        if (isCurrentSlideFinished()) {
            selectNextSlide();
            if (noMoreSlidesAvailable()) {
                finishPlaying();
            }
        }
    }

    void CutScenePlayer::finishPlaying() {
        finished = true;
        playing = false;
        if (!cutScene->musicUri.empty()) {
            AUDIOSYS.stopMusic();
        }
        if (!cutScene->speechUri.empty()) {
            AUDIOSYS.stopSounds({"PLAYER"});
        }
    }

    void CutScenePlayer::selectNextSlide() {
        currentSlideIndex++;
        timeInCurrentSlide = 0;
    }

    bool CutScenePlayer::isCurrentSlideFinished() const { return timeInCurrentSlide > cutScene->slides[currentSlideIndex]->seconds; }

    bool CutScenePlayer::noMoreSlidesAvailable() const { return currentSlideIndex >= (int) cutScene->slides.size(); }

    void CutScenePlayer::render() {
        if (finished) return;

        Slide* slide = cutScene->slides[currentSlideIndex].get();
        computeFrameFadingAlpha(slide);

        GFXSYS.setTransparencyMode(TransparencyMode::Opaque);
        renderSlideImage(slide->texture);

        GFXSYS.setTransparencyMode(TransparencyMode::Transparent);
        for (auto caption : slide->captions) {
            renderSlideCaption(caption);
        }
    }

    void CutScenePlayer::computeFrameFadingAlpha(const Slide* slide) {
        frameFadingAlpha = 1;
        if (slide->fadeInSeconds > 0 && timeInCurrentSlide < slide->fadeInSeconds) {
            frameFadingAlpha = easeInQuintic(timeInCurrentSlide / slide->fadeInSeconds);
        } else if (slide->fadeOutSeconds > 0 && timeInCurrentSlide >= (slide->seconds - slide->fadeOutSeconds)) {
            frameFadingAlpha = easeOutQuintic((slide->seconds - timeInCurrentSlide) / slide->fadeOutSeconds);
        }
    }

    void CutScenePlayer::renderSlideImage(shared_ptr<Texture> texture) {
        // TODO: terribly inefficient. Optimize this later
        SpriteSheet spriteSheet;
        spriteSheet.texture = texture;
        spriteSheet.autoAdd(texture->getWidth(), texture->getHeight());

        Sprite sprite(&spriteSheet, 0);
        vec2 position(0, 0);
        vec4 color(frameFadingAlpha, frameFadingAlpha, frameFadingAlpha, 1);

        GFXSYS.drawSprite(sprite, position, color);
    }

    void CutScenePlayer::renderSlideCaption(shared_ptr<Caption> caption) {
        float secondsSinceCaptionStart = timeInCurrentSlide - caption->secondsFromSlideStart;
        if (secondsSinceCaptionStart >= 0) {
            bool shouldDisplayCaption = secondsSinceCaptionStart < caption->displaySeconds || caption->displaySeconds <= 0;
            if (shouldDisplayCaption) {

                float captionFadingAlpha = computeCaptionFadingAlpha(caption, secondsSinceCaptionStart);

                captionFont->print(caption->position.x,
                                   caption->position.y,
                                   caption->content,
                                   caption->color * vec4(frameFadingAlpha) * vec4(1, 1, 1, captionFadingAlpha),
                                   FontPrintStyle::SHADOW);
            }
        }
    }

    float CutScenePlayer::computeCaptionFadingAlpha(const shared_ptr<Caption>& caption, float secondsSinceCaptionStart) const {
        float captionFadingAlpha = 1;
        if (caption->fadeInSeconds > 0 && secondsSinceCaptionStart < caption->fadeInSeconds) {
            captionFadingAlpha = easeInCubic(secondsSinceCaptionStart / caption->fadeInSeconds);
        } else if (caption->fadeOutSeconds > 0) {
            float secondsSinceFadeOutStart = caption->displaySeconds - secondsSinceCaptionStart;
            if (secondsSinceFadeOutStart >= 0) {
                captionFadingAlpha = easeOutCubic(secondsSinceFadeOutStart / caption->fadeOutSeconds);
            }
        }
        return captionFadingAlpha;
    }


} // end of namespace Acidrain







