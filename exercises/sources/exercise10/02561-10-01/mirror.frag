#version 150

uniform samplerCube cubemap;
uniform vec3 cameraPos;

in vec3 vNormal;
in vec3 vPosition;

out vec4 fragColor;

void main(void)
{
	vec3 n = normalize(vNormal); 
	vec3 i = normalize(vPosition - cameraPos); 
	vec3 rv = normalize(reflect(i, n)); 
	fragColor = texture(cubemap, rv);
}
