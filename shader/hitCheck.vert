#version 420
uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;
in vec4 position;
in vec4 color;
in vec4 normal;
in vec2 texcoord;

uniform mat4 modelMat;
out vec3 vWorldPos;

void main() {
    vWorldPos = (modelMat * position).xyz;
    gl_Position = modelViewProjectionMatrix * position;

}
