#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Angel.h"
#include "select.h"

using namespace std;
using namespace Angel;

mat4 projection;

bool renderPoints = true;

GLuint pointShader;
GLuint pointShaderModelViewUniform, 
	pointShaderProjectionUniform,
	pointShaderColorUniform;
GLuint pointVertexArrayObject;

GLuint bezierCurveShader;
GLuint bezierCurveShaderProjectionUniform,
	bezierCurveShaderModelViewUniform,
	bezierCurveShaderCurveMatrixUniform;
GLuint bezierCurveVertexArrayObject;
const int bezierCurveResolution = 1024; // number of points on the line


SelectBuffer *selectBuffer = NULL;

int selectedPoint = -1;

int WINDOWS_HEIGHT;
int WINDOWS_WIDTH;

const int numberOfControlPoints = 4;
vec4 controlPoints[numberOfControlPoints] = {
	vec4(50,50,0,1),
	vec4(100,200,0,1),
	vec4(150,150,0,1),
	vec4(250,50,0,1)
};

void renderScene(bool select){
	mat4 modelView = LookAt(vec4(0,0,1,1), vec4(0,0,0,1), vec4(0,1,0,0));

	// render curve
	if (select == false){ // only when not select
		glUseProgram(bezierCurveShader);
		glUniformMatrix4fv(bezierCurveShaderModelViewUniform, 1, GL_TRUE, modelView);
		glUniformMatrix4fv(bezierCurveShaderProjectionUniform, 1, GL_TRUE, projection);
		mat4 curveMatrix;
		// todo compute curveMatrix
		mat4 M_B(vec4(1,0,0,0), vec4(-3, 3, 0, 0),  vec4(3, -6, 3, 0),  vec4(-1, 3, -3, 1) );
		
		mat4 P(controlPoints[0], controlPoints[1],controlPoints[2],controlPoints[3] ); 
		
		curveMatrix= M_B * P ;
		if (bezierCurveShaderCurveMatrixUniform != GL_INVALID_INDEX) { // only set matrix when it exist and is used
			glUniformMatrix4fv(bezierCurveShaderCurveMatrixUniform, 1, GL_TRUE, curveMatrix);
		}
		glBindVertexArray(bezierCurveVertexArrayObject);
		glDrawArrays(GL_LINE_STRIP, 0,bezierCurveResolution);
	}

	// render points
	glUseProgram(pointShader);
	glUniformMatrix4fv(pointShaderModelViewUniform, 1, GL_TRUE, modelView);
	glUniformMatrix4fv(pointShaderProjectionUniform, 1, GL_TRUE, projection);
	glBindVertexArray(pointVertexArrayObject);
	selectBuffer->setColorUniform(pointShaderColorUniform);
	for (int i=0;i<numberOfControlPoints;i++){
		glUniformMatrix4fv(pointShaderModelViewUniform, 1, GL_TRUE, modelView * Translate(controlPoints[i]));
		if (select){
			selectBuffer->setId(i);
		} else {
			vec4 color (0,0,0,1);
			if (i == selectedPoint){
				color = vec4(1,0,0,1); 
			}
			glUniform4fv(pointShaderColorUniform,1,color);
		}
		glDrawArrays(GL_POINTS, 0,1);
	}
}

void display() {	
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);	

	renderScene(false);	
	
	glutSwapBuffers();
		
	Angel::CheckError();

}

void reshape(int w, int h) {
	WINDOWS_HEIGHT = h;
	WINDOWS_WIDTH = w;
	projection = Ortho2D (0, w, 0, h);
	glViewport(0, 0, w, h);
	if (selectBuffer != NULL){
		delete selectBuffer;
	}
	selectBuffer = new SelectBuffer(w,h);
}

void mouse(int button, int state, int x, int y){
	if (state == GLUT_DOWN){
		selectBuffer->bind();
		renderScene(true);
		selectedPoint = selectBuffer->getId(x, WINDOWS_HEIGHT-y);
		selectBuffer->release();
	} else {
		selectedPoint = -1; // on mouse release then deselect
	}
	glutPostRedisplay();
}

void mouseMotion(int x, int y){
	if (selectedPoint != -1){
		controlPoints[selectedPoint].x = x;
		controlPoints[selectedPoint].y = WINDOWS_HEIGHT-y;
		glutPostRedisplay();
	}
}

void initBezierCurve(){
	bezierCurveShader = InitShader("bezier.vert",  "bezier.frag", "fragColor");
	bezierCurveShaderProjectionUniform = glGetUniformLocation(bezierCurveShader, "projection");
	if (bezierCurveShaderProjectionUniform == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'projection' uniform."<<endl;
	}
	bezierCurveShaderModelViewUniform = glGetUniformLocation(bezierCurveShader, "modelView");
	if (bezierCurveShaderModelViewUniform == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'modelView' uniform."<<endl;
	}
	bezierCurveShaderCurveMatrixUniform = glGetUniformLocation(bezierCurveShader, "curveMatrix");
	if (bezierCurveShaderCurveMatrixUniform == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'curveMatrix' uniform."<<endl;
	}
	GLuint uAttribute = glGetAttribLocation(bezierCurveShader, "u");
	if (uAttribute == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'u' attribute." << endl;
	}
	// create point vertex array object
	glGenVertexArrays(1, &bezierCurveVertexArrayObject);
	glBindVertexArray(bezierCurveVertexArrayObject);
	GLuint  vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	// create u between 0.0 and 1.0
	float u[bezierCurveResolution];
	for (int i=0;i<bezierCurveResolution;i++){
		u[i] = i/(bezierCurveResolution-1.0f);
	}
	glBufferData(GL_ARRAY_BUFFER, bezierCurveResolution * sizeof(float), u, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(uAttribute);
	glVertexAttribPointer(uAttribute, 1, GL_FLOAT, GL_FALSE, sizeof(float), (const GLvoid *)0);
	glBindVertexArray(0);

	// set pointSize
	glPointSize(8);
}

void initPoints(){
	// create point shader
	pointShader = InitShader("point.vert",  "point.frag", "fragColor");
	pointShaderProjectionUniform = glGetUniformLocation(pointShader, "projection");
	if (pointShaderProjectionUniform == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'projection' uniform."<<endl;
	}
	pointShaderModelViewUniform = glGetUniformLocation(pointShader, "modelView");
	if (pointShaderModelViewUniform == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'modelView' uniform."<<endl;
	}
	pointShaderColorUniform = glGetUniformLocation(pointShader, "color");
	if (pointShaderColorUniform == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'color' uniform."<<endl;
	}
	GLuint uAttribute = glGetAttribLocation(pointShader, "u");
	if (uAttribute == GL_INVALID_INDEX) {
		cerr << "Shader did not contain the 'u' attribute." << endl;
	}
	// create point vertex array object
	glGenVertexArrays(1, &pointVertexArrayObject);
	glBindVertexArray(pointVertexArrayObject);
	GLuint  vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	float data[1] = {1};
	glBufferData(GL_ARRAY_BUFFER, 1 * sizeof(float), data, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(uAttribute);
	glVertexAttribPointer(uAttribute, 1, GL_FLOAT, GL_FALSE, sizeof(float), (const GLvoid *)0);
	glBindVertexArray(0);

	// set pointSize
	glPointSize(8);
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
	glutCreateWindow("02561-11-01");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
	glutReshapeWindow(500, 500);

	Angel::InitOpenGL();

	initPoints();
	initBezierCurve();
	glutFullScreen(); // toggle fullscreen

	Angel::CheckError();

	glutMainLoop();
}
