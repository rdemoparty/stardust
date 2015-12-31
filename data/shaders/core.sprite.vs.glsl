#version 150 core

in vec2 position;
in vec2 texCoord;
in vec4 vertexColor;

out vec2 interpolatedTexCoord;
out vec2 interpolatedPosition;
out vec4 interpolatedVertexColor;

uniform mat4 orthoMatrix;

void main() {
    gl_Position = orthoMatrix * vec4(position.x, position.y, 0.0f, 1.0f);
    interpolatedPosition = position;
    interpolatedTexCoord = vec2(texCoord.x, 1.0 - texCoord.y);
    interpolatedVertexColor = vertexColor;
}
