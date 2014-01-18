// 02561-01-03

#include <iostream>
#include <string>
#include <vector>
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

struct Vertex {
    vec2 position;
    vec3 color;
};

struct Mesh {
	GLuint vertexArrayObject;
	GLuint indexSize;

	void drawMesh(mat4 projection, mat4 modelView){
		glUniformMatrix4fv(modelViewUniform, 1, GL_TRUE, modelView);
		glUniformMatrix4fv(projectionUniform, 1, GL_TRUE, projection);
		glBindVertexArray(vertexArrayObject);
		glDrawArrays(GL_TRIANGLE_FAN, 0, indexSize);
	}
};

vector<Mesh> meshes;

bool enableStencil = false;
bool drawStencil[3] = {true, false, false};
bool drawMesh[3] = {true, true, true};

void loadShader();
void display();
GLuint loadBufferData(Vertex* vertices, int vertexCount);
void reshape(int W, int H);
void loadGeometry();

void setupStencil(mat4& projection, mat4& modelView){
	glClear(GL_STENCIL_BUFFER_BIT);			 // Clear stencil buffer (set values to 0)
	glStencilFunc(GL_ALWAYS, 1, 1);			 //Test always success, value written 1
	glColorMask(false, false, false, false); //Disable writting in color buffer
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); //Stencil & Depth test passes => replace existing value in stencil buffer

	for (int i=0;i<meshes.size();i++){
		if (drawStencil[i]){
			meshes[i].drawMesh(projection, modelView); // render object to stencil
		}
	}

	glColorMask(true, true, true, true);    //Enable writting in color buffer
	glStencilFunc(GL_EQUAL, 1, 1);          //Draw only to color buffer where stencil buffer is 1
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); //Stencil buffer read only
}

void display() {	
	glClearColor(1.0, 0.0, 0.0, 1.0); // red background
    glClear(GL_COLOR_BUFFER_BIT);

	mat4 projection = Ortho2D(-15.0f, 15.0f, -15.0f, 15.0f);
	mat4 modelView;
	
	if (enableStencil){
		glEnable(GL_STENCIL_TEST);  
		setupStencil(projection, modelView);
	}
	
	glUseProgram(shaderProgram);

	for (int i=0;i<meshes.size();i++){
		if (drawMesh[i]){
			meshes[i].drawMesh(projection, modelView);
		}
	}

	glDisable(GL_STENCIL_TEST);
	glutSwapBuffers();
	Angel::CheckError();
}

void loadGeometry() {
	vec3 color(1.0f, 1.0f, 0.0f);
	Vertex rectangleData[3] = {
        { vec2(   0.0,  10.0 ), color},
        { vec2(  10.0, -10.0 ), color},
        { vec2( -10.0, -10.0 ), color}
    };
	Mesh triangle1;
	triangle1.indexSize = 3;
	triangle1.vertexArrayObject = loadBufferData(rectangleData, triangle1.indexSize);

	color = vec3(1.0f, 0.0f, 1.0f);
	Vertex rectangleData2[3] = {
        { vec2(   0.0, -10.0 ), color},
        { vec2(  10.0,  10.0 ), color},
        { vec2( -10.0,  10.0 ), color}
    };
	Mesh triangle2;
	triangle2.indexSize = 3;
	triangle2.vertexArrayObject = loadBufferData(rectangleData2, triangle2.indexSize);

	color = vec3(0.0f, 1.0f, 1.0f);
	Vertex rectangleData3[4] = {
        { vec2( 7.5, -7.5), color},
        { vec2(-7.5, -7.5), color},
        { vec2(-7.5,  7.5), color},
		{ vec2( 7.5,  7.5), color},
    };
	Mesh rectangle;
	rectangle.indexSize = 4;
	rectangle.vertexArrayObject = loadBufferData(rectangleData3, rectangle.indexSize);

	meshes.push_back(triangle1);
	meshes.push_back(triangle2);
	meshes.push_back(rectangle);
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
	modelViewUniform = glGetUniformLocation(shaderProgram, "modelView");
	colorAttribute = glGetAttribLocation(shaderProgram, "color");
	positionAttribute = glGetAttribLocation(shaderProgram, "position");
}

void reshape(int W, int H) {
    WINDOW_WIDTH = W;
	WINDOW_HEIGHT = H;
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void keyboard(unsigned char c, int x, int y){
	switch (c){
	case '1':
		drawMesh[0] = !drawMesh[0];
		cout << "draw mesh 0: "<<drawMesh[0]<<endl;
		break;
	case '2':
		drawMesh[1] = !drawMesh[1];
		cout << "draw mesh 1: "<<drawMesh[1]<<endl;
		break;
	case '3':
		drawMesh[2] = !drawMesh[2];
		cout << "draw mesh 2: "<<drawMesh[2]<<endl;
		break;
	case '4':
		drawStencil[0] = !drawStencil[0];
		cout << "draw stencil 0: "<<drawStencil[0]<<endl;
		break;
	case '5':
		drawStencil[1] = !drawStencil[1];
		cout << "draw stencil 1: "<<drawStencil[1]<<endl;
		break;
	case '6':
		drawStencil[2] = !drawStencil[2];
		cout << "draw stencil 2: "<<drawStencil[2]<<endl;
		break;
	case 's':
	case 'S':
		enableStencil = !enableStencil;
		cout << "Enable stencil "<<enableStencil<<endl;
		break;
	}

	string title = "Stencil ";
	title += enableStencil?"enabled":"false";
	title += " Geometry: ";
	for (int i=0;i<3;i++){
		title += drawMesh[i]?'0'+i:' ';
		title += ' ';
	}
	if (enableStencil){
		title += " Stencil-geometry: ";	
		for (int i=0;i<3;i++){
			title += drawStencil[i]?'0'+i:' ';
			title += ' ';
		}
	}
	glutSetWindowTitle(title.c_str());

	glutPostRedisplay();
}

int main(int argc, char* argv[]) {
	cout << "------------- Stencil test help --------------"<<endl;
	cout << "'s' toggle stencil-test on/off"<<endl;
	cout << "'1'-'3' toggle geometry 1-3"<<endl;
	cout << "'4'-'6' toggle stencil geometry 1-3"<<endl;
    glutInit(&argc, argv);
	glutInitContextVersion(3, 1);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    
	glutSetOption(
        GLUT_ACTION_ON_WINDOW_CLOSE,
        GLUT_ACTION_GLUTMAINLOOP_RETURNS
    );

	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_STENCIL|GLUT_3_2_CORE_PROFILE);
	glutCreateWindow("02561-09-01");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);

	Angel::InitOpenGL();
	
	
	loadShader();
    loadGeometry();

	Angel::CheckError();

	glutMainLoop();
}