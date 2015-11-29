#include <Font.h>

#define STB_TRUETYPE_IMPLEMENTATION

#include <stb_truetype.h>
#include <FileSystem.h>

#include <memory>
#include <iostream>

namespace Acidrain {

    static const int TEXTURE_SIZE = 512;

    Font::Font(const char* const fontFile, float fontSize) {
        HEIGHT_MAGIC_OFFSET = fontSize * 3.0f / 4.0f;

        string fontContent = FILESYS.getFileContent(fontFile);
        unsigned char* bitmap = new unsigned char[TEXTURE_SIZE * TEXTURE_SIZE];

        this->fontSize = fontSize;

        stbtt_BakeFontBitmap((unsigned char const*) fontContent.c_str(), 0,
                             fontSize,
                             bitmap,
                             TEXTURE_SIZE, TEXTURE_SIZE,
                             32, 96,
                             characterData);

        glGenTextures(1, &atlasId);
        glBindTexture(GL_TEXTURE_2D, atlasId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, TEXTURE_SIZE, TEXTURE_SIZE, 0, GL_ALPHA, GL_UNSIGNED_BYTE, bitmap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenerateMipmap(GL_TEXTURE_2D);

        delete[] bitmap;
    }

    Font::Font(string fontFile, float fontSize)
    : Font(fontFile.c_str(), fontSize)
    {
    }

    Font::~Font() {
        glDeleteTextures(1, &atlasId);
    }

    void Font::print(float x, float y, const char* text, const glm::vec4& color) {
        x = round(x);
        y = round(y);

        float originalX = x;

        // assume orthographic projection with units = screen pixels, origin at top left
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, atlasId);
        glColor4f(color.r, color.g, color.b, color.a);
        glBegin(GL_QUADS);
        while (*text) {
            unsigned char charToPrint = static_cast<unsigned char>(*text);
            if (charToPrint == '\n') {
                x = originalX;
                y += fontSize;
            } else if (charToPrint == '~') {
                // do not print pause characters
            } else {
                if (charToPrint >= 32 && charToPrint < 128) {
                    stbtt_aligned_quad q;
                    stbtt_GetBakedQuad(characterData, TEXTURE_SIZE, TEXTURE_SIZE, *text - 32, &x, &y, &q, 1);//1=opengl,0=old d3d

                    const float scale = 1.f;

                    glTexCoord2f(q.s0, q.t0);
                    glVertex2f(q.x0 * scale, (q.y0 + HEIGHT_MAGIC_OFFSET) * scale);

                    glTexCoord2f(q.s1, q.t0);
                    glVertex2f(q.x1 * scale, (q.y0 + HEIGHT_MAGIC_OFFSET) * scale);

                    glTexCoord2f(q.s1, q.t1);
                    glVertex2f(q.x1 * scale, (q.y1 + HEIGHT_MAGIC_OFFSET) * scale);

                    glTexCoord2f(q.s0, q.t1);
                    glVertex2f(q.x0 * scale, (q.y1 + HEIGHT_MAGIC_OFFSET) * scale);
                }
            }
            ++text;
        }
        glEnd();
        glDisable(GL_TEXTURE_2D);
        lastCharPosition.x = x;
        lastCharPosition.y = y;
    }


    void Font::print(float x, float y, string text, const vec4& color) {
        print(x, y, text.c_str(), color);
    }
}
