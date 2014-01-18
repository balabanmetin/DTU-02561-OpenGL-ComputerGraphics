// 02561-01-05

#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Angel.h"

using namespace std;
using namespace Angel;

int WINDOW_WIDTH  = 600;
int WINDOW_HEIGHT = 600;

GLuint shaderProgram;
GLuint projectionUniform,
	modelViewUniform;
GLuint colorAttribute, positionAttribute;
GLuint vertexArrayObject;

struct Vertex {
	vec2 position;
	vec3 color;
};

const int NUMBER_OF_VERTICES = 3;
Vertex vertexData[NUMBER_OF_VERTICES] = {
	{ vec2( 0.0,  0.0 ), vec3( 1.0, 0.0, 0.0) },
	{ vec2( 1.0,  0.0 ), vec3( 0.0, 1.0, 0.0 ) },
	{ vec2( 1.0,  1.0 ), vec3( 0.0, 0.0, 1.0 ) }
};

// forward declaration
void loadShader();
void display();
void loadBufferData();
void reshape(int W, int H);

void display() {	
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glUseProgram(shaderProgram);

	mat4 projection = Ortho2D(-20.0f, 20.0f, -20.0f, 20.0f);
    glUniformMatrix4fv(projectionUniform, 1, GL_TRUE, projection);

	mat4 modelView;

	glBindVertexArray(vertexArrayObject);

	// todo transform modelView and draw triangles
	modelView=Scale(-10,+10,10);
	//cout<<modelView;
	glUniformMatrix4fv(modelViewUniform, 1, GL_TRUE, modelView);
	glDrawArrays(GL_TRIANGLES, 0, NUMBER_OF_VERTICES);

	modelView=Scale(-10,10,10)*RotateZ(-90);//problematic
	glUniformMatrix4fv(modelViewUniform, 1, GL_TRUE, modelView);
	glDrawArrays(GL_TRIANGLES, 0, NUMBER_OF_VERTICES);	
	
	modelView=Scale(+10,-10,10);
	glUniformMatrix4fv(modelViewUniform, 1, GL_TRUE, modelView);
	glDrawArrays(GL_TRIANGLES, 0, NUMBER_OF_VERTICES);
		
	modelView=Scale(+10,-10,+10)*RotateZ(-90);//problematic
	glUniformMatrix4fv(modelViewUniform, 1, GL_TRUE, modelView);
	glDrawArrays(GL_TRIANGLES, 0, NUMBER_OF_VERTICES);
	
	modelView=Translate(+10,+10,0)*Scale(+20/sqrt(2),-20/sqrt(2),+20/sqrt(2))*RotateZ(45);
	glUniformMatrix4fv(modelViewUniform, 1, GL_TRUE, modelView);
	glDrawArrays(GL_TRIANGLES, 0, NUMBER_OF_VERTICES);	
	
	modelView=Translate(-10,-10,0)*Scale(-20/sqrt(2),+20/sqrt(2),+20/sqrt(2))*RotateZ(45);
	glUniformMatrix4fv(modelViewUniform, 1, GL_TRUE, modelView);
	glDrawArrays(GL_TRIANGLES, 0, NUMBER_OF_VERTICES);	
	
	modelView=Translate(+10,+10,0)*Scale(-20/sqrt(2),-20/sqrt(2),+20/sqrt(2))*RotateZ(-45);
	glUniformMatrix4fv(modelViewUniform, 1, GL_TRUE, modelView);
	glDrawArrays(GL_TRIANGLES, 0, NUMBER_OF_VERTICES);	
	
	modelView=Translate(-10,-10,0)*Scale(-20/sqrt(2),-20/sqrt(2),+20/sqrt(2))*RotateZ(135);
	glUniformMatrix4fv(modelViewUniform, 1, GL_TRUE, modelView);
	glDrawArrays(GL_TRIANGLES, 0, NUMBER_OF_VERTICES);	
	
	glutSwapBuffers();


	Angel::CheckError();
}

void reshape(int W, int H) {
	WINDOW_WIDTH = W;
	WINDOW_HEIGHT = H;
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void loadBufferData() {
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);
	GLuint  vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, NUMBER_OF_VERTICES * sizeof(Vertex), vertexData, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(positionAttribute);
	glEnableVertexAttribArray(colorAttribute);
	glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)0);
	glVertexAttribPointer(colorAttribute  , 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)sizeof(vec2));
}

void loadShader() {
	shaderProgram = InitShader("color-shader.vert",  "color-shader.frag", "fragColor");
	colorAttribute = glGetAttribLocation(shaderProgram, "color");
	if (colorAttribute == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'color' uniform."<<endl;
	}
	positionAttribute = glGetAttribLocation(shaderProgram, "position");
	if (positionAttribute == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'position' uniform."<<endl;
	}
	projectionUniform = glGetUniformLocation(shaderProgram, "projection");
	if (projectionUniform == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'projection' uniform."<<endl;
	}
	modelViewUniform = glGetUniformLocation(shaderProgram, "modelView");
	if (modelViewUniform == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'modelView' uniform."<<endl;
	}
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitContextVersion(3, 1);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	
	glutSetOption(
		GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS
	);

	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_3_2_CORE_PROFILE);
	glutCreateWindow("02561-01-05");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);

	Angel::InitOpenGL();
	
	loadShader();
	loadBufferData();

	Angel::CheckError();

	glutMainLoop();
}