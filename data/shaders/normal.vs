uniform mat4 modelToWorldMatrix;
uniform mat4 worldToViewMatrix;
varying vec2 texCoord;

void main() {
    texCoord = gl_MultiTexCoord0;
    gl_Position = worldToViewMatrix * modelToWorldMatrix * gl_Vertex;
}