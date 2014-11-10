#version 130

uniform mat4 orthoMatrix;

in vec2 position;
in vec2 textCoords;
in vec4 color;

out vec4 interpolatedColor;
out vec2 interpolatedTextCoords;

void main() {
    gl_Position = orthoMatrix * vec4(position.x, position.y, 0.0f, 1.0f);
    interpolatedColor = color;
    interpolatedTextCoords = textCoords;
}