#pragma once

#include <Font.h>
#include <Box.h>

namespace Acidrain {

    class GpuProgram;
    class GpuProgramConstantBundle;

    string wrapText(const string& text, int lineMaxWidth, Font* font);

    class TextLayout {
    public:
        TextLayout(const string& textToRender, Box constraint, Font* fnt);
        ~TextLayout();

        const Box& getTextBox() const;

        void render() const;
        void render(int howManyChars) const;

        void render(vec4 color, vec4 outlineColor);
    private:
        void buildVbo(const string& text);
        void addCharToVbo(const char& charToRender);

        shared_ptr<GpuProgram> gpuProgram;
        shared_ptr<GpuProgramConstantBundle> gpuProgramConstantBundle;

        Font* font;
        Box textBox;
        vec2 penPosition;
        char previousChar;
        Vbo vbo;
    };
}