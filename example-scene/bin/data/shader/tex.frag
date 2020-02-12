#version 420
uniform sampler2D tex;
in vec2 vTexCoord;
out vec4 outputColor;

// vec3 ContrastSaturationBrightness(vec3 color, float brt, float sat, float con) {
// 	const float AvgLumR = 0.5;
// 	const float AvgLumG = 0.5;
// 	const float AvgLumB = 0.5;
//
// 	const vec3 LumCoeff = vec3(0.2125, 0.7154, 0.0721);
//
// 	vec3 AvgLumin  = vec3(AvgLumR, AvgLumG, AvgLumB);
// 	vec3 brtColor  = color * brt;
// 	vec3 intensity = vec3(dot(brtColor, LumCoeff));
// 	vec3 satColor  = mix(intensity, brtColor, sat);
// 	vec3 conColor  = mix(AvgLumin, satColor, con);
//
// 	return conColor;
// }

void main() {
    vec3 color = texture(tex, vTexCoord).rgb;
    //color = ContrastSaturationBrightness(color, 1., 1.0, 1.0);
    color = pow(color, vec3(0.5)); // Gamma correction
    outputColor = vec4(color, 1.);
}
