#version 150
// 02561-01-04

uniform mat4 projection;
uniform mat4 modelView;

in vec2 position;

void main (void)
{
    gl_Position = projection * modelView * vec4(position, 0.0, 1.0);
}