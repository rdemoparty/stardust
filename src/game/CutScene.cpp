#include <CutScene.h>
#include <GfxSystem.h>
#include <Font.h>

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

            if (currentSlideIndex >= cutScene->slides.size()) {
                finished = true;
                playing = false;
            }
        }
    }

    void CutScenePlayer::render() {
        if (finished) return;

        Slide* slide = cutScene->slides[currentSlideIndex].get();

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
        vec4 color(1, 1, 1, 1);
        GFXSYS.drawSprite(sprite, position, color);
    }

    void CutScenePlayer::renderSlideCaption(shared_ptr<Caption> caption) {
        captionFont->print(caption->position.x,
                           caption->position.y,
                           caption->content,
                           caption->color,
                           FontPrintStyle::SHADOW);
    }

} // end of namespace Acidrain




