#version 120

uniform mat4 orthoMatrix;

void main() {
    gl_Position = orthoMatrix * vec4(gl_Vertex.x, gl_Vertex.y, 0.0f, 1.0f);
    gl_TexCoord[0] = gl_MultiTexCoord0;
}