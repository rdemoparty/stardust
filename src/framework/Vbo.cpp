#include <Vbo.h>

namespace Acidrain {

    constexpr char* BUFFER_OFFSET(int i) {
        return (char*) NULL + i * sizeof(float);
    }

    constexpr int componentsInVertex() {
        return 2 + 2 + 4; // position, uv, color
    }

    constexpr int sizeOfVertexInBytes() {
        return componentsInVertex() * sizeof(float);
    }

    Vbo::Vbo() {
    }

    Vbo::~Vbo() {
    }

    void Vbo::empty() {
        buffer.clear();
        triangles = 0;
    }

    void Vbo::addQuad(const vector<vec2>& verts, const vector<vec2>& texs, const vec4& color) {
        addTriangle(verts, texs, color, 0, 1, 2);
        addTriangle(verts, texs, color, 2, 3, 0);
    }

    void Vbo::addTriangle(const vector<vec2>& verts, const vector<vec2>& texs, const vec4& color, int a, int b, int c) {
        buffer.push_back(verts[a]);
        buffer.push_back(texs[a]);
        buffer.push_back(color.rg());
        buffer.push_back(color.ba());

        buffer.push_back(verts[b]);
        buffer.push_back(texs[b]);
        buffer.push_back(color.rg());
        buffer.push_back(color.ba());

        buffer.push_back(verts[c]);
        buffer.push_back(texs[c]);
        buffer.push_back(color.rg());
        buffer.push_back(color.ba());

        triangles++;
    }

    void Vbo::draw() {
        if (id == 0)
            glGenBuffers(1, &id);

        glBindBuffer(GL_ARRAY_BUFFER, id);
        glBufferData(GL_ARRAY_BUFFER, sizeOfVertexInBytes() * triangles * 3, &buffer[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0); // vertices on stream 0
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeOfVertexInBytes(), BUFFER_OFFSET(0));

        glEnableVertexAttribArray(1); // tex coords on stream 1
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeOfVertexInBytes(), BUFFER_OFFSET(2));

        glEnableVertexAttribArray(2); // colors on stream 2
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeOfVertexInBytes(), BUFFER_OFFSET(4));

        glDrawArrays(GL_TRIANGLES, 0, triangles * 3);

        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
    }
} // namespace Acidrain
