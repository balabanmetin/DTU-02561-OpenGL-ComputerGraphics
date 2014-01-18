#version 150

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMat;
uniform vec3 pointLightPos;


in vec3 position;
in vec3 normal;

out vec3 vNormal;
out vec3 vLight;
out vec3 vWorldPos;

void main(void)
{
	vec4 worldPos = model * vec4(position,1.0);
	gl_Position = projection * view * worldPos;
	vNormal = normalMat * normal;
	vWorldPos = worldPos.xyz;
	vLight = pointLightPos-vWorldPos;
}