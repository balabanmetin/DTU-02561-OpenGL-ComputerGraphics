#version 150 

in vec4 modelPosition;
in vec4 modelNormal;
in vec3 LightPos;
out vec4 fragColor;


uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform float Shininess;

void main() 
{ 
    vec4 ambient, diffuse, specular; 

	vec3 N = normalize(modelNormal).xyz;
	vec3 L = normalize(LightPos);
	vec3 E = -normalize(modelPosition).xyz;
	vec3 H = normalize (L + E) ;	
	float Kd = max(dot(L, N), 0.0);
	float Ks =  pow(max(dot(N, H), 0.0), Shininess);//Shininess is set to 10! Caution!
	ambient=AmbientProduct;
	diffuse = Kd*DiffuseProduct;
	specular= max(Ks*SpecularProduct, 0.0);
	if (dot(N, L) < 0.0) 
		specular.rgb =vec3 (0.0);
    fragColor = ambient + diffuse + specular;

} 

