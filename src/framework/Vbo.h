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
        void setQuad(int quadIndex, const vector<vec2>& verts, const vector<vec2>& texs, const vec4& color);

        void addTriangle(const vector<vec2>& verts, const vector<vec2>& texs, const vec4& color, int a, int b, int c);
        void setTriangle(int triangleIndex, const vector<vec2>& verts, const vector<vec2>& texs, const vec4& color, int a, int b, int c);

        void draw() const;
        void draw(int howManyTriangles) const;

    private:
        vector<vec2> buffer;
        int triangles = 0;
        mutable GLuint id = 0;
        mutable GLuint vaoId = 0;
        mutable bool initialized = false;
    };
} // namespace Acidrain
