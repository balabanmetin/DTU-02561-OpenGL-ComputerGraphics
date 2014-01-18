// 02561-03-01
#include <stdio.h>
#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Angel.h"

#include "vtk_reader.h"

using namespace std;
using namespace Angel;

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 800;

GLuint shaderProgram;
GLuint projectionUniform,
	modelViewUniform,
	blendValueUniform,
	normalExtrusionUniform;
GLuint positionAttribute,
	colorAttribute,
	normalAttribute,
	position2Attribute,
	color2Attribute,
	normal2Attribute;	

GLuint vertexArrayObject,
	vertexBuffer;

struct Vertex {
    vec3 position1;
	vec3 color1;
	vec3 normal1;

	vec3 position2;
	vec3 color2;
	vec3 normal2;
};

vector<unsigned int> indices;

vec2 sphericalCoordinates; // two first components of spherical coordinates (azimuth and elevation)
float dist = 160; // last component of spherical coordinates
vec2 angleOffset;
vec2 mousePos;
float blendValue = 0.0f;
float normalExtrusion = 0.0f;

void loadShader();
void display();

vector<Vertex> interleaveData(vector<float> &position, vector<float> &color ,vector<float> &normal, vector<float> &position2, vector<float> &color2, vector<float> &normal2  ){
	vector<Vertex> interleavedVertexData;
	for (int i=0;i<position.size();i=i+3){
		vec3 positionVec(position[i],position[i+1],position[i+2]);
		vec3 colorVec(color[i],color[i+1],color[i+2]);
		vec3 normalVec(normal[i],normal[i+1],normal[i+2]);

		vec3 positionVec2(position2[i],position2[i+1],position2[i+2]);
		vec3 colorVec2(color2[i],color2[i+1],color2[i+2]);
		vec3 normalVec2(normal[i],normal[i+1],normal[i+2]);

		Vertex v = {
			positionVec,
			colorVec,
			normalVec,
			positionVec2,
			colorVec2,
			normalVec2
		};

		interleavedVertexData.push_back(v);
	}
	return interleavedVertexData;
}

void uploadData(vector<Vertex> &interleavedVertexData){
	glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);
    
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, interleavedVertexData.size() * sizeof(Vertex), interleavedVertexData[0].position1, GL_STATIC_DRAW);
    
	GLuint stride = sizeof(Vertex);
	glEnableVertexAttribArray(positionAttribute);
	glEnableVertexAttribArray(colorAttribute);
	glEnableVertexAttribArray(normalAttribute);

	glEnableVertexAttribArray(position2Attribute);
	glEnableVertexAttribArray(color2Attribute);
	glEnableVertexAttribArray(normal2Attribute);

	glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid *)(0));
	glVertexAttribPointer(colorAttribute , 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid *)(sizeof(vec3)));
	glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid *)(sizeof(vec3)*2));

	glVertexAttribPointer(position2Attribute, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid *)(sizeof(vec3)*3));
	glVertexAttribPointer(color2Attribute , 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid *)(sizeof(vec3)*4));
	glVertexAttribPointer(normal2Attribute, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid *)(sizeof(vec3)*5));

}

void initData(char * mesh1, char * mesh2){
	vector<float> position[2];
	vector<float> color[2];
	vector<float> normal[2];
	char *meshes[] = {mesh1, mesh2};

	for (int i=0;i<2;i++){
		indices.clear(); // this avoid loading indices multiple times
		cout << "Loading file "<<meshes[i]<<endl;
	
		bool res = readVTKFile(meshes[i], position[i], color[i], normal[i], indices);
		if (!res){
			cout << "Cannot read " << mesh1 << flush<<endl;
			exit(1);
		}
	}
	cout << "Interleaving data" << endl;
	vector<Vertex> interleavedVertexData = interleaveData(position[0], color[0], normal[0], position[1], color[1], normal[1]);
	
	cout << "Uploading data" << endl;
	uploadData(interleavedVertexData);
	cout <<"Data uploaded"<<endl;
}

void loadShader(){
	shaderProgram = InitShader("blending.vert",  "blending.frag", "fragColor");
	projectionUniform = glGetUniformLocation(shaderProgram, "projection");
	if (projectionUniform == GL_INVALID_INDEX) {
		cerr << "Shader did not contain/use the 'projection' uniform."<<endl;
	}
	modelViewUniform = glGetUniformLocation(shaderProgram, "modelView");
	if (modelViewUniform == GL_INVALID_INDEX) {
		cerr << "Shader did not contain/use the 'modelView' uniform."<<endl;
	}
	blendValueUniform = glGetUniformLocation(shaderProgram, "blendValue");
	if (blendValueUniform == GL_INVALID_INDEX) {
		cerr << "Shader did not contain/use the 'blendValue' uniform."<<endl;
	}
	normalExtrusionUniform = glGetUniformLocation(shaderProgram, "normalExtrusion");
	if (normalExtrusionUniform == GL_INVALID_INDEX) {
		cerr << "Shader did not contain/use the 'normalExtrusion' uniform."<<endl;
	}
	normalAttribute = glGetAttribLocation(shaderProgram, "normal1"); 
	if (normalAttribute == GL_INVALID_INDEX){
		cerr << "Shader did not contain/use the 'normal1' attribute." << endl;
	}
	colorAttribute = glGetAttribLocation(shaderProgram, "color1"); 
	if (colorAttribute == GL_INVALID_INDEX) {
		cerr << "Shader did not contain/use the 'color1' attribute." << endl;
	}	
	positionAttribute = glGetAttribLocation(shaderProgram, "position1"); 
	if (positionAttribute == GL_INVALID_INDEX){
		cerr << "Shader did not contain/use the 'position1' attribute." << endl;
	}
	normal2Attribute = glGetAttribLocation(shaderProgram, "normal2"); 
	if (normal2Attribute == GL_INVALID_INDEX){
		cerr << "Shader did not contain/use the 'normal2' attribute." << endl;
	}
	color2Attribute = glGetAttribLocation(shaderProgram, "color2"); 
	if (color2Attribute == GL_INVALID_INDEX) {
		cerr << "Shader did not contain/use the 'color2' attribute." << endl;
	}	
	position2Attribute = glGetAttribLocation(shaderProgram, "position2"); 
	if (position2Attribute == GL_INVALID_INDEX){
		cerr << "Shader did not contain/use the 'position2' attribute." << endl;
	}
}

