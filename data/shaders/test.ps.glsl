#version 120

uniform sampler2D diffuseSampler;
uniform vec4 textDiffuseColor;
uniform vec4 textOutlineColor;
uniform vec4 textShadowColor;

void main() {
//
//    gl_FragColor = vec4(1, 1, 1, 1);
//    return;
//
    vec3 tex = texture2D(diffuseSampler, vec2(gl_TexCoord[0].s, gl_TexCoord[0].t)).rgb;

    // color consists of the (diffuse color * main alpha) + (background color * outline alpha)
    vec3 finalColor = (textDiffuseColor.rgb * tex.r) + (textOutlineColor.rgb * tex.g);

    // make the main alpha more pronounced, makes small text sharper
//    tex.r = clamp(tex.r * 2.0, 0.0, 1.0);

    // alpha is the sum of main alpha and outline alpha
    // main alpha is main font color alpha
    // outline alpha is the stroke or shadow alpha
    float mainAlpha = tex.r * textDiffuseColor.a;
    float outlineAlpha = tex.g * textOutlineColor.a * textDiffuseColor.a;
    gl_FragColor = vec4(finalColor, mainAlpha + outlineAlpha);
}
