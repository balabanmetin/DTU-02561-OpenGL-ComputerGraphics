#version 150

in vec2 vTextureCoord;

uniform sampler2D textureMap;

out vec4 fragColor;

void main(void)
{
	fragColor = texture(textureMap,vTextureCoord);
}