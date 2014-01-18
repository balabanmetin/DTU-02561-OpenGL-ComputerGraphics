#version 150

uniform samplerCube cubemap;
uniform sampler2D textureBump;
uniform vec3 cameraPos;

in vec3 vNormal;
in vec3 vPosition;

out vec4 fragColor;

vec3 bumpMap(vec3 normal, vec3 position)
{ 
	vec3 n = normalize(normal); 
	float radius = length(vec3(position)); 
	float PI = 3.14159265; 
	vec2 tangent2 = vec2(acos(position.z/radius), atan(position.y, position.x)); 
	vec2 tangentUV = vec2(tangent2.x / (2*PI), tangent2.y / PI);
	vec3 T = vec3(cos(tangent2.x)*cos(tangent2.y), cos(tangent2.x)*sin(tangent2.y),-sin(tangent2.x)); 
	vec3 B = vec3(-sin(tangent2.x)*sin(tangent2.y), sin(tangent2.x)*cos(tangent2.y), 0.0); 
	mat3 tbn = mat3(T,B,n);
	vec3 nn = texture(textureBump,tangentUV).xyz; 
	vec3 V = (nn - vec3(0.5, 0.5, 0.5))*2.0; 
	return normalize(tbn*V); 
}



void main(void)
{
	float air = 1.0;
	float glass = 1.62;
		
	vec3 n = bumpMap(vNormal, vPosition); 
	vec3 i = normalize(vPosition - cameraPos); 
	vec3 rv = normalize(reflect(i, n)); 
	vec3 rfv = normalize(refract(i, n, air/glass)); //consider normalization
	vec4 reflection = texture(cubemap, rv); 
	vec4 refraction = texture(cubemap, rfv);
	float R0 = pow(abs((air-glass)/(air+glass)),2.0); 
	float R = R0 + (1-R0)*pow(1.-max(dot(n, -i),0.), 5);
	fragColor = R*reflection + refraction*(1-R);
}