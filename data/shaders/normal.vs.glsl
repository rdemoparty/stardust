#version 120

uniform mat4 orthoMatrix;
uniform mat4 cameraShakeMatrix;
varying vec4 color;

void main() {
    gl_Position =  orthoMatrix * cameraShakeMatrix * vec4(gl_Vertex.x, gl_Vertex.y, 0.0f, 1.0f);
    gl_TexCoord[0] = gl_MultiTexCoord0;
    color = gl_Color;
}
