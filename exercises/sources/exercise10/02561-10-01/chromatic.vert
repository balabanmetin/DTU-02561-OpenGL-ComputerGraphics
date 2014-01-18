#version 150

uniform mat4 projection;
uniform mat4 modelView;

in vec3 position;

out vec3 vNormal;
out vec3 vPosition;

void main(void)
{
	gl_Position = projection * modelView * vec4(position,1.0);
	vNormal = normalize(position);
	vPosition=position;
}