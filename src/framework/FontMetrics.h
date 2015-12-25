#pragma once

#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace Acidrain {
    using namespace std;

    struct FontMetrics {
        void setFrom(FT_Face face);

        int height;
        int lineSkip;
        int ascent;
        int descent;
        string family;
    };
}