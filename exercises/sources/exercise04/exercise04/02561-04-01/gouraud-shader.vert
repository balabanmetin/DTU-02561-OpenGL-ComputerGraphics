#version 150 

in  vec3 vPosition;
in  vec3 vNormal;
out vec4 color;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform mat4 ModelView;
uniform mat4 Projection;
uniform vec4 LightPosition;
uniform float Shininess;
 
void main()
{
    vec4 ambient, diffuse, specular; 
	vec4 mPosition= ModelView * vec4(vPosition, 1.0);
	gl_Position = Projection * mPosition;
	vec3 N = normalize(ModelView*vec4(vNormal, 0.0)).xyz;
	vec3 L = normalize(LightPosition.xyz - mPosition.xyz);
	vec3 E = -normalize(mPosition).xyz;
	vec3 H = normalize (L + E) ;	
	float Kd = max(dot(L, N), 0.0);
	float Ks =  pow(max(dot(N, H), 0.0), Shininess);//Shininess is set to 20! Caution!
	ambient=AmbientProduct;
	diffuse = Kd*DiffuseProduct;
	specular= max(Ks*SpecularProduct, 0.0);
	if (dot(N, L) < 0.0) 
		specular.rgb =vec3 (0.0);
    color = ambient + diffuse + specular;
}
