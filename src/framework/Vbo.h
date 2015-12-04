#pragma once

#define GLM_SWIZZLE

#include <glm/vec2.hpp>
#include <vector>
#include <GLheaders.h>
#include <glm/vec4.hpp>

namespace Acidrain {

    using namespace glm;
    using namespace std;

    class Vbo {
    public:
        Vbo();

        ~Vbo();

        void empty();

        void addQuad(const vector<vec2>& verts, const vector<vec2>& texs, const vec4& color);

        void addTriangle(const vector<vec2>& verts, const vector<vec2>& texs, const vec4& color, int a, int b, int c);

        void draw();

    private:
        vector<vec2> buffer;
        int triangles = 0;
        GLuint id = 0;
    };
} // namespace Acidrain
