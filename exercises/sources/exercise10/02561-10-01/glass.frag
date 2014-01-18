#version 150

uniform samplerCube cubemap;
uniform vec3 cameraPos;

in vec3 vNormal;
in vec3 vPosition;

out vec4 fragColor;


void main(void)
{
	float air = 1.0;
	float glass = 1.62;
		
	vec3 n = normalize(vNormal); 
	vec3 i = normalize(vPosition - cameraPos); 
	vec3 rv = normalize(reflect(i, n)); 
	vec3 rfv = normalize(refract(i, n, air/glass)); //consider normalization
	vec4 reflection = texture(cubemap, rv); 
	vec4 refraction = texture(cubemap, i);
	float R0 = pow(abs((air-glass)/(air+glass)),2.0); 
	float R = R0 + (1-R0)*pow(1.-max(dot(n, -i),0.), 5);
	fragColor = R*reflection + refraction*(1-R);

}
