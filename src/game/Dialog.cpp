#include <Dialog.h>
#include <AudioSystem.h>
#include <Font.h>
#include <math.h>
#include <GfxSystem.h>

namespace Acidrain {

    class Dialog::impl {
    public:
        impl(string message, float x, float y, Font* font, DialogOptions options);

        void update(float dt);
        void render();

        bool isFinished();

        string message;
        float x;
        float y;
        Font* font;
        DialogOptions options;

        bool finished = false;
        bool timeToBlink = false;
        float realOffset = 0;
        float delayTimer = 0;
        float elapsedTime = 0;
        void drawText(string basic_string);
    };

    Dialog::Dialog(string message, float x, float y, Font* font, DialogOptions options)
            : pimpl(new impl(message, x, y, font, options)) {
    }

    Dialog::~Dialog() {
    }

    void Dialog::update(float dt) {
        pimpl->update(dt);
    }

    void Dialog::render() {
        pimpl->render();
    }

    bool Dialog::isFinished() {
        return pimpl->isFinished();
    }

    // ---------------------------------------------------------------------

    Dialog::impl::impl(string message, float x, float y, Font* font, DialogOptions options) {
        this->message = message;
        this->x = x;
        this->y = y;
        this->font = font;
        this->options = options;
    }

    bool Dialog::impl::isFinished() {
        return finished;
    }

    void Dialog::impl::update(float dt) {
//            x = x - 20*dt; // scrolling
        if (delayTimer >= 0) {
            delayTimer -= dt;
        } else {
            if (!finished && delayTimer <= 0) {
                float oldOffset = realOffset;
                realOffset += dt * options.charactersTypedPerSecond;
                if (realOffset >= message.size()) {
                    finished = true;
                } else {
                    unsigned long oldCharOffset = (unsigned long) oldOffset;
                    unsigned long currentCharOffset = (unsigned long) realOffset;
                    bool charJustChanged = currentCharOffset > oldCharOffset;
                    if (charJustChanged) {
                        char currentChar = message.at(currentCharOffset);
                        if (currentChar == '~') {
                            delayTimer += 1;
                        } else if (currentChar != '\n') {
                            if (!options.charSound.empty()) {
                                AUDIOSYS.playSound(options.charSound, "TYPER", 128);
                            }
                        } else {
                            if (options.scrollOldLinesUp) {
                                y -= this->font->metrics().height;
                            }
                        }
                        timeToBlink = true;
                    }
                }
            }
        }

        elapsedTime += dt;
        if (options.blinkingCursor) {
            double period = 1.0 / (2.0 * options.blinksPerSecond);
            double halfPeriod = period / 2.0;
            timeToBlink = fmod(elapsedTime, period) <= halfPeriod;
        } else if (options.showCursor) {
            timeToBlink = true;
        } else {
            timeToBlink = false;
        };
    }

    void Dialog::impl::render() {
        unsigned long charsToDraw = (unsigned long) realOffset;
        string newMessage = message.substr(0, charsToDraw);
        if (options.showCursor) {
            if (timeToBlink)
                newMessage = newMessage + options.cursor;
            else
                newMessage = newMessage + ' ';
        }
        drawText(newMessage);
    }

    void Dialog::impl::drawText(string message) {
        GFXSYS.setTransparencyMode(TransparencyMode::Transparent);
        if (message.size() > 2) {
            font->print(x, y, message.substr(0, message.size() - 2), options.charColor, vec4(0, 0, 0, 0.5), FontPrintStyle::OUTLINE);
        }

        if (message.size() > 1) {
            const vec2 lastPos = font->getLastCharPosition();
            font->print(lastPos.x, lastPos.y, message.substr(message.size() - 2, 1), options.activeCharColor, vec4(0, 0, 0, 0.5), FontPrintStyle::OUTLINE);
        }

        if (message.size() > 1) {
            const vec2 lastPos = font->getLastCharPosition();
            font->print(lastPos.x, lastPos.y, message.substr(message.size() - 1, 1), options.activeCharColor, vec4(0, 0, 0, 0.5), FontPrintStyle::OUTLINE);
        }
    }
}

