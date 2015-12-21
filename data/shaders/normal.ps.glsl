#version 120

uniform sampler2D diffuseSampler;
varying vec4 color;

void main() {
	gl_FragColor = texture2D(diffuseSampler, gl_TexCoord[0].st) * color;
}
