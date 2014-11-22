#pragma once

#include <GLheaders.h>
#include <stb_truetype.h>
#include <string>
#include <glm/vec4.hpp>

namespace Acidrain {

    class Font {
    public:

        Font(std::string fontFile, float fontSize = 32.0f);

        ~Font();

        void print(float x, float y, const char* text, const glm::vec4& color);

    private:
        GLuint atlasId;
        float fontSize;
        stbtt_bakedchar cdata[96];
    };
} // namespace Acidrain