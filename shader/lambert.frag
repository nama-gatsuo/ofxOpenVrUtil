#version 420
uniform mat4 modelViewMatrix;
out vec4 outputColor;
uniform vec3 lightDir;

in vec2 vTexCoord;
in vec3 vColor;
in vec4 vNormal;

void main() {
    float lamb = mix(0.6, 1.0, dot(vNormal.xyz, normalize(lightDir.xyz)));
    outputColor = vec4(vColor * lamb, 1.);
}
