#version 150

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform mat4 lightViewProjection;

out vec4 fragColor;

in vec2 vTextureCoordinate;
in vec4 worldCoordinates;

void main(void) {
	if (!gl_FrontFacing){
		fragColor = vec4(1,1,1,1);
		return;
	}
	mat4 biasMatrix;
	biasMatrix[0]=vec4(0.5, 0.0, 0.0, 0.0);
	biasMatrix[1]=vec4(0.0, 0.5, 0.0, 0.0);
	biasMatrix[2]=vec4(0.0, 0.0, 0.5, 0.0);
	biasMatrix[3]=vec4(0.5, 0.5, 0.5, 1.0);

	vec4 shadowMapCoordinates = lightViewProjection * worldCoordinates;
	
	shadowMapCoordinates = shadowMapCoordinates / (shadowMapCoordinates.w);
	shadowMapCoordinates = biasMatrix*shadowMapCoordinates;
	if( (shadowMapCoordinates.x >1.0 || shadowMapCoordinates.x<0) ||
		(shadowMapCoordinates.y >1.0 || shadowMapCoordinates.y<0) ||
		(shadowMapCoordinates.z >1.0 || shadowMapCoordinates.z<0) ) {
		//fragColor = vec4(1,1,1,1);
		//return;
	}
		
	vec2 shadowUV; // todo find the shadow map UV coordinates
	shadowUV = shadowMapCoordinates.xy;
	vec4 blue= texture(texture1, vTextureCoordinate);
	vec4 colors=texture(texture2, shadowUV);
	if(colors.x<1)
		fragColor=colors;
	else
		fragColor = blue;
}
