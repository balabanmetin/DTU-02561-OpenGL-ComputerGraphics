// 02561-06-05
#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Angel.h"
#include "TextureLoader.h"

using namespace std;
using namespace Angel;

int WINDOW_WIDTH = 500;
int WINDOW_HEIGHT = 500;

float delta=0;
unsigned int width, height;

GLuint shaderProgram;
GLuint projectionUniform,
	modelViewUniform,
	textureUniform,
	textureTransUniform;
GLuint positionAttribute,
	textureCoordAttribute;
GLuint polygonVertexArrayObject;
GLuint textureId;

const int polygonSize = 4;

struct Vertex {
    vec4 position;
	vec2 textureCoord;
};

void loadShader();
void display();
GLuint loadBufferData(Vertex* vertices, int vertexCount);

void buildPolygon() {
	Vertex polygonData[polygonSize] = {
		{vec4(0., 1, 1, 1.0), vec2(1,1)},
		{vec4(0.,-1, 1, 1.0), vec2(0,1)}, 
		{vec4(0.,-1,-1, 1.0), vec2(0,0)},
		{vec4(0., 1,-1, 1.0), vec2(1,0)}
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
	glEnableVertexAttribArray(textureCoordAttribute);
    glVertexAttribPointer(positionAttribute, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)0);
    glVertexAttribPointer(textureCoordAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)(sizeof(vec4)));

	return vertexArrayObject;
}

void loadShader(){
	shaderProgram = InitShader("tex-shader.vert",  "tex-shader.frag", "fragColor");
	projectionUniform = glGetUniformLocation(shaderProgram, "projection");
	if (projectionUniform == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'projection' uniform."<<endl;
	}
	modelViewUniform = glGetUniformLocation(shaderProgram, "modelView");
	if (modelViewUniform == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'modelView' uniform."<<endl;
	}
	textureUniform = glGetUniformLocation(shaderProgram, "textureMap");
	if (textureUniform == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'texture' uniform."<<endl;
	}
	textureTransUniform = glGetUniformLocation(shaderProgram, "textureTrans");
	if (textureTransUniform == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'textureTrans' uniform."<<endl;
	}
	positionAttribute = glGetAttribLocation(shaderProgram, "position");
	if (positionAttribute == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'position' attribute." << endl;
	}
	textureCoordAttribute = glGetAttribLocation(shaderProgram, "textureCoord");
	if (textureCoordAttribute == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'textureCoord' attribute." << endl;
	}
}

void drawPolygon() {
	glBindVertexArray(polygonVertexArrayObject);
	glDrawArrays(GL_TRIANGLE_FAN, 0, polygonSize);	
}

void display() {	
    glClearColor(1.,1.,1.,1.);
    glClear(GL_COLOR_BUFFER_BIT);
	
    glUseProgram(shaderProgram);

	mat4 projection = Perspective(90., (GLfloat)WINDOW_WIDTH / (GLfloat) WINDOW_HEIGHT, .1, 50.);
	glUniformMatrix4fv(projectionUniform, 1, GL_TRUE, projection);

	vec4 eye(2., 0., 0.,1);
	vec4 at( 0., 0., 0.,1);
	vec4 up( 0., 1., 0.,0);
	mat4 modelView = LookAt(eye, at, up);
	glUniformMatrix4fv(modelViewUniform, 1, GL_TRUE, modelView);
	
	mat4 textureTrans=Translate(0.5, 0.5, 0.0) * RotateZ(delta)*Translate(-0.5, -0.5, 0.0);
	glUniformMatrix4fv(textureTransUniform, 1, GL_TRUE, textureTrans);
	
	// bind texture
	glActiveTexture(GL_TEXTURE0); // activate texture in slot 0
	glBindTexture(GL_TEXTURE_2D, textureId); // assign texture to slot 0
	glUniform1i(textureUniform, 0); // assign slot 0 to uniform
	
	drawPolygon();
	
	glutSwapBuffers();

	Angel::CheckError();
}

void reshape(int W, int H) {
    WINDOW_WIDTH = W;
	WINDOW_HEIGHT = H;
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

GLuint loadTexture(const char* name){

    unsigned char * data = loadBMPRaw(name, width, height);
   	// Create one OpenGL texture
 	GLuint textureId;
 	glGenTextures(1, &textureId);
 
 	// "Bind" the newly created texture : all future texture functions will modify this texture
 	glBindTexture(GL_TEXTURE_2D, textureId);
 
 	// Give the image to OpenGL
 	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
 
 	// Poor filtering, or ...
 	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
 	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
 
 	// ... nice trilinear filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
 	glGenerateMipmap(GL_TEXTURE_2D);
    delete[] data;
	return textureId;
}
void idle()
{
	delta+=0.05;
	glutPostRedisplay();
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
	glutCreateWindow("02561-06-05");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutIdleFunc(idle);

	Angel::InitOpenGL();

	loadShader();
    buildPolygon();
	
	textureId = loadTexture("earth.bmp");

	Angel::CheckError();

	glutMainLoop();
}