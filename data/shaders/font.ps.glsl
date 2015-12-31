#version 150

in vec2 interpolatedTexCoord;
in vec2 interpolatedPosition;
in vec4 interpolatedVertexColor;

uniform sampler2D diffuseSampler;
uniform vec4 textDiffuseColor;
uniform vec4 textOutlineColor;
// 0 = normal, 1 = outline, 2 = shadow
uniform int textPrintStyle;

out vec4 outColor;

void main() {
    vec3 texel = texture(diffuseSampler, vec2(interpolatedTexCoord.x, interpolatedTexCoord.y)).rgb;
    if (textPrintStyle == 0) {
        outColor = vec4(textDiffuseColor.rgb, texel.r * textDiffuseColor.a);
        return;
    }

    vec3 finalColor;
    if (textPrintStyle == 1) { // outline
        // color consists of the (diffuse color * main alpha) + (background color * outline alpha)
        finalColor = (textDiffuseColor.rgb * texel.r) + (textOutlineColor.rgb * texel.g);
    } else {
        // color consists of the (diffuse color * main alpha) + (background color * outline alpha)
        finalColor = (textDiffuseColor.rgb * texel.r) + (textOutlineColor.rgb * texel.b);
    }
    // make the main alpha more pronounced, makes small text sharper
    texel.r = clamp(texel.r * 2.0, 0.0, 1.0);

    // alpha is the sum of main alpha and outline alpha
    // main alpha is main font color alpha
    // outline alpha is the stroke or shadow alpha
    float mainAlpha = texel.r * textDiffuseColor.a;
    float outlineAlpha = (textPrintStyle == 1 ? texel.g : texel.b) * textOutlineColor.a * textDiffuseColor.a;
//    gl_FragColor = vec4(finalColor, clamp(mainAlpha + outlineAlpha, 0.0, 1.0));
    outColor = vec4(finalColor, clamp(max(mainAlpha, outlineAlpha), 0.0, 1.0));
}
