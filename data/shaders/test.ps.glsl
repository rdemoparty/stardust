#version 120

uniform sampler2D diffuseSampler;
varying vec4 color;

void main() {
//	gl_FragColor = vec4(1, 0, 0, 1);
    vec4 texel = texture2D(diffuseSampler, vec2(gl_TexCoord[0].s, gl_TexCoord[0].t));
	gl_FragColor = vec4(color.rgb, color.a * texel.a);
//	gl_FragColor = vec4(1, 0, 0, 1);
}
