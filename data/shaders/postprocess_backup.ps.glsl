#version 120

uniform sampler2D diffuseSampler;

void main() {
	gl_FragColor = vec4(0, 1, 0, 1);
	gl_FragColor = texture2D(diffuseSampler, gl_TexCoord[0].st);
}
