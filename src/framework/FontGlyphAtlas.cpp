#include <FontGlyphAtlas.h>
#include <easylogging++.h>
#include <SDL_rect.h>

namespace Acidrain {

    GlyphAtlas::GlyphAtlas() {
        atlasTexture = new unsigned char[atlasWidth * atlasHeight * 4];
        memset(atlasTexture, 0, (size_t) (atlasWidth * atlasHeight * 4));
    }

    GlyphAtlas::~GlyphAtlas() {
        if (atlasTexture != nullptr) {
            delete[] atlasTexture;
            atlasTexture = nullptr;
        }
    }

    void GlyphAtlas::add(unsigned short int asciiChar, FT_GlyphSlot glyphSlot, FT_Bitmap outlineBitmap, FontRenderStyle fontRenderStyle) {

        // establish glyph location in atlas
        SDL_Rect srcRect;
        srcRect.x = 0;
        srcRect.y = 0;
        srcRect.w = outlineBitmap.width;
        srcRect.h = outlineBitmap.rows;

        while (!hasRoomFor(outlineBitmap.width + paddingSize * 2, outlineBitmap.rows + paddingSize * 2)) {
            enlargeAtlas();
        }

        SDL_Rect dstRect;
        if ((currentX + (int)outlineBitmap.width + paddingSize * 2) >= atlasWidth) {
            currentX = 0;
            currentY += maxHeightOnLastRow;
            maxHeightOnLastRow = 0;
        }
        maxHeightOnLastRow = std::max(maxHeightOnLastRow, (int) (outlineBitmap.rows + paddingSize * 2));

        dstRect.x = currentX;
        dstRect.y = currentY;
        dstRect.w = outlineBitmap.width;
        dstRect.h = outlineBitmap.rows;

        glyphs[asciiChar] = GlyphInfo{
                dstRect.x,
                dstRect.y,
                dstRect.w + paddingSize * 2,
                dstRect.h + paddingSize * 2,
                (int) (glyphSlot->advance.x / 64.0),
                glyphSlot->bitmap_left,
                glyphSlot->bitmap_top
        };

        // render outlined glyph
        for (unsigned int y = 0; y < outlineBitmap.rows; y++) {
            int sourceOffset = y * outlineBitmap.pitch;
            int destinationOffset = (dstRect.y + y + paddingSize) * atlasWidth + dstRect.x + paddingSize;
            unsigned char* sourcePointer = &outlineBitmap.buffer[sourceOffset];
            for (unsigned int x = 0; x < outlineBitmap.width; x++) {
                unsigned char alphaComponent = *sourcePointer++;
                atlasTexture[(destinationOffset * 4 + 1)] = alphaComponent;
                atlasTexture[(destinationOffset * 4 + 3)] = 255;
                destinationOffset++;
            }
        }

        // copy rendered glyph
        int outlineSize = (int)(fontRenderStyle.outlineSize * TEXT_DOWNSAMPLE_FACTOR);
        for (unsigned int y = 0; y < glyphSlot->bitmap.rows; y++) {
            int sourceOffset = y * glyphSlot->bitmap.pitch;
            int destinationOffset = (dstRect.y + y + outlineSize + paddingSize) * atlasWidth + dstRect.x + outlineSize + paddingSize;
            unsigned char* sourcePointer = &glyphSlot->bitmap.buffer[sourceOffset];
            for (unsigned int x = 0; x < glyphSlot->bitmap.width; x++) {
                unsigned char alphaComponent = *sourcePointer++;
                atlasTexture[(destinationOffset * 4 + 0)] = alphaComponent;
                atlasTexture[(destinationOffset * 4 + 3)] = 255;
                destinationOffset++;
            }
        }

        // copy shadow glyph
        int shadowOffsetX = fontRenderStyle.shadowOffsetX * TEXT_DOWNSAMPLE_FACTOR;
        int shadowOffsetY = fontRenderStyle.shadowOffsetY * TEXT_DOWNSAMPLE_FACTOR;
        for (unsigned int y = 0; y < glyphSlot->bitmap.rows; y++) {
            int dstY = dstRect.y + y + outlineSize + shadowOffsetY + paddingSize;
//                if (y >= (outlineBitmap.rows + paddingSize)) continue;

            int destinationOffset = dstY * atlasWidth + dstRect.x + outlineSize + shadowOffsetX + paddingSize;

            int sourceOffset = y * glyphSlot->bitmap.pitch;
            unsigned char* sourcePointer = &glyphSlot->bitmap.buffer[sourceOffset];
            for (unsigned int x = 0; x < glyphSlot->bitmap.width; x++) {
                unsigned char alphaComponent = *sourcePointer++;
                atlasTexture[(destinationOffset * 4 + 2)] = alphaComponent;
                atlasTexture[(destinationOffset * 4 + 3)] = 255;
                destinationOffset++;
            }
        }

        currentX += srcRect.w + paddingSize * 2;
    }

