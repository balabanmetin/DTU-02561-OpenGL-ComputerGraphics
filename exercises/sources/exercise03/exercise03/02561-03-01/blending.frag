#version 150
// 02561-03-01

in vec4 glPosition;
in vec3 colorV;

out vec4 fragColor;

void main(void)
{
	if(mod(round(glPosition[1]),2.0)==0)
		discard;
    fragColor = vec4(colorV, 1.0);
}