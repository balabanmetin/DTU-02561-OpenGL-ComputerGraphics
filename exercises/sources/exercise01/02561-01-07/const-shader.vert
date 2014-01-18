#version 150
// 02561-01-07

in vec2 position;

void main (void)
{
    gl_Position = vec4(position, 0.0, 1.0);
}