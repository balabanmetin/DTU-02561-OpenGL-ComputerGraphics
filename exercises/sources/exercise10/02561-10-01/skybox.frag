#version 150

uniform samplerCube cubemap;

out vec4 fragColor;
in vec3 normal;

void main(void)
{
	fragColor = texture(cubemap,normal);
}
