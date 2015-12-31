#include <Vbo.h>

namespace Acidrain {

    const char* BUFFER_OFFSET(int i) {
        return (char*) NULL + i * sizeof(float);
    }

    int componentsInVertex() {
        return 2 + 2 + 4; // position, uv, color
    }

    int sizeOfVertexInBytes() {
        return (int) (componentsInVertex() * sizeof(float));
    }

    Vbo::Vbo() {
    }

    Vbo::~Vbo() {
        if (initialized) {
            glDeleteVertexArrays(1, &vaoId);
            glDeleteBuffers(1, &id);
        }
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

    // OpenGL >= 3.0
    void Vbo::draw(int howManyTriangles) const {
        if (howManyTriangles == 0)
            return;
        if (triangles == 0)
            return;

        if (!initialized) {
            glGenVertexArrays(1, &vaoId);
            glGenBuffers(1, &id);
        }

        glBindVertexArray(vaoId);
        glBindBuffer(GL_ARRAY_BUFFER, id);

        glBufferData(GL_ARRAY_BUFFER, sizeOfVertexInBytes() * triangles * 3, &buffer[0], GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0); // vertices on stream 0
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeOfVertexInBytes(), BUFFER_OFFSET(0));

        glEnableVertexAttribArray(1); // tex coordinates on stream 1
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeOfVertexInBytes(), BUFFER_OFFSET(2));

        glEnableVertexAttribArray(2); // colors on stream 2
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeOfVertexInBytes(), BUFFER_OFFSET(4));

        glDrawArrays(GL_TRIANGLES, 0, howManyTriangles * 3);

        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void Vbo::draw() const {
        draw(triangles);
    }

    void Vbo::setQuad(int quadIndex, const vector<vec2>& verts, const vector<vec2>& texs, const vec4& color) {
        setTriangle(quadIndex * 2 + 0, verts, texs, color, 0, 1, 2);
        setTriangle(quadIndex * 2 + 1, verts, texs, color, 2, 3, 0);
    }

    void Vbo::setTriangle(int triangleIndex, const vector<vec2>& verts, const vector<vec2>& texs, const vec4& color, int a, int b, int c) {
        buffer.at(triangleIndex * 12 + 0) = verts[a];
        buffer.at(triangleIndex * 12 + 1) = texs[a];

        buffer.at(triangleIndex * 12 + 4) = verts[b];
        buffer.at(triangleIndex * 12 + 5) = texs[b];

        buffer.at(triangleIndex * 12 + 8) = verts[c];
        buffer.at(triangleIndex * 12 + 9) = texs[c];
    }
} // namespace Acidrain
