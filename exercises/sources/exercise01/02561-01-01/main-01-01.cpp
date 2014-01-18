// 02561-01-01 Hello triangle world

#include <iostream>
#include <string>


#include "Angel.h"

using namespace std;
using namespace Angel;

int WINDOW_WIDTH = 600;
int WINDOW_HEIGHT = 600;

GLuint shaderProgram;
GLuint colorAttribute, positionAttribute;
GLuint vertexArrayObject;

struct Vertex{
	vec2 position;
	vec3 color;
};

const int NUMBER_OF_VERTICES = 6;
Vertex vertexData[NUMBER_OF_VERTICES] = {
	{ vec2(-0.5, -0.5 ), vec3( 1.0, 0.0, 0.0) },
	{ vec2( 0.5, -0.5 ), vec3( 0.0, 1.0, 0.0 ) },
	{ vec2( 0.5,  0.5 ), vec3( 0.0, 0.0, 1.0 ) },
	{ vec2( -0.5,  0.5 ), vec3( 1.0, 1.0, 1.0 ) },
	{ vec2( -0.5, -0.5 ), vec3( 1.0, 1.0, 1.0 ) },
	{ vec2(  0.5,  0.5 ), vec3( 1.0, 1.0, 1.0 ) }
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

	glBindVertexArray(vertexArrayObject);
    
	// GL_TRIANGLES draws a triangle for each 3 points it takes
	glDrawArrays(GL_TRIANGLES, 0, NUMBER_OF_VERTICES);
	
	glutSwapBuffers();
	Angel::CheckError();
}

void reshape(int windowWidth, int windowHeight) {
	glViewport(0, 0, windowWidth, windowHeight);
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
	positionAttribute = glGetAttribLocation(shaderProgram, "position");
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv); // initialize
	glutInitContextVersion(3, 1); //opengl version
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE); //sets flag
	glutInitContextProfile(GLUT_CORE_PROFILE); // sets profile
	
	glutSetOption(
		GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS
	);

	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_3_2_CORE_PROFILE);
	glutCreateWindow("02561-01-01");
	glutDisplayFunc(display); // bind display method - need to draw stuff
	glutReshapeFunc(reshape); // bind reshape method - used when resizing the window
	glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT); // sets the initial windows size

	Angel::InitOpenGL();
		
	loadShader();
	loadBufferData();

	Angel::CheckError();
	glutMainLoop(); // main loop of program. If uncommented, program exits immediately
}