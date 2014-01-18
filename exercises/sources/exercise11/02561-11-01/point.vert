#version 150

uniform mat4 projection;
uniform mat4 modelView;

in float u;

void main(void)
{
	vec4 point = vec4(0,0,0,u);
	gl_Position = projection * modelView * point;
}