#pragma once

#include <GLheaders.h>
#include <stb_truetype.h>
#include <string>

namespace Acidrain {

    class Font {
    public:

        Font(std::string fontFile, float fontSize = 32.0f);

        ~Font();

        void print(float x, float y, char *text);

    private:

        GLuint atlasId;
        stbtt_bakedchar cdata[96];
    };

} // namespace Acidrain