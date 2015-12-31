#version 150 core

in vec2 interpolatedTexCoord;
in vec2 interpolatedPosition;
in vec4 interpolatedVertexColor;

out vec4 colorOut;

void main(void) {
    colorOut = interpolatedVertexColor;
}
