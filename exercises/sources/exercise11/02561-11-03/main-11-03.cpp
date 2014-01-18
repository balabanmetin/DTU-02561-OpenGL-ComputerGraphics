#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Angel.h"
#include "NURBSCurve.h"
#include "NURBSRenderer.h"

using namespace std;
using namespace Angel;

mat4 projection;

NURBSRenderer *lineRenderer = NULL;
NURBSRenderer *bezierRenderer = NULL;
NURBSRenderer *uniformBSplineRenderer = NULL;
NURBSRenderer *nonUniformBSplineRenderer = NULL;
NURBSRenderer *nurbsRenderer = NULL;
NURBSRenderer *current = NULL;
bool renderPoints = true;

vec3 controlPoints [7] = {
	vec3(-10.,0.,0.),
	vec3(-10.,5.,0.),
	vec3(-5.,5.,0.),
	vec3(0.,10.,0.),
	vec3(5.,5.,0.),
	vec3(10.,5.,0.),
	vec3(10.,0.,0.)};

void display() {	
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);	
	
	mat4 modelView = LookAt(vec4(0,5,1,1), vec4(0,5,0,1), vec4(0,1,0,0)); // identity modelView matrix
	
	if (current != NULL){
		current->render(projection, modelView);
	}
	
	if (renderPoints){
		lineRenderer->renderControlPoints(projection, modelView);
	}

	glutSwapBuffers();
	Angel::CheckError();
}

void reshape(int w, int h) {
	if (w <= h)
      projection = Ortho2D (-12.0, 12.0, -12.0*(GLfloat)h/(GLfloat)w, 
               12.0*(GLfloat)h/(GLfloat)w);
   else
      projection = Ortho2D(-12.0*(GLfloat)w/(GLfloat)h, 
               12.0*(GLfloat)w/(GLfloat)h, -12.0, 12.0);
    glViewport(0, 0, w, h);
}

void setupLines(){
	const int numberOfControlPoints = 7;
	NURBSCurve *line = new NURBSCurve(numberOfControlPoints);
	for (int i=0;i<numberOfControlPoints;i++){
		line->setControlPoint(i, controlPoints[i]);
	}
	const int numberOfKnots = 8;
	float knotVector[numberOfKnots] = {0,1,2,3,4,5,6,7};
	line->setKnotVector(numberOfKnots, knotVector);
	cout << "Piecewise line curve: degree "<< line->getDegree() << " Order " << line->getOrder() << endl;
	lineRenderer = new NURBSRenderer(line);
}

void setupBezier(){
	// todo implement
	// bezierRenderer = ...

	const int numberOfControlPoints = 4;

	NURBSCurve *line = new NURBSCurve(numberOfControlPoints);
	for (int i=0;i<numberOfControlPoints;i++){
		line->setControlPoint(i, controlPoints[i]);
	}
	const int numberOfKnots = 8;
	float knotVector[numberOfKnots] = {0,0,0,0,1,1,1,1};
	line->setKnotVector(numberOfKnots, knotVector);
	cout << "Piecewise line curve: degree "<< line->getDegree() << " Order " << line->getOrder() << endl;
	bezierRenderer = new NURBSRenderer(line);

}

void setupUniformBSpline(){
	 //todo implement
	 //uniformBSplineRenderer = ...

	const int numberOfControlPoints = 7;
	NURBSCurve *line = new NURBSCurve(numberOfControlPoints);
	for (int i=0;i<numberOfControlPoints;i++){
		line->setControlPoint(i, controlPoints[i]);
	}
	const int numberOfKnots = 11;
	float knotVector[numberOfKnots] = {0,1,2,3,4,5,6,7,8,9,10};
	line->setKnotVector(numberOfKnots, knotVector);
	cout << "Piecewise line curve: degree "<< line->getDegree() << " Order " << line->getOrder() << endl;
	uniformBSplineRenderer = new NURBSRenderer(line);
}

void setupNonUniformBSpline(){
	// todo implement
	// nonUniformBSplineRenderer = ...
	const int numberOfControlPoints = 7;
	NURBSCurve *line = new NURBSCurve(numberOfControlPoints);
	for (int i=0;i<numberOfControlPoints;i++){
		line->setControlPoint(i, controlPoints[i]);
	}
	const int numberOfKnots = 11;
	float knotVector[numberOfKnots] = {0,0,0,0,1,2,3,4,4,4,4};
	line->setKnotVector(numberOfKnots, knotVector);
	cout << "Piecewise line curve: degree "<< line->getDegree() << " Order " << line->getOrder() << endl;
	nonUniformBSplineRenderer = new NURBSRenderer(line);
}

void setupNURBS(){
	// todo implement
	// nurbsRenderer = ...
	const int numberOfControlPoints = 7;
	NURBSCurve *line = new NURBSCurve(numberOfControlPoints);
	for (int i=0;i<numberOfControlPoints;i++){
		vec4 controlPointWithWeights(controlPoints[i]); // weights are 4,3,2,1,2,3,4
		controlPointWithWeights.w = 1 + abs(3-i);
		line->setControlPoint(i, controlPointWithWeights);
	}
	const int numberOfKnots = 11;
	float knotVector[numberOfKnots] = {0,0,0,0,1,1,1,2,2,2,2};
	line->setKnotVector(numberOfKnots, knotVector);
	cout << "Piecewise line curve: degree "<< line->getDegree() << " Order " << line->getOrder() << endl;
	nurbsRenderer = new NURBSRenderer(line);
}

void keyboard(unsigned char c, int x, int y){
	switch (c){
	case '1':
		current = lineRenderer;
		glutSetWindowTitle("Lines");
		break;
	case '2':
		current = bezierRenderer;
		glutSetWindowTitle("Bezier");
		break;
	case '3':
		current = uniformBSplineRenderer;
		glutSetWindowTitle("UniformBSpline");
		break;
		break;
	case '4':
		current = nonUniformBSplineRenderer;
		glutSetWindowTitle("NonUniformBSpline");
		break;
	case '5':
		current = nurbsRenderer;
		glutSetWindowTitle("NURBS");
		break;
	case 'p':
	case 'P':
		renderPoints = !renderPoints;
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

	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_3_2_CORE_PROFILE);
	glutCreateWindow("02561-11-03");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutReshapeWindow(500, 500);

	Angel::InitOpenGL();
		
	setupLines();
	setupBezier();
	setupUniformBSpline();
	setupNonUniformBSpline();
	setupNURBS();

	keyboard('1', 0, 0);

	Angel::CheckError();


	glutMainLoop();
}
