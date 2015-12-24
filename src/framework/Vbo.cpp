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
//     void Vbo::draw() {
//         if (triangles == 0)
//             return;
//
//         if (id == 0)
//             glGenBuffers(1, &id);
//
//         glBindBuffer(GL_ARRAY_BUFFER, id);
//         glBufferData(GL_ARRAY_BUFFER, sizeOfVertexInBytes() * triangles * 3, &buffer[0], GL_STATIC_DRAW);
//
//         glEnableVertexAttribArray(0); // vertices on stream 0
//         glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeOfVertexInBytes(), BUFFER_OFFSET(0));
//
//         glEnableVertexAttribArray(1); // tex coords on stream 1
//         glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeOfVertexInBytes(), BUFFER_OFFSET(2));
//
//         glEnableVertexAttribArray(2); // colors on stream 2
//         glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeOfVertexInBytes(), BUFFER_OFFSET(4));
//
//         glDrawArrays(GL_TRIANGLES, 0, triangles * 3);
//
//         glDisableVertexAttribArray(2);
//         glDisableVertexAttribArray(1);
//         glDisableVertexAttribArray(0);
//     }

    // OpenGL 2.1
    void Vbo::draw() {
        draw(triangles);
    }

    void Vbo::draw(int howManyTriangles) {
        if (howManyTriangles == 0)
            return;

        if (id == 0)
            glGenBuffers(1, &id);

        glBindBuffer(GL_ARRAY_BUFFER, id);
        glBufferData(GL_ARRAY_BUFFER, sizeOfVertexInBytes() * howManyTriangles * 3, &buffer[0], GL_STATIC_DRAW);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);

        glVertexPointer(2, GL_FLOAT, sizeOfVertexInBytes(), BUFFER_OFFSET(0));
        glTexCoordPointer(2, GL_FLOAT, sizeOfVertexInBytes(), BUFFER_OFFSET(2));
        glColorPointer(4, GL_FLOAT, sizeOfVertexInBytes(), BUFFER_OFFSET(4));

        glDrawArrays(GL_TRIANGLES, 0, howManyTriangles * 3);

        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);

    }

    void Vbo::setQuad(int quadIndex, const vector<vec2>& verts, const vector<vec2>& texs, const vec4& color) {
        setTriangle(quadIndex*2 + 0, verts, texs, color, 0, 1, 2);
        setTriangle(quadIndex*2 + 1, verts, texs, color, 2, 3, 0);
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
