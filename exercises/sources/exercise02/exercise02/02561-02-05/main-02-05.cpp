// 02561-02-05

#include <iostream>
#include <string>


#include "Angel.h"
#include "ObjLoader.h"

using namespace std;
using namespace Angel;

int WINDOW_WIDTH = 600;
int WINDOW_HEIGHT = 600;

GLuint shaderProgram;
GLuint colorAttribute, positionAttribute;
GLuint projectionUniform,
    modelViewUniform;
GLuint vertexArrayObject;

int numberOfIndices = 0;

mat4 projection = Perspective(60, 1, 0.1, 100);
mat4 view = LookAt(vec4(8, 8, 8,1), vec4(0,0,0,1), vec4(0,1,0,0));
mat4 model = mat4(1.0f);

// forward declaration
void loadShader();
void display();
void loadBufferData();
void reshape(int W, int H);


void key1(){
    view = LookAt(vec4(-.5, 1, 6,1), vec4(-2,1,0,1), vec4(0,1,0,0));
}

void key2(){
    // todo replace with series of matrix transformations (using Translate, Rotate, Scale functions)
    view = LookAt(vec4(-.5, 1, 6,1), vec4(-2,1,0,1), vec4(0,1,0,0));
	view = RotateY(-atan(0.25)/DegreesToRadians)*Translate(vec3(+.5, -1, -6));
}

void key3(){
    view = RotateY(-120) * Translate(-4, -1, -1);
}

void key4(){
    // todo replace with lookAt function
	view = LookAt(vec3(4, 1, 1), vec3(4, 1, 1) + vec3(-sqrt(3), 0, 1), vec3(0,1,0));
}

void key5(){
    view = LookAt(vec4(-1, 1, 9,1), vec4(-1,1,0,1), vec4(0,1,0,0));
	cout <<LookAt(vec4(-1, 1, 9,1), vec4(-1,1,0,1), vec4(0,1,0,0)) << endl;
}

void key6(){
    // todo replace with full hardcoded matrix using the following mat4 contructor
    //    mat4( GLfloat m00, GLfloat m10, GLfloat m20, GLfloat m30,
    //      GLfloat m01, GLfloat m11, GLfloat m21, GLfloat m31,
    //      GLfloat m02, GLfloat m12, GLfloat m22, GLfloat m32,
    //      GLfloat m03, GLfloat m13, GLfloat m23, GLfloat m33 )
    //view = LookAt(vec4(-1, 1, 9,1), vec4(-1,1,0,1), vec4(0,1,0,0));
	view = mat4 ( 1,0,0,0,
				  0,1,0,0,
				  0,0,1,0,
				  1,-1,-9,1); //column first
}

void key0(){
    view = LookAt(vec4(8, 8, 8,1), vec4(0,0,0,1), vec4(0,1,0,0));
}


void display() {	
	glClearColor(0.4, 0.4, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(shaderProgram);

	glUniformMatrix4fv(projectionUniform, 1, GL_TRUE, projection);
    
	mat4 modelView = view * model;
    glUniformMatrix4fv(modelViewUniform, 1, GL_TRUE, modelView);
    
	glBindVertexArray(vertexArrayObject);
    
    glDrawElements(GL_TRIANGLES, numberOfIndices, GL_UNSIGNED_INT, (void*)0);
    
	glutSwapBuffers();

	Angel::CheckError();
}

void reshape(int W, int H) {
	WINDOW_WIDTH = W;
	WINDOW_HEIGHT = H;
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void loadBufferData() {
    std::vector<vec3> outPositions;
    std::vector<int> outIndices;
    std::vector<vec3> outNormal;
    std::vector<vec2> outUv;
    loadObject("house.obj", outPositions, outIndices, outNormal, outUv);
    glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);
	GLuint  vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, outPositions.size() * sizeof(vec3) * 2, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, outPositions.size() * sizeof(vec3), outPositions[0]);
    glBufferSubData(GL_ARRAY_BUFFER, outPositions.size() * sizeof(vec3), outPositions.size() * sizeof(vec3), outNormal[0]);
	
    GLuint  elementIndexBuffer;

    glGenBuffers(1, &elementIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,  elementIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*outIndices.size(), &(outIndices[0]), GL_STATIC_DRAW);
	
    numberOfIndices = (int)outIndices.size();
    
	glEnableVertexAttribArray(positionAttribute);
	glEnableVertexAttribArray(colorAttribute);
	glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)0);
	glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)(outPositions.size() * sizeof(vec3)));
}

void loadShader() {
	shaderProgram = InitShader("color-shader.vert",  "color-shader.frag", "fragColor");
	colorAttribute = glGetAttribLocation(shaderProgram, "color");
	positionAttribute = glGetAttribLocation(shaderProgram, "position");
    projectionUniform = glGetUniformLocation(shaderProgram, "projection");
	if (projectionUniform == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'projection' uniform."<<endl;
	}
	modelViewUniform = glGetUniformLocation(shaderProgram, "modelView");
	if (modelViewUniform == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'modelView' uniform."<<endl;
	}
}

void onKeyPress(unsigned char key,
                int x, int y){
    switch (key) {
        case '1':
            key1();
            break;
        case '2':
            key2();
            break;
        case '3':
            key3();
            break;
        case '4':
            key4();
            break;
        case '5':
            key5();
            break;
        case '6':
            key6();
            break;
        case '0':
            key0();
            break;
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

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
	glutCreateWindow("02561-02-05");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
    glutKeyboardFunc(onKeyPress);
	glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
    glEnable(GL_DEPTH_TEST);
	Angel::InitOpenGL();
		
	loadShader();
	loadBufferData();

	Angel::CheckError();
	glutMainLoop();
}