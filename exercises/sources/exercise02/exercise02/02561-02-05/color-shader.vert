#version 150
// 02561-01-01 Hello triangle world

uniform mat4 projection;
uniform mat4 modelView;

in vec3 position;
in vec3 color;

out vec4 colorV;

void main (void)
{
    colorV = vec4(color,1.0);
    gl_Position = projection * modelView * vec4(position,1.0);
}