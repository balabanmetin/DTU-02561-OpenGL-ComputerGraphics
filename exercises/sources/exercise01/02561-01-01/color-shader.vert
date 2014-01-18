#version 150
// 02561-01-01 Hello triangle world

in vec2 position;
in vec3 color;

out vec4 colorV;

void main (void)
{
    colorV = vec4(color,1.0);
    gl_Position = vec4(position,0.0,1.0);
}