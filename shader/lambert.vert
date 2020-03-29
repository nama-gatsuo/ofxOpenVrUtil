#version 420
uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;
in vec4 position;
in vec4 color;
in vec4 normal;
in vec2 texcoord;

uniform vec4 baseColor;

out vec2 vTexCoord;
out vec4 vNormal;
out vec3 vColor;

void main() {

    vTexCoord = texcoord;
    vNormal = normal;
    vColor = baseColor.rgb;
    gl_Position = modelViewProjectionMatrix * position;

}
