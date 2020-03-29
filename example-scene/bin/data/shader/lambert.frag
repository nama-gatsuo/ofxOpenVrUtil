#version 420
uniform mat4 modelViewMatrix;
out vec4 outputColor;
uniform vec3 lightDir;
uniform mat4 viewMatrix;

in vec2 vTexCoord;
in vec3 vColor;
in vec4 vNormal;

void main() {
    if (all(lessThan(vTexCoord, vec2(0.98))) && all(greaterThan(vTexCoord, vec2(0.02)))) discard;

    vec4 n = normalize(modelViewMatrix * vec4(vNormal.xyz, 0));
    vec4 l = normalize(viewMatrix * vec4(lightDir.xyz, 0));

    float lamb = mix(0.6, 1.0, dot(n.xyz, l.xyz));
    outputColor = vec4(vColor * lamb, 1.);

}