void updateTitle(){
	static int count = 0;
	if (count == 10){
		char buffer[100];
		sprintf(buffer, "VertexBlend: Blend %.2f NormalExtrusion: Extrusion %.2f", blendValue,normalExtrusion);

		glutSetWindowTitle(buffer);
		count = 0;
	}
	count++;
}

void display() {	
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
	const float timeScale = 0.008f;
	
    glUseProgram(shaderProgram);

	mat4 view = Translate(0,0,-dist) * RotateX(sphericalCoordinates.y) * RotateY(sphericalCoordinates.x);
	mat4 projection = Perspective(55, float(WINDOW_WIDTH) / WINDOW_HEIGHT, 0.01, 1000);
	glUniformMatrix4fv(projectionUniform, 1, GL_TRUE, projection);
	glUniformMatrix4fv(modelViewUniform, 1, GL_TRUE, view);
	glUniform1fv(blendValueUniform, 1, &blendValue);
	glUniform1fv(normalExtrusionUniform, 1, &normalExtrusion);

	
    
    // vertex shader uniforms
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, &indices[0]);
    
	glutSwapBuffers();

	Angel::CheckError();

	updateTitle();
}

void reshape(int W, int H) {
    WINDOW_WIDTH = W;
	WINDOW_HEIGHT = H;
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void animate() {	
	glutPostRedisplay();
}

void visible(int vis) {
	if (vis == GLUT_VISIBLE)
		glutIdleFunc(animate);
	else
		glutIdleFunc(0);
}

void mouse(int button, int state, int x, int y) {
	if(state == GLUT_DOWN) {
		mousePos = vec2(x,y);
		angleOffset = sphericalCoordinates;
    } 
}

void motion(int x, int y) {	
	const float motionSpeed = 32;
	vec2 deltaAng(M_PI * (mousePos.x - x) / WINDOW_WIDTH, 
		M_PI * (mousePos.y - y) / WINDOW_HEIGHT);
	sphericalCoordinates = angleOffset + deltaAng * motionSpeed;
}

void mouseWheel(int button, int dir, int x, int y) {
	if (dir > 0) {
		blendValue += 0.01;
		if (blendValue > 1.0){
			blendValue = 1.0;
		}
	} else {
		blendValue -= 0.01;
		if (blendValue < 0.0){
			blendValue = 0.0;
		}
	}
}

void keyboard(unsigned char key, int x, int y){
	// allow keyboard access if scroll wheel is unavailable
	if (key == '+') {
		mouseWheel(0, 1, 0,0 );
	} else if (key == '-') {
		mouseWheel(0, -1, 0,0 );
	} else if (key == 'n') {
		normalExtrusion+=0.01;
		if(normalExtrusion>1.0) {
			normalExtrusion=1.0;
		}
	} else if (key == 'm') {
		normalExtrusion-=0.01;
		if(normalExtrusion<0.0) {
			normalExtrusion=0.0;
		}
	}

}

void printHelp(){
	cout << "Use mouse drag to rotate around head."<< endl;
	cout << "Use mouse wheel or '+'/'-' to change blending (works when implemented)."<< endl;
}

int main(int argc, char* argv[]) {
	printHelp();
	glutInit(&argc, argv);
	glutInitContextVersion(3, 1);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    
	glutSetOption(
        GLUT_ACTION_ON_WINDOW_CLOSE,
        GLUT_ACTION_GLUTMAINLOOP_RETURNS
    );

	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH|GLUT_3_2_CORE_PROFILE);
	glutCreateWindow("02561-03-01");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMotionFunc(motion);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutMouseWheelFunc(mouseWheel);
	glutVisibilityFunc(visible);
	glutIdleFunc(animate);
	glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);

	Angel::InitOpenGL();
	
	glEnable(GL_DEPTH_TEST);

	loadShader();
	char * mesh1 = "Rasmus_DenseMesh_RGB_aligned.vtk";
	char * mesh2 = "Stine_DenseMesh_RGB_aligned.vtk";
	initData(mesh1, mesh2);

	Angel::CheckError();

	glutMainLoop();
}