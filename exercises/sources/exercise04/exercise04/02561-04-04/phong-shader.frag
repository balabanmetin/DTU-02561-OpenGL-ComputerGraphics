#version 150 

in vec4 modelPosition;
in vec4 modelNormal;
out vec4 fragColor;


uniform vec4 LightAmbient;
uniform vec4 Light1Position, Light2Position, Light3Position;
uniform vec4 Light1Diffuse, Light2Diffuse, Light3Diffuse;
uniform vec4 Light1Specular, Light2Specular, Light3Specular;
uniform vec4 MaterialAmbient, MaterialDiffuse, MaterialSpecular;

uniform float Shininess;

void main() 
{ 
 
    vec4 AmbientProduct = LightAmbient * MaterialAmbient;
    
	vec4 DiffuseProduct[3];
	DiffuseProduct[0]= Light1Diffuse * MaterialDiffuse;
	DiffuseProduct[1]= Light2Diffuse * MaterialDiffuse;
	DiffuseProduct[2]= Light3Diffuse * MaterialDiffuse;
	 
    vec4 SpecularProduct[3];
	SpecularProduct[0]= Light1Specular * MaterialSpecular;
	SpecularProduct[1]= Light2Specular * MaterialSpecular;
	SpecularProduct[2]= Light3Specular * MaterialSpecular;

	vec4 LightPos[3]; 
	LightPos[0]=Light1Position;
	LightPos[1]=Light2Position;
	LightPos[2]=Light3Position;

    vec4 ambient, diffuse, specular;	

	 ambient=AmbientProduct;

	fragColor = ambient;
	for(int i=0;i<2;i++)
	{
		vec3 N = normalize(modelNormal).xyz;
		vec3 L = -normalize(LightPos[i].xyz);
		vec3 E = normalize(LightPos[i].xyz-modelPosition.xyz);
		vec3 H = normalize (L + E) ;	
		float Kd = max(dot(L, N), 0.0);
		float Ks =  pow(max(dot(N, H), 0.0), Shininess);//Shininess is set to 100! Caution!
		diffuse = Kd*DiffuseProduct[i];
		specular= max(Ks*SpecularProduct[i], 0.0);
		//if (dot(N, L) < 0.0) 
			//specular.rgb =vec3 (0.0);
		fragColor +=diffuse;
		fragColor +=specular;

	}
    
	
} 

