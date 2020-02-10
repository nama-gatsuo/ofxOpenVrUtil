#version 420
uniform sampler2D tex;
in vec2 vTexCoord;
out vec4 outputColor;

void main() {
    outputColor = texture(tex, vTexCoord);
}
