#version 150

uniform mat4 projection;
uniform mat4 modelView;

in vec4 position;

void main (void)
{
    gl_Position = projection * modelView * position;
}