#version 150

in vec2 position;
in vec2 texCoord;
in vec4 vertexColor;

uniform mat4 orthoMatrix;
uniform mat4 cameraShakeMatrix;

out vec2 interpolatedTexCoord;
out vec4 interpolatedVertexColor;

void main() {
    gl_Position = orthoMatrix * cameraShakeMatrix * vec4(position.x, position.y, 0.0f, 1.0f);
    interpolatedTexCoord = texCoord;
    interpolatedVertexColor = vertexColor;
}
