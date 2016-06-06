#include <CutScene.h>
#include <GfxSystem.h>
#include <Font.h>
#include <MathSupport.h>
#include <easylogging++.h>
#include <glm/ext.hpp> // for glm::to_string

namespace Acidrain {


    Slide::Slide(shared_ptr<Texture> texture, float seconds) {
        this->texture = texture;
        this->seconds = seconds;

    }

    void Slide::addCaption(string content, vec2 position, vec4 color) {
        shared_ptr<Caption> caption = shared_ptr<Caption>(new Caption);
        caption->content = content;
        caption->position = position;
        caption->color = color;
        this->captions.push_back(caption);
    }

    CutScenePlayer::CutScenePlayer(shared_ptr<CutScene> cs)
            : cutScene(cs),
              timeInCurrentSlide(0),
              currentSlideIndex(0),
              finished(false),
              playing(false) {
        captionFont = shared_ptr<Font>(new Font("fonts/arial.ttf", 32));
    }

    void CutScenePlayer::start() {
        playing = true;
    }

    void CutScenePlayer::update(float dt) {
        if (finished || !playing) return;

        timeInCurrentSlide += dt;
        if (timeInCurrentSlide > cutScene->slides[currentSlideIndex]->seconds) {
            currentSlideIndex++;
            timeInCurrentSlide = 0;

            if (currentSlideIndex >= (int)cutScene->slides.size()) {
                finished = true;
                playing = false;
            }
        }
    }

    void CutScenePlayer::render() {
        if (finished) return;

        Slide* slide = cutScene->slides[currentSlideIndex].get();

        frameFadingAlpha = 1;
        if (slide->fadeInSeconds > 0 && timeInCurrentSlide < slide->fadeInSeconds) {
            frameFadingAlpha = easeInQuintic(timeInCurrentSlide / slide->fadeInSeconds);
        } else if (slide->fadeOutSeconds > 0 && timeInCurrentSlide >= (slide->seconds - slide->fadeOutSeconds)) {
            frameFadingAlpha = easeOutQuintic((slide->seconds - timeInCurrentSlide) / slide->fadeOutSeconds);
        }

        GFXSYS.setTransparencyMode(TransparencyMode::Opaque);
        renderSlideImage(slide->texture);

        GFXSYS.setTransparencyMode(TransparencyMode::Transparent);
        for (auto caption : slide->captions) {
            renderSlideCaption(caption);
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
        captionFont->print(caption->position.x,
                           caption->position.y,
                           caption->content,
                           caption->color * vec4(frameFadingAlpha),
                           FontPrintStyle::SHADOW);
    }

} // end of namespace Acidrain




