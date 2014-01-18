
#version 150

uniform samplerCube cubemap;
uniform float w;

in vec3 v_refraction;

in vec3 v_reflection;
in float v_fresnel;
in float cos_theta_t;
in float eta;

out vec4 fragColor;


void main(void)
{
		float a =  2 * 3.141592 * w * eta * cos_theta_t;
		float reda = sin(a/720.);
		float greena = sin(a/540.);
		float bluea = sin(a/450.);
        vec4 refractionColor = texture(cubemap, v_refraction);
     		
        vec4 reflectionLookup = texture(cubemap, normalize(v_reflection));
		vec4 reflectionColor;
		reflectionColor.r = (reda*reda*reflectionLookup).r;
		reflectionColor.g = (greena*greena*reflectionLookup).g;
		reflectionColor.b = (bluea*bluea*reflectionLookup).b;
		reflectionColor.w = reflectionLookup.w;
		//###############################################
		//# To Do: per-fragment reflection, refraction	#
		//# Pelvin noise								#
		//# G-force width change						#
		//												#
		//###############################################

        fragColor =mix(refractionColor, reflectionColor, 4*v_fresnel);
}
