#version 120

uniform sampler2D diffuseSampler;
varying vec4 color;

void main() {

    vec4 outlineColor = vec4(0, 0, 0, 0.3);

//	gl_FragColor = vec4(1, 0, 0, 1);
    vec3 tex = texture2D(diffuseSampler, vec2(gl_TexCoord[0].s, gl_TexCoord[0].t)).rgb;

    // color consists of the (diffuse color * main alpha) + (background color * outline alpha)
    vec3 finalColor = (color.rgb * tex.r) + (outlineColor.rgb * tex.b);

    // make the main alpha more pronounced, makes small text sharper
    tex.r = clamp(tex.r * 2.0, 0.0, 1.0);

    // alpha is the sum of main alpha and outline alpha
    // main alpha is main font color alpha
    // outline alpha is the stroke or shadow alpha
    float mainAlpha = tex.r * color.a;
    float outlineAlpha = tex.b * outlineColor.a * color.a;
    gl_FragColor = vec4(finalColor, mainAlpha + outlineAlpha);
//	gl_FragColor = vec4(color.rgb, color.a * texel.a);
//	gl_FragColor = texel;
//	gl_FragColor = vec4(1, 0, 0, 1);
}
