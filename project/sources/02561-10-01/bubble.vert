#version 150

uniform mat4 projection;
uniform mat4 modelView;
uniform vec3 cameraPos;


in vec3 position;

out vec3 v_reflection;
out vec3 v_refraction;

out float v_fresnel;
out float cos_theta_t;
out float eta;


const float Air = 1.0;
const float Glass = 1.33;


const float Eta = Air / Glass;

 
const float R0 = ((Air - Glass) * (Air - Glass)) / ((Air + Glass) * (Air + Glass));

void main(void)
{
        // We calculate in world space.
        
        vec4 vertex = vec4(position, 1.0);
	    
        vec3 incident = normalize(vec3(position-cameraPos));

        // Assume incoming normal is normalized.
        vec3 normal = normalize(position);
        
        v_refraction = incident; //ignoring refraction
 
        v_reflection = reflect(incident, normal);
                        
        v_fresnel = R0 + (1.0 - R0) * pow((1.0 - dot(-incident, normal)), 5.0);
        cos_theta_t =  sqrt(1 + Eta*Eta*(dot(-incident, normal)*dot(-incident, normal)-1));
		eta = Eta;    
        gl_Position = projection*modelView*vertex;
}