#version 120

uniform sampler2D diffuseSampler;
uniform vec4 textDiffuseColor;
uniform vec4 textOutlineColor;
// 0 = normal, 1 = outline, 2 = shadow
uniform int textPrintStyle;

void main() {
    if (textPrintStyle == 0) {
        vec3 tex = texture2D(diffuseSampler, vec2(gl_TexCoord[0].s, gl_TexCoord[0].t)).rgb;
        gl_FragColor = vec4(textDiffuseColor.rgb, tex.r * textDiffuseColor.a);
        return;
    }

    vec3 tex = texture2D(diffuseSampler, vec2(gl_TexCoord[0].s, gl_TexCoord[0].t)).rgb;
    vec3 finalColor;

    if (textPrintStyle == 1) { // outline
        // color consists of the (diffuse color * main alpha) + (background color * outline alpha)
        finalColor = (textDiffuseColor.rgb * tex.r) + (textOutlineColor.rgb * tex.g);
    } else {
        // color consists of the (diffuse color * main alpha) + (background color * outline alpha)
        finalColor = (textDiffuseColor.rgb * tex.r) + (textOutlineColor.rgb * tex.b);
    }
    // make the main alpha more pronounced, makes small text sharper
    tex.r = clamp(tex.r * 2.0, 0.0, 1.0);


    // alpha is the sum of main alpha and outline alpha
    // main alpha is main font color alpha
    // outline alpha is the stroke or shadow alpha
    float mainAlpha = tex.r * textDiffuseColor.a;
//    float outlineAlpha = (textPrintStyle == 1 ? tex.g : tex.b) * textOutlineColor.a * textDiffuseColor.a;
    float outlineAlpha = (textPrintStyle == 1 ? tex.g : tex.b) * textOutlineColor.a * textDiffuseColor.a;
//    gl_FragColor = vec4(finalColor, clamp(mainAlpha + outlineAlpha, 0.0, 1.0));
    gl_FragColor = vec4(finalColor, clamp(max(mainAlpha, outlineAlpha), 0.0, 1.0));
}
