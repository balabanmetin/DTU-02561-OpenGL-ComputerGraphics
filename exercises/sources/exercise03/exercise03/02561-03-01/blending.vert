#version 150
// 02561-03-01

uniform mat4 projection;
uniform mat4 modelView;
uniform float blendValue;
uniform float normalExtrusion;

in vec3 position1;
in vec3 color1;
in vec3 position2;
in vec3 color2;
in vec3 normal1;
in vec3 normal2;

out vec3 colorV;
out vec4 glPosition;


void main (void) {
	glPosition=vec4((1-blendValue)*position1 + blendValue*position2, 1.0);
	vec4 normalWtihBlendRatio = vec4((1-blendValue)*normal1 + blendValue*normal2, 0.0);
    colorV = (1-blendValue)*color1 + blendValue*color2;
    gl_Position = projection * modelView * (glPosition + normalExtrusion*normalWtihBlendRatio);
}