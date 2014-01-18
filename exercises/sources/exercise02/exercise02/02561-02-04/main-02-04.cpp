// 02561-03-04
#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Angel.h"

using namespace std;
using namespace Angel;

int WINDOW_WIDTH = 500;
int WINDOW_HEIGHT = 500;

GLuint shaderProgram;
GLuint projectionUniform,
	modelViewUniform,
	colorUniform;
GLuint positionAttribute;
GLuint unitCubeVertexArrayObject,
	axisVertexArrayObject;

const int axisSize = 6;

struct Vertex {
    vec4 position;
};

void loadShader();
void display();
GLuint loadBufferData(Vertex* vertices, int vertexCount);

void buildUnitCube() {
	const int cubeSize = 8;
	Vertex cubeData[cubeSize] = {
        { vec4( 0.5,  0.5,  0.5, 1.0 ) },
        { vec4( 0.5, -0.5,  0.5, 1.0 ) },
        { vec4( 0.5,  0.5, -0.5, 1.0 ) },
        { vec4( 0.5, -0.5, -0.5, 1.0 ) },
		{ vec4(-0.5,  0.5, -0.5, 1.0 ) },
		{ vec4(-0.5, -0.5, -0.5, 1.0 ) },
		{ vec4(-0.5,  0.5,  0.5, 1.0 ) },
		{ vec4(-0.5, -0.5,  0.5, 1.0 ) }
    };
	unitCubeVertexArrayObject = loadBufferData(cubeData, cubeSize);
}

void buildAxis() {
	Vertex axisData[axisSize] = {
		{vec4(0., 0., 0., 1.0)}, // v0
		{vec4(4., 0., 0., 1.0)}, // vx
		{vec4(0., 4., 0., 1.0)}, // vy
		{vec4(0., 0., 4., 1.0)}, // vz
		{vec4(1., 0., 0., 1.0)}, // v0x1
		{vec4(1., 3., 0., 1.0)} // vyx1
	};
	axisVertexArrayObject = loadBufferData(axisData, axisSize);
}

GLuint loadBufferData(Vertex* vertices, int vertexCount) {
	GLuint vertexArrayObject;

    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);
    
	GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), vertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(positionAttribute);
    glVertexAttribPointer(positionAttribute, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)0);

	return vertexArrayObject;
}

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

void drawWireUnitCube() {
	GLuint indices[24] = {
		0,1,
		1,3,
		3,2,
		2,0,
		4,5,
		5,7,
		7,6,
		6,4,
		7,1,
		6,0,
		4,2,
		5,3
	};
	glBindVertexArray(unitCubeVertexArrayObject);
	glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, indices);
}

void drawAxis() {
	GLuint indices[8] = {
		0,1,
		0,2,
		0,3,
		4,5
	};
	glBindVertexArray(axisVertexArrayObject);
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, indices);
}

void display() {	
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
	
    glUseProgram(shaderProgram);

	mat4 projection = Ortho(-6., 6., -6., 6., -6., 10.);
	glUniformMatrix4fv(projectionUniform, 1, GL_TRUE, projection);

	mat4 modelView;

	modelView=LookAt(vec3(5,5,5), vec3(0,0,0), vec3(0,0,1));
	vec4 white(1.0, 1.0, 1.0, 1.0);
	glUniform4fv(colorUniform, 1, white);
	glUniformMatrix4fv(modelViewUniform, 1, GL_TRUE, modelView);
	drawWireUnitCube();
	
	vec4 red(1.0, 0.0, 0.0, 1.0);
	glUniform4fv(colorUniform, 1, red);
	glUniformMatrix4fv(modelViewUniform, 1, GL_TRUE, modelView);
    drawAxis();

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
	glutCreateWindow("02561-02-04");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);

	Angel::InitOpenGL();
	

	loadShader();
    buildUnitCube();
	buildAxis();
	
	Angel::CheckError();

	glutMainLoop();
}