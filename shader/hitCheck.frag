#version 420
in vec3 vWorldPos;

out vec4 outputColor;
uniform int id;

void main() {
    outputColor = vec4(vWorldPos, float(id));
}
