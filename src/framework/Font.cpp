#include <Font.h>

#define STB_TRUETYPE_IMPLEMENTATION

#include <stb_truetype.h>
#include <FileSystem.h>

#include <memory>

namespace Acidrain {
    Font::Font(std::string fontFile, float fontSize) {
        std::string fontContent = FILESYS.getFileContent(fontFile);
        unsigned char* bitmap = new unsigned char[512 * 512];

        this->fontSize = fontSize;

        stbtt_BakeFontBitmap((unsigned char const*) fontContent.c_str(), 0, fontSize, bitmap, 512, 512, 32, 96, cdata);

        glGenTextures(1, &atlasId);
        glBindTexture(GL_TEXTURE_2D, atlasId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, bitmap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        delete[] bitmap;
    }

    Font::~Font() {
        glDeleteTextures(1, &atlasId);
    }

    void Font::print(float x, float y, const char* text, const glm::vec4& color) {

        const float HEIGHT_MAGIC_OFFSET = fontSize * 3.0f / 4.0f;

        // assume orthographic projection with units = screen pixels, origin at top left
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, atlasId);
        glColor4f(color.r, color.g, color.b, color.a);
        glBegin(GL_QUADS);
        while (*text) {
            if (static_cast<unsigned char>(*text) >= 32 && static_cast<unsigned char>(*text) < 128) {
                stbtt_aligned_quad q;
                stbtt_GetBakedQuad(cdata, 512, 512, *text - 32, &x, &y, &q, 1);//1=opengl,0=old d3d

                glTexCoord2f(q.s0, q.t0);
                glVertex2f(q.x0, q.y0 + HEIGHT_MAGIC_OFFSET);

                glTexCoord2f(q.s1, q.t0);
                glVertex2f(q.x1, q.y0 + HEIGHT_MAGIC_OFFSET);

                glTexCoord2f(q.s1, q.t1);
                glVertex2f(q.x1, q.y1 + HEIGHT_MAGIC_OFFSET);

                glTexCoord2f(q.s0, q.t1);
                glVertex2f(q.x0, q.y1 + HEIGHT_MAGIC_OFFSET);
            }
            ++text;
        }
        glEnd();
    }
}
