#version 150

uniform mat4 projection;
uniform mat4 modelView;

in vec4 position;
in vec2 textureCoord;

out vec2 vTextureCoord;

void main (void)
{
	gl_Position = projection * modelView * position;
	vTextureCoord = textureCoord;
}