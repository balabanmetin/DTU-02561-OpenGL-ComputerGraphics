#include "Axis.h"

#include <iostream>

using namespace std;

Axis::Axis(void)
	:vertexArrayObject(GL_INVALID_INDEX),
	 shaderProgram(GL_INVALID_INDEX),
	 projectionUniform(GL_INVALID_INDEX),
	 modelViewUniform(GL_INVALID_INDEX),
	 colorUniform(GL_INVALID_INDEX)
{
}

Axis::~Axis(void)
{
}

void Axis::renderWorldAxis(mat4& projection, vec4 eye, vec4 at, vec4 up, int window_width, int window_height, float width, float heigh){
	if (vertexArrayObject == GL_INVALID_INDEX){
		initialize();
	}

	glViewport(int(window_width*(1-width)), int(window_height*(1-heigh)), int(window_width*width), int(window_height*heigh));
	glScissor(int(window_width*(1-width)), int(window_height*(1-heigh)), int(window_width*width), int(window_height*heigh));
	glEnable(GL_SCISSOR_TEST);
	glClearColor(0.8, 0.8, 0.8, 1.0);
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	
	// translate to zero
	eye = eye - at;
	at = vec4(0,0,0,1);

	// change distance
	eye = normalize(eye)*2;

	mat4 modelview = LookAt(eye, at, up);

	glUseProgram(shaderProgram);
	glBindVertexArray(vertexArrayObject);
	glUniformMatrix4fv(modelViewUniform, 1, GL_TRUE, modelview);
	glUniformMatrix4fv(projectionUniform, 1, GL_TRUE, projection);

	// draw X axis
	glUniform4fv(colorUniform,1,vec4(1,0,0,1));
	glDrawArrays(GL_LINES,0,vertexCount);
	
	// draw Y axis
	glUniformMatrix4fv(modelViewUniform, 1, GL_TRUE, modelview * RotateZ(90));
	glUniform4fv(colorUniform,1,vec4(0,1,0,1));
	glDrawArrays(GL_LINES,0,vertexCount);
	
	// draw Y axis
	glUniformMatrix4fv(modelViewUniform, 1, GL_TRUE, modelview * RotateY(-90));
	glUniform4fv(colorUniform,1,vec4(0,0,1,1));
	glDrawArrays(GL_LINES,0,vertexCount);
	
	glViewport(0,0,window_width, window_height);
	glScissor(0,0,window_width, window_height);
	glDisable(GL_SCISSOR_TEST);
}

void Axis::initialize() {
	shaderProgram = InitShader("const-shader.vert",  "const-shader.frag", "fragColor");
	projectionUniform = glGetUniformLocation(shaderProgram, "projection");
	if (projectionUniform == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'projection' uniform."<<endl;
	}
	modelViewUniform = glGetUniformLocation(shaderProgram, "modelView");
	if (modelViewUniform == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'modelView' uniform."<<endl;
	}
	colorUniform = glGetUniformLocation(shaderProgram, "color");
	if (colorUniform == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'color' uniform."<<endl;
	}
	GLuint positionAttribute = glGetAttribLocation(shaderProgram, "position");
	if (positionAttribute == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'position' attribute." << endl;
	}
	
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);
    
	vec4 vertices[2] = {
		vec4(0,0,0,1),
		vec4(2,0,0,1),
	};
	vertexCount = 2;

	GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(vec4), vertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(positionAttribute);
    glVertexAttribPointer(positionAttribute, 4, GL_FLOAT, GL_FALSE, sizeof(vec4), (const GLvoid *)0);
}

