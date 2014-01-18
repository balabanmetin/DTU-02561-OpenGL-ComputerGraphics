// 02561-02-01
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
	modelViewUniform;
GLuint positionAttribute;
GLuint teapotVAO,
	vertexBuffer;

int numVertices;

struct Vertex {
    vec4 position;
};

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
	positionAttribute = glGetAttribLocation(shaderProgram, "position");
	if (positionAttribute == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'position' attribute." << endl;
	}
}

void display() {	
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
	
    glUseProgram(shaderProgram);

	mat4 projection = Ortho(-5.0f, 5.0f, -5.0f, 5.0f, 30.0f, -30.0f);
	glUniformMatrix4fv(projectionUniform, 1, GL_TRUE, projection);

	vec3 eye(0,3,6);
	vec3 at(0,0,0);
	vec3 up(0,1,0);
	mat4 modelView = LookAt(eye, at, up);
	glUniformMatrix4fv(modelViewUniform, 1, GL_TRUE, modelView);

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
	glutCreateWindow("02561-02-01");
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