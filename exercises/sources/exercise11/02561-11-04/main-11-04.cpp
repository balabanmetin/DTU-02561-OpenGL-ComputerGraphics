#include <cmath>
#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Angel.h"
#include "NURBSSurface.h"
#include "NURBSRenderer.h"

using namespace std;
using namespace Angel;

mat4 projection;

NURBSRenderer *surfaceRenderer = NULL;

bool renderControlPoints = true;
bool renderNormals = false;

bool wireframemode = false;

vec4 lightPosWorldSpace(0,100,0,1);

vec3 eyePointSpericalCoordinate(50,0.25f*float(M_PI), 0.25f*float(M_PI)); // radius, altitude, azimuth
vec4 eyePoint(0,0,0,1);

int width;
int height;

const int numberOfControlPointsU = 7;
const int numberOfControlPointsV = 7;
vec3 controlPoints[numberOfControlPointsU] = {
	vec3(-15,-15,0),
	vec3(-10,-15,.2),
	vec3( -5,-15,.4),
	vec3(  0,-15,.6),
	vec3( +5,-15,.4),
	vec3(+10,-15,.2),
	vec3(+15,-15,0),
};
	
vec3 controlPointsSurface[numberOfControlPointsU][numberOfControlPointsV];

void init_surface()
{
   int u, v;
  
   for (u = 0; u < numberOfControlPointsU; u++){
	   for (v = 0; v < numberOfControlPointsV;v++){
		   controlPointsSurface[u][v].x = controlPoints[u].x;
		   controlPointsSurface[u][v].y = controlPoints[u].y + v*5;
		   controlPointsSurface[u][v].z = controlPoints[u].z + 0.6-0.2*abs(3-v);
	   }
   }

   controlPointsSurface[1][1].z+=15;
   controlPointsSurface[1][2].z+=15;
   controlPointsSurface[2][1].z+=15;
   controlPointsSurface[2][2].z+=15;

   controlPointsSurface[4][4].z+=15;
   controlPointsSurface[4][5].z+=15;
   controlPointsSurface[5][4].z+=15;
   controlPointsSurface[5][5].z+=15;
}    

void display() {	
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	
	mat4 modelView = LookAt(eyePoint, vec4(0,0,0,1), vec4(0,1,0,0)); // identity modelView matrix
	
	vec4 lightPositionEyeSpace =  modelView * lightPosWorldSpace;
	
	surfaceRenderer->render(projection, modelView, lightPositionEyeSpace);
		
	if (renderControlPoints){
		surfaceRenderer->renderControlPoints(projection, modelView);
	}

	if (renderNormals){
		surfaceRenderer->renderNormals(projection, modelView);
	}

	glutSwapBuffers();

	Angel::CheckError();
}

void reshape(int w, int h) {
	width = w;
	height = h;
    projection = Perspective(60, w/float(h), 0.5,80);
    glViewport(0, 0, w, h);
}

void setupSurface(){
	const int numberOfKnotsU = 11;
	const int numberOfKnotsV = 11;
	GLfloat knotsU[numberOfKnotsU] = {0,0,0,0,1,2,3,4,4,4,4}; 
	GLfloat knotsV[numberOfKnotsV] = {0,0,0,0,1,2,3,4,4,4,4};

	NURBSSurface *surfacePatch = new NURBSSurface(numberOfControlPointsU,numberOfControlPointsV);
	for (int i=0;i<numberOfControlPointsU;i++){
		for (int j=0;j<numberOfControlPointsV;j++){
			surfacePatch->setControlPoint(i, j, controlPointsSurface[i][j]);
		}
	}

	surfacePatch->setKnotVectorU(numberOfKnotsU, knotsU);
	surfacePatch->setKnotVectorV(numberOfKnotsV, knotsV);

	surfaceRenderer = new NURBSRenderer(surfacePatch);
}

void keyboard(unsigned char c, int x, int y){
	switch (c){
	case 'p':
		renderControlPoints = !renderControlPoints;
		break;
	case 'w':
		wireframemode = !wireframemode;
		if (wireframemode){
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			surfaceRenderer->setColor(vec4(0,0,0,1));
		} else {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
			surfaceRenderer->setColor(vec4(1,0,0,1));
		}
		break;
	case 'n':
		renderNormals = !renderNormals;
		break;
	}
	glutPostRedisplay();
}

void updateEyepoint(){
	eyePoint[0] = eyePointSpericalCoordinate[0]*sin(eyePointSpericalCoordinate[1])*cos(eyePointSpericalCoordinate[2]);
	eyePoint[2] = eyePointSpericalCoordinate[0]*sin(eyePointSpericalCoordinate[1])*sin(eyePointSpericalCoordinate[2]);
	eyePoint[1] = eyePointSpericalCoordinate[0]*cos(eyePointSpericalCoordinate[1]);
}


void mouseMotion(int x, int y){
	eyePointSpericalCoordinate[1] = y*0.008f;
	eyePointSpericalCoordinate[2] = x*0.008f;
	
	eyePointSpericalCoordinate[1] = min(float(M_PI-0.0001), max(0.001f, eyePointSpericalCoordinate[1]));
	
	updateEyepoint();
	glutPostRedisplay();
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
	glutInitContextVersion(3,1);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    
	glutSetOption(
        GLUT_ACTION_ON_WINDOW_CLOSE,
        GLUT_ACTION_GLUTMAINLOOP_RETURNS
    );

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH|GLUT_3_2_CORE_PROFILE);
	glutCreateWindow("02561-11-04");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMotionFunc(mouseMotion);
	glutKeyboardFunc(keyboard);
	glutReshapeWindow(500, 500);

	Angel::InitOpenGL();
	
	init_surface();
	setupSurface();
	updateEyepoint();

	glEnable(GL_DEPTH_TEST);

	Angel::CheckError();


	glutMainLoop();
}
