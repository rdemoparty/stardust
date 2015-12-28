#pragma once

#include <FontGlyphAtlas.h>
#include <FontMetrics.h>

#include <memory>
#include <Texture.h>
#include <Vbo.h>
#include <glm/vec4.hpp>

namespace Acidrain {

    using namespace std;
    using namespace glm;

    class GpuProgram;
    class GpuProgramConstantBundle;

    enum class FontPrintStyle {
        NORMAL = 0,
        OUTLINE = 1,
        SHADOW = 2
    };


    class Font {
    public:
        Font(const string& fontFile, int pointSize, FontRenderStyle renderStyle = DEFAULT_FONT_RENDER_STYLE);
        ~Font();

        void print(float x, float y, const string& text, const vec4& color, FontPrintStyle printStyle = FontPrintStyle::NORMAL);
        void print(float x, float y, const string& text, const vec4& textColor, const vec4& outlineColor, FontPrintStyle printStyle = FontPrintStyle::NORMAL);

        const FontMetrics& metrics() const;
        const GlyphAtlas& atlas() const;
        const shared_ptr<Texture> texture() const;

        bool hasChar(char characterToRender) const;
        int getKerning(char previousChar, const char& currentChar) const;
        int getCharWidth(const char& character) const;

        const vec2 getLastCharPosition();
    private:
        void buildGlyphAtlas(FontRenderStyle fontRenderStyle);
        void addCharToVbo(const char& charToRender, const vec4& color, FontPrintStyle printStyle);

        FontRenderStyle renderStyle;
        FontMetrics fontMetrics;

        shared_ptr<Texture> texture_;
        shared_ptr<GpuProgram> gpuProgram;
        shared_ptr<GpuProgramConstantBundle> gpuProgramConstantBundle;

        FT_Face face;
        char previousChar;
        Vbo vbo;
        GlyphAtlas atlas_;
        vec2 penPosition;
    };

} // namespace Acidrain