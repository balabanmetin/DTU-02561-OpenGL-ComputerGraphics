// 02561-06-03
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
	textureUniform;
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
		{vec4( 1., -1, -1, 1.0), vec2(1,1)},
		{vec4( 1., -1,  1, 1.0), vec2(0,1)}, 
		{vec4(-20., -1,  1, 1.0), vec2(0,-19)},
		{vec4(-20., -1, -1, 1.0), vec2(1,-19)}
	};
	polygonVertexArrayObject = loadBufferData(polygonData, polygonSize);
}

void buildProceduralTexture() {
	const int checkImageWidth = 64;
	const int checkImageHeight = 64;
	GLubyte checkImage[checkImageWidth][checkImageHeight][3];
	
	// create image data
	for (int i = 0; i < checkImageWidth; i++) {
		for (int j = 0; j < checkImageHeight; j++) {
			int c = ((((i&0x8)==0)^((j&0x8))==0))*255;
			checkImage[i][j][0] = (GLubyte) c;
			checkImage[i][j][1] = (GLubyte) c;
			checkImage[i][j][2] = (GLubyte) c;
		}
    } // CREATES a CHESS BOARD


	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, checkImageWidth, checkImageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, &checkImage[0][0][0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
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

	vec4 eye(2.0, 0., 0.,1);
	vec4 at( 0., 0., 0.,1);
	vec4 up( 0., 1., 0.,0);
	mat4 modelView = LookAt(eye, at, up);
	glUniformMatrix4fv(modelViewUniform, 1, GL_TRUE, modelView);
	
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

void keyboard(unsigned char c, int x, int y){
	glBindTexture(GL_TEXTURE_2D, textureId);

	switch (c){
	case '1':
		glutSetWindowTitle("Mag: Linear");
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	case '2':
		glutSetWindowTitle("Mag: Nearest");
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;
	case '3':
		glutSetWindowTitle("Min: Nearest mipmap nearest");
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		break;
	case '4':
		glutSetWindowTitle("Min: Nearest mipmap linear");
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		break;
	case '5':
		glutSetWindowTitle("Min: Nearest");
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		break;
	case '6':
		glutSetWindowTitle("Min: Linear");
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		break;
	case '7':
		glutSetWindowTitle("Min: Linear mipmap nearest");
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		break;
	case '8':
		glutSetWindowTitle("Min: Linear mipmap Linear");
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		break;
		// todo implement for rest of combinations (for both minfication and magnification filter)
	}
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
	glutCreateWindow("02561-06-03");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);

	Angel::InitOpenGL();

	loadShader();
    buildPolygon();
	buildProceduralTexture();

	Angel::CheckError();

	glutMainLoop();
}