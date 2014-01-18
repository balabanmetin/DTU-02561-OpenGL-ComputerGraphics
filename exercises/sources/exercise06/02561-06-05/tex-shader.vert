#version 150

uniform mat4 projection;
uniform mat4 modelView;
uniform mat4 textureTrans;


in vec4 position;
in vec2 textureCoord;

out vec2 vTextureCoord;

void main (void)
{
	gl_Position = projection * modelView * position;
	vTextureCoord = (textureTrans * vec4(textureCoord,0.0,1.0)).xy;
}