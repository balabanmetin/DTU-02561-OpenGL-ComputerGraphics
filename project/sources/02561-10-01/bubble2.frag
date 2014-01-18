#version 150

uniform samplerCube cubemap;
uniform vec3 cameraPos;
uniform float w;

in vec3 vNormal;
in vec3 vPosition;

out vec4 fragColor;


const float Air = 1.0;
const float Bubble = 1.33;


const float Eta = Air / Bubble;

 
const float R0 = ((Air - Bubble) * (Air - Bubble)) / ((Air + Bubble) * (Air + Bubble));


void main(void)
{		
	vec3 n = vNormal; 
	vec3 i = normalize(vPosition - cameraPos); 
	vec3 rv = normalize(reflect(i, n)); 
	//vec3 rfv = normalize(refract(i, n, air/glass)); //consider normalization
	vec4 reflection = texture(cubemap, rv); 
	vec4 refraction = texture(cubemap, i);
	//float R0 = pow(abs((air-glass)/(air+glass)),2.0); 
	float v_fresnel = R0 + (1-R0)*pow(1.-max(dot(n, -i),0.), 5);
	float cos_theta_t =  sqrt(1 + Eta*Eta*(dot(-i, n)*dot(-i, n)-1));
	float a =  2 * 3.141592 * w * Eta * cos_theta_t;
	
	float reda = sin(a/720.);
	float greena = sin(a/540.);
	float bluea = sin(a/450.);
	vec4 reflectionColor;
	reflectionColor.r = (reda*reda*reflection).r;
	reflectionColor.g = (greena*greena*reflection).g;
	reflectionColor.b = (bluea*bluea*reflection).b;
	reflectionColor.w = reflection.w;

	fragColor =mix(refraction, reflectionColor, 4*v_fresnel);

}
