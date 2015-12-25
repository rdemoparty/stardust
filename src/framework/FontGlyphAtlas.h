#pragma once

#include <string>
#include <map>

#include <FontRenderStyle.h>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Acidrain {

    using namespace std;

    struct GlyphInfo {
        int x;
        int y;
        int w;
        int h;
        int advance;
        int bitmapLeft;
        int bitmapTop;
    };

    class GlyphAtlas {
    public:
        explicit GlyphAtlas();
        virtual ~GlyphAtlas();

        void add(unsigned short int asciiChar, FT_GlyphSlot glyphSlot, FT_Bitmap outlineBitmap, FontRenderStyle fontRenderStyle);
        void dump(const string& filename) const;

        int padding() const;
        int width() const;
        int height() const;
        const unsigned char* texture() const;

        bool containsGlyphFor(char characterToRender) const;
        const GlyphInfo& glyph(char character) const;

    private:
        void enlargeAtlas();
        bool hasRoomFor(int width, int height);

        int paddingSize = 5;
        int atlasWidth = 16;
        int atlasHeight = 16;
        int currentX = 0;
        int currentY = 0;
        int maxHeightOnLastRow = 0;
        unsigned char* atlasTexture;
        map <unsigned short int, GlyphInfo> glyphs;
    };

} // namespace Acidrain