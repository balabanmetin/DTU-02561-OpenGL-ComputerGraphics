// 02561-01-03

#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Angel.h"
#include "mat.h"

using namespace std;
using namespace Angel;

int WINDOW_WIDTH = 600;
int WINDOW_HEIGHT = 500;

GLuint shaderProgram;
GLuint projectionUniform,
	modelViewUniform;
GLuint colorAttribute, positionAttribute;
GLuint shapeVertexArrayBuffer;
const GLuint rectangleSize = 6;

struct Vertex {
    vec2 position;
    vec3 color;
};

void loadShader();
void display();
GLuint loadBufferData(Vertex* vertices, int vertexCount);
void reshape(int W, int H);
void loadGeometry();


void display() {	
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
	
	glUseProgram(shaderProgram);

	mat4 projection = Ortho2D(-15.0f, 15.0f, -15.0f, 15.0f);
    glUniformMatrix4fv(projectionUniform, 1, GL_TRUE, projection);

	glBindVertexArray(shapeVertexArrayBuffer);

	mat4 modelView;
    
	// upper left
	modelView = Translate(-7,+7,0);
	glUniformMatrix4fv(modelViewUniform, 1, GL_TRUE, modelView);
	glDrawArrays(GL_TRIANGLE_FAN, 0, rectangleSize);

	// upper right
	modelView = Translate(+7,+7,0);
	glUniformMatrix4fv(modelViewUniform, 1, GL_TRUE, modelView);
	GLuint indices[6] = {0,1,2,3,4,0};
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);
	
	//lower left
	modelView = Translate(-7,-7,0);
	glUniformMatrix4fv(modelViewUniform, 1, GL_TRUE, modelView);
	GLuint indices2[6] = {0,1,5,3,4,2};
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices2);

	//lower right
	modelView = Translate(+7,-7,0);
	glUniformMatrix4fv(modelViewUniform, 1, GL_TRUE, modelView);
	GLuint indices3[9] = {0,1,5,3,4,5,1,2,3};
	glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, indices3);

	glutSwapBuffers();

	Angel::CheckError();
}

void loadGeometry() {
	vec3 color(1.0f, 1.0f, 0.0f);
	Vertex rectangleData[rectangleSize] = {
        { vec2( 0.0,  0.0 ), color },
        { vec2( 5.0, -5.0 ), color },
        { vec2( 5.0,  0.0 ), color },
        { vec2( 0.0,  5.0 ), color },
		{ vec2(-5.0,  0.0 ), color },
		{ vec2(-5.0, -5.0 ), color }
    };
	shapeVertexArrayBuffer = loadBufferData(rectangleData, rectangleSize);
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
    glEnableVertexAttribArray(colorAttribute);
    glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)0);
    glVertexAttribPointer(colorAttribute  , 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)sizeof(vec2));

	return vertexArrayObject;
}

void loadShader(){
	shaderProgram = InitShader("color-shader.vert",  "color-shader.frag", "fragColor");
	projectionUniform = glGetUniformLocation(shaderProgram, "projection");
	if (projectionUniform == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'projection' uniform."<<endl;
	}
	modelViewUniform = glGetUniformLocation(shaderProgram, "modelView");
	if (modelViewUniform == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'modelView' uniform."<<endl;
	}
	colorAttribute = glGetAttribLocation(shaderProgram, "color");
	if (colorAttribute == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'color' attribute." << endl;
	}
	positionAttribute = glGetAttribLocation(shaderProgram, "position");
	if (positionAttribute == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'position' attribute." << endl;
	}
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
	glutCreateWindow("02561-01-03");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);

	Angel::InitOpenGL();

	loadShader();
    loadGeometry();

	Angel::CheckError();

	glutMainLoop();
}