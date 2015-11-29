#pragma once

#include <string>
#include <memory>
#include <glm/vec4.hpp>

namespace Acidrain {

    using namespace std;
    using namespace glm;
    class Font;

    struct DialogOptions {
        string charSound;
        char cursor;
        bool showCursor;
        bool blinkingCursor;
        bool scrollOldLinesUp;
        float charactersTypedPerSecond;
        float blinksPerSecond;
        vec4 charColor;
        vec4 activeCharColor;
    };

    static DialogOptions DEFAULT_DIALOG_OPTIONS = {
            "sounds/type.ogg",
            '_',
            true,
            true,
            false,
            20,
            2,
            vec4(0, 0.8, 0, 0.7),
            vec4(0, 1, 0, 0.7)
    };

    class Dialog {
    public:
        Dialog(string message, float x, float y, Font* font, DialogOptions options = DEFAULT_DIALOG_OPTIONS);
        virtual ~Dialog();

        void update(float dt);
        void render();
        bool isFinished();

    private:
        class impl;
        unique_ptr<impl> pimpl;
    };
}
