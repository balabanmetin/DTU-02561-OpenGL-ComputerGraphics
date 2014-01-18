#version 150

uniform samplerCube cubemap;
uniform vec3 cameraPos;

in vec3 vNormal;
in vec3 vPosition;

out vec4 fragColor;


void main(void)
{

	float air = 1.0;
	float glass = 1.615;
	float glassRed = 1.56;
	float glassGreen = 1.615;
	float glassBlue = 1.63;

		
	vec3 n = normalize(vNormal); 
	vec3 i = normalize(vPosition - cameraPos); 
	vec3 rv = normalize(reflect(i, n)); 
	vec3 rfvr = normalize(refract(i, n, air/glassRed)); 
	vec3 rfvg = normalize(refract(i, n, air/glassGreen)); 
	vec3 rfvb = normalize(refract(i, n, air/glassBlue)); 
	vec4 reflection = texture(cubemap, rv); 
	vec4 refractionRed = texture(cubemap, rfvr);
	vec4 refractionGreen = texture(cubemap, rfvg);
	vec4 refractionBlue = texture(cubemap, rfvb);
	vec4 refraction = vec4(refractionRed.r, refractionGreen.g, refractionBlue.b, (refractionRed.w+refractionGreen.w+refractionBlue.w)/3.0);
	float R0 = pow(abs((air-glass)/(air+glass)),2.0); 
	float R = R0 + (1-R0)*pow(1.-max(dot(n, -i),0.), 5);
	fragColor = R*reflection + refraction*(1-R);

}
