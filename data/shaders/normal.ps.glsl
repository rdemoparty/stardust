#version 150

in vec2 interpolatedTexCoord;
in vec4 interpolatedVertexColor;

uniform sampler2D diffuseSampler;

out vec4 colorOut;

void main() {
	colorOut = texture(diffuseSampler, interpolatedTexCoord) * interpolatedVertexColor;
}
