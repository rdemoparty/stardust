#include <FontMetrics.h>
#include <easylogging++.h>

namespace Acidrain {

#define FT_CEIL(X)  (((X + 63) & -64) / 64)

    void FontMetrics::setFrom(FT_Face face) {
        if (FT_IS_SCALABLE(face)) {
            FT_Fixed scale = face->size->metrics.y_scale;
            ascent = FT_CEIL(FT_MulFix(face->ascender, scale));
            descent = FT_CEIL(FT_MulFix(face->descender, scale));
            height = ascent - descent + /* baseline */ 1;
            lineSkip = FT_CEIL(FT_MulFix(face->height, scale));
            family = string(face->family_name);
        } else {
            LOG(ERROR) << "Non scalable font, cannot get font metrics";
        }
    }

}