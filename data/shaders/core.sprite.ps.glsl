#version 150 core

in vec2 interpolatedTexCoord;
in vec2 interpolatedPosition;
in vec4 interpolatedVertexColor;

uniform sampler2D diffuseSampler;
uniform vec4 color;

out vec4 colorOut;

void main(void) {
    colorOut = color * interpolatedVertexColor * texture(diffuseSampler, interpolatedTexCoord);
}
