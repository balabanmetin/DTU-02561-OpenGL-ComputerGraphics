#version 150 

in  vec3 vPosition;
in  vec3 vNormal;
out vec4 modelPosition;
out vec4 modelNormal;
out vec3 LightPos;


uniform mat4 ModelView;
uniform mat4 Projection;
uniform vec4 LightPosition;

void main()
{
    gl_Position = Projection * ModelView * vec4(vPosition, 1.0);
	vec4 mP= ModelView * vec4(vPosition, 1.0);
	modelPosition = mP;
	modelNormal = ModelView*vec4(vNormal, 0.0);
	if(LightPosition.w==1.0)
	{
		LightPos = (LightPosition -mP).xyz;
	}
	else
	{
		LightPos= -LightPosition.xyz;
	}


}