    void GlyphAtlas::enlargeAtlas() {
        int newAtlasWidth = atlasWidth;
        int newAtlasHeight = atlasHeight;

        if (newAtlasWidth > 512) {
            newAtlasHeight += 100;
        } else {
            newAtlasWidth *= 2;
        }

        unsigned char* newAtlasTexture = new unsigned char[newAtlasWidth * newAtlasHeight * 4];
        memset(newAtlasTexture, 0, (size_t) (newAtlasWidth * newAtlasHeight * 4));

        // relocate glyphs
        currentX = currentY = maxHeightOnLastRow = 0;

        for (auto& kv : glyphs) {
            int newX = currentX;
            int newY = currentY;

            bool noMoreRoomOnRow = (newX + kv.second.w) >= newAtlasWidth;
            if (noMoreRoomOnRow) {
                newX = 0;
                newY += maxHeightOnLastRow;
            }

            for (int y = 0; y < kv.second.h; y++) {
                unsigned char* sourcePointer = &atlasTexture[((y + kv.second.y) * atlasWidth + kv.second.x) * 4];
                unsigned char* destinationPointer = &newAtlasTexture[((y + newY) * newAtlasWidth + newX) * 4];
                memcpy(destinationPointer, sourcePointer, (size_t) (kv.second.w * 4));
            }
            maxHeightOnLastRow = std::max(maxHeightOnLastRow, kv.second.h);

            kv.second.x = newX;
            kv.second.y = newY;

            // advance
            currentX = newX + kv.second.w;
            currentY = newY;
        }

        // remove old data and replace it with the new one
        delete[] atlasTexture;

        atlasTexture = newAtlasTexture;
        atlasWidth = newAtlasWidth;
        atlasHeight = newAtlasHeight;
    }

    bool GlyphAtlas::hasRoomFor(int width, int height) {
        int y = (currentX + width) >= atlasWidth ?
                currentY + maxHeightOnLastRow :
                currentY;
        return (y + height) < atlasHeight;
    }

    int GlyphAtlas::padding() const {
        return paddingSize;
    }

    int GlyphAtlas::width() const {
        return atlasWidth;
    }

    int GlyphAtlas::height() const {
        return atlasHeight;
    }

    bool GlyphAtlas::containsGlyphFor(char characterToRender) const {
        return glyphs.lower_bound(static_cast<unsigned short int>(characterToRender)) != glyphs.end();
    }

    void GlyphAtlas::dump(const string& filename) const {
        LOG(INFO) << "Font atlas size is " << atlasWidth << "x" << atlasHeight;
        ofstream file(filename);
        file.write((const char*) atlasTexture, atlasWidth * atlasHeight * 4);
        file.close();
    }

    const GlyphInfo& GlyphAtlas::glyph(char character) const {
        return glyphs.at(static_cast<unsigned short int>(character));
    }

    const unsigned char* GlyphAtlas::texture() const {
        return atlasTexture;
    }
};