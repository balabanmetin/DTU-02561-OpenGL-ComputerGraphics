// 02561-06-02
#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Angel.h"
#include "Teapot.h"

using namespace std;
using namespace Angel;

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 800;

GLuint shaderProgram;
GLuint projectionUniform,
	modelViewUniform,
	colorUniform;
GLuint positionAttribute;
GLuint teapotVAO,
	vertexBuffer;

int numVertices;

struct Vertex {
    vec4 position;
};

void loadShader();
void display();

void loadShader(){
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
	positionAttribute = glGetAttribLocation(shaderProgram, "position");
	if (positionAttribute == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'position' attribute." << endl;
	}
}

void display() {	
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
	
    glUseProgram(shaderProgram);

	mat4 projection = Perspective(24.3,WINDOW_WIDTH/(float)WINDOW_HEIGHT, 0.01, 10);
	glUniformMatrix4fv(projectionUniform, 1, GL_TRUE, projection);

	vec4 eye(3,5,10,1);
	vec4 at(0,0,0,1);
	vec4 up(0,1,0,0);
	mat4 modelView = LookAt(eye, at, up);
	glUniformMatrix4fv(modelViewUniform, 1, GL_TRUE, modelView);

	vec4 red(1., 0., 0., 1.0);
	glUniform4fv(colorUniform, 1, red);

	glBindVertexArray(teapotVAO);

    glDrawArrays( GL_TRIANGLES, 0, numVertices);
    
	glutSwapBuffers();
	Angel::CheckError();
}

void reshape(int W, int H) {
    WINDOW_WIDTH = W;
	WINDOW_HEIGHT = H;
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
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
	glutCreateWindow("02561-06-2");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);

	Angel::InitOpenGL();

	loadShader();
    teapotVAO = LoadTeapotVAO(numVertices,positionAttribute);

	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	Angel::CheckError();
	glutMainLoop();
}