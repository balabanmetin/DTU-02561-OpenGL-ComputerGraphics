// 02561-06-01
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
	linesVertexArrayObject,
	polygonVertexArrayObject;

const int linesSize = 4;
const int polygonSize = 4;

struct Vertex {
    vec4 position;
};

void loadShader();
void display();
GLuint loadBufferData(Vertex* vertices, int vertexCount);

void buildUnitCube(float size) {
	const int cubeSize = 8;
	Vertex cubeData[cubeSize] = {
        { vec4( 0.5*size,  0.5*size,  0.5*size, 1.0 ) },
        { vec4( 0.5*size, -0.5*size,  0.5*size, 1.0 ) },
        { vec4( 0.5*size,  0.5*size, -0.5*size, 1.0 ) },
        { vec4( 0.5*size, -0.5*size, -0.5*size, 1.0 ) },
		{ vec4(-0.5*size,  0.5*size, -0.5*size, 1.0 ) },
		{ vec4(-0.5*size, -0.5*size, -0.5*size, 1.0 ) },
		{ vec4(-0.5*size,  0.5*size,  0.5*size, 1.0 ) },
		{ vec4(-0.5*size, -0.5*size,  0.5*size, 1.0 ) }
    };
	unitCubeVertexArrayObject = loadBufferData(cubeData, cubeSize);
}

void buildLines() {
	Vertex axisData[linesSize] = {
		{vec4( 0,  0, 0, 1.0)},
		{vec4(15,  0, 0, 1.0)}, 
		{vec4( 0,  0, 0, 1.0)},
		{vec4( 0, 15, 0, 1.0)} 
	};
	linesVertexArrayObject = loadBufferData(axisData, linesSize);
}

void buildPolygon() {
	Vertex polygonData[linesSize] = {
		{vec4(13., 13, 13, 1.0)},
		{vec4(13., -13, 13, 1.0)}, 
		{vec4(13., -13, -13, 1.0)},
		{vec4(13., 13, -13, 1.0)}
	};
	polygonVertexArrayObject = loadBufferData(polygonData, polygonSize);
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

void drawSolidUnitCube() {
	GLuint indices[36] = {
		0,1,2,
		2,1,3,
		2,3,4,
		4,3,5,
		4,5,6,
		6,5,7,
		6,7,0,
		0,7,1,
		6,0,2,
		6,2,4,
		7,5,3,
		7,3,1
	};
	glBindVertexArray(unitCubeVertexArrayObject);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, indices);
}

void drawLines() {
	glBindVertexArray(linesVertexArrayObject);
	glDrawArrays(GL_LINES, 0, linesSize);
}

void drawPolygon() {
	glBindVertexArray(polygonVertexArrayObject);
	glDrawArrays(GL_TRIANGLE_FAN, 0, polygonSize);
	
}

void display() {	
    glClearColor(0.,0.,0.,1.);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
    glUseProgram(shaderProgram);

	mat4 projection = Perspective(90., (GLfloat)WINDOW_WIDTH / (GLfloat) WINDOW_HEIGHT, 0.1, 50.);
	glUniformMatrix4fv(projectionUniform, 1, GL_TRUE, projection);

	vec4 eye(20., 10., 0.,1);
	vec4 at(0., 10., 0.,1);
	vec4 up(0., 0., 1.,0);
	mat4 modelView = LookAt(eye, at, up);
	glUniformMatrix4fv(modelViewUniform, 1, GL_TRUE, modelView);

	vec4 red(1., 0., 0., 1.0);
	glUniform4fv(colorUniform, 1, red);
	drawLines();
	
	glUniform4fv(colorUniform, 1, vec4(0., 1., 0., 1.0));
	drawPolygon();
	
	glUniform4fv(colorUniform, 1, vec4(0., 1., 1., 1.0));
	drawSolidUnitCube();

	modelView = modelView * Translate(0,20,0) * RotateY(180) * Scale(.2, .2, .2);
	glUniformMatrix4fv(modelViewUniform, 1, GL_TRUE, modelView);
	glUniform4fv(colorUniform, 1, vec4(0., 1., 0., 1.0));
	drawPolygon();

	glutSwapBuffers();

	Angel::CheckError();
}

void reshape(int W, int H) {
    WINDOW_WIDTH = W;
	WINDOW_HEIGHT = H;
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void keyboard( unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'h':
	case 'H':
		glEnable(GL_DEPTH_TEST);
		glutPostRedisplay();
		break;
	case 'j':
	case 'J':
		glDisable(GL_DEPTH_TEST);
		glutPostRedisplay();
		break;
	case '1':
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glutPostRedisplay();
		break;
	case '2':
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glutPostRedisplay();
		break;
	case '3':
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT_AND_BACK);
		glutPostRedisplay();
		break;
	case '0':
		glDisable(GL_CULL_FACE);
		glutPostRedisplay();
		break;
	default:
		break;
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

	glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH|GLUT_DOUBLE|GLUT_3_2_CORE_PROFILE);
	glutCreateWindow("02561-06-01");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);

	Angel::InitOpenGL();

	loadShader();
    buildUnitCube(6);
	buildLines();
	buildPolygon();
	
	Angel::CheckError();

	glutMainLoop();
}