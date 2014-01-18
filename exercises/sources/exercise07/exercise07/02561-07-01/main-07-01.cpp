// 02561-07-01
#include <iostream>
#include <string>
#include <cmath>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Angel.h"
#include "Axis.h"

using namespace std;
using namespace Angel;

int WINDOW_WIDTH = 500;
int WINDOW_HEIGHT = 500;
const int FRAMERATE_MS = 16; /* 60 fps */

GLuint shaderProgram;
GLuint projectionUniform,
	modelViewUniform,
	colorUniform;
GLuint positionAttribute;
GLuint unitCubeVertexArrayObject;

vec3 eyePointSpericalCoordinate(20,2*M_PI, 0.5*M_PI); // radius, altitude, azimuth
vec4 eyePoint(0,12,0,0);

vec4 lightPos(10,2,0,1);
vec4 initialLights(10,2,0,1);
bool usePerspectiveShadow = true;
bool pauseLight = false;
int howFar=10000;
Axis axis;

struct Vertex {
    vec4 position;
};

void loadShader();
void display();
GLuint loadBufferData(Vertex* vertices, int vertexCount);

void buildUnitCube() {
	const int cubeSize = 8;
	Vertex cubeData[cubeSize] = {
        { vec4( 0.5,  0.5,  0.5, 1.0 ) },
        { vec4( 0.5, -0.5,  0.5, 1.0 ) },
        { vec4( 0.5,  0.5, -0.5, 1.0 ) },
        { vec4( 0.5, -0.5, -0.5, 1.0 ) },
		{ vec4(-0.5,  0.5, -0.5, 1.0 ) },
		{ vec4(-0.5, -0.5, -0.5, 1.0 ) },
		{ vec4(-0.5,  0.5,  0.5, 1.0 ) },
		{ vec4(-0.5, -0.5,  0.5, 1.0 ) }
    };
	unitCubeVertexArrayObject = loadBufferData(cubeData, cubeSize);
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

void drawScene(mat4 view){
	mat4 model;
	mat4 modelView = view * model;
	
	glUniformMatrix4fv(modelViewUniform, 1, GL_TRUE, modelView);
	drawSolidUnitCube();
}

void drawLightSource(mat4 view){
	vec4 color(1.0, 1.0, 0.0, 1.0);
	glUniform4fv(colorUniform, 1, color);

	mat4 model = Translate(lightPos);
	mat4 modelView = view * model;
	
	glUniformMatrix4fv(modelViewUniform, 1, GL_TRUE, modelView);
	drawSolidUnitCube();
}
GLfloat shadowDistance = -4;
mat4 createShadowProjectionPointLight(){
	mat4 m;

	m[3][0] = -1.0/(lightPos[0]-shadowDistance);
	m[3][3] = 0;
 	mat4 shadow=Translate(lightPos[0],lightPos[1], lightPos[2])*m*Translate(-lightPos[0],-lightPos[1],-lightPos[2]);
	return shadow;
}

mat4 createShadowProjectionDirectionalLight() {
	mat4 m;
	vec4 newlightPos=howFar*lightPos;//drag the light source far far away without changing its direction. 

	m[3][0] = -1.0/(newlightPos[0]-shadowDistance);
	m[3][3] = 0;
 	mat4 shadow=Translate(newlightPos[0],newlightPos[1], newlightPos[2])*
			m*Translate(-newlightPos[0],-newlightPos[1],-newlightPos[2]);
	return shadow;	 
}

void display() {	
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
    glUseProgram(shaderProgram);
	
	mat4 projection = Perspective(45,WINDOW_WIDTH/(float)WINDOW_HEIGHT, 0.1, 1000.);
	glUniformMatrix4fv(projectionUniform, 1, GL_TRUE, projection);

	mat4 view = LookAt(eyePoint,vec4(0,0,0,0), vec4(0,1,0,0));

	// draw shadow
	mat4 shadowProjection;
	if (usePerspectiveShadow){
		shadowProjection = createShadowProjectionPointLight();
	} else {
		shadowProjection = createShadowProjectionDirectionalLight();
	}
	vec4 color(0.0, 0.0, 0.0, 1.0);
	glUniform4fv(colorUniform, 1, color);
	drawScene(view * shadowProjection);

	// draw normal
	color = vec4(1.0, 0.0, 0.0, 1.0);
	glUniform4fv(colorUniform, 1, color);
	drawScene(view);
	
	drawLightSource(view);

	axis.renderWorldAxis(projection, eyePoint,vec4(0,0,0,0), vec4(0,1,0,0),WINDOW_WIDTH, WINDOW_HEIGHT);

	glutSwapBuffers();

	Angel::CheckError();
}

void reshape(int W, int H) {
    WINDOW_WIDTH = W;
	WINDOW_HEIGHT = H;
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void updateEyepoint(){
	eyePoint[0] = eyePointSpericalCoordinate[0]*sin(eyePointSpericalCoordinate[1])*cos(eyePointSpericalCoordinate[2]);
	eyePoint[2] = eyePointSpericalCoordinate[0]*sin(eyePointSpericalCoordinate[1])*sin(eyePointSpericalCoordinate[2]);
	eyePoint[1] = eyePointSpericalCoordinate[0]*cos(eyePointSpericalCoordinate[1]);
}

void update (float timedelta){
	static float time = 0;
	if (!pauseLight){
		time += timedelta;
	}

	float lightMotionRadius = 5;
	lightPos[1] = initialLights[1]+ sin(time*0.5) * lightMotionRadius;
	lightPos[2] = initialLights[2]+ cos(time*0.5) * lightMotionRadius;
}


void mouseMotion(int x, int y){
	eyePointSpericalCoordinate[1] = y*0.008f;
	eyePointSpericalCoordinate[2] = x*0.008f;
	
	updateEyepoint();
}

// handle timer loop
// responsible for updating the model (update() function) and redisplaying the result)
void timerEvent(int v) {
	static int lastUpdate = 0;
	int time = glutGet(GLUT_ELAPSED_TIME);
	int deltaTimeMs = time-lastUpdate;
	lastUpdate = time;

	float timeDelta = deltaTimeMs/1000.0f;
	update(timeDelta);
	glutPostRedisplay();

	glutTimerFunc(FRAMERATE_MS, &timerEvent, 0);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;
	case 'p':
		pauseLight = !pauseLight;
		if (pauseLight){
			cout << "Pause light motion" << endl;
		} else {
			cout << "Resume light motion" << endl;
		}
		break;
	case 't':
		usePerspectiveShadow = !usePerspectiveShadow;
		if (usePerspectiveShadow){
			cout << "Perspective shadow projection" << endl;
		} else {
			cout << "Orthogonale shadow projection" << endl;
		}
		glutPostRedisplay();
		break;
	case 'z':
		shadowDistance--;
		break;
	case 'x':
		shadowDistance++;
		break;
	case 'a':
		howFar*=2;
		break;
	case 's':
		howFar/=2;
		break;
	case 'd':
		if(shadowDistance==-4)
			shadowDistance=-8;
		else
			shadowDistance=-4;
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

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
	glutCreateWindow("02561-07-01");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutTimerFunc(FRAMERATE_MS, timerEvent, 0);
	glutMotionFunc(mouseMotion);
	glutPassiveMotionFunc(mouseMotion);
	glutKeyboardFunc(keyboard);
	glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);

	Angel::InitOpenGL();
	
	cout <<"P pause/resume light motion"<<endl;
	cout <<"T to toggle shadow projection type"<<endl;
	
	loadShader();
    buildUnitCube();
	updateEyepoint();

	glEnable(GL_DEPTH_TEST);

	Angel::CheckError();

	glutMainLoop();
}