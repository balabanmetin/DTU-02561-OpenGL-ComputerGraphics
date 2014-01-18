#version 150

uniform vec4 color;
uniform vec4 clipPlane;

in vec3 vNormal;
in vec3 vLight;
in vec3 vWorldPos;

out vec4 fragColor;

void main(void) {
	vec3 n = normalize(vNormal);
	vec3 l = normalize(vLight);

	// light computed in world space
	float Kd = max(dot(l, n), 0.0);
    float ambience = 0.3;
	vec3 diffuseColor = vec3(1,1,1)*(Kd+ambience);
	
	fragColor = vec4(diffuseColor,1.0) * color;
}
