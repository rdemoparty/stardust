#pragma once

#include <GLheaders.h>
#include <stb_truetype.h>
#include <string>
#include <glm/vec4.hpp>

namespace Acidrain {

    using namespace std;
    using namespace glm;

    class Font {
    public:
        Font(string fontFile, float fontSize = 32.0f);

        ~Font();

        void print(float x, float y, const char* text, const vec4& color = vec4(1, 1, 1, 1));

        void print(float x, float y, string text, const vec4& color = vec4(1, 1, 1, 1));

    private:
        GLuint atlasId;
        float fontSize;
        stbtt_bakedchar characterData[96];
        float HEIGHT_MAGIC_OFFSET;
    };
} // namespace Acidrain