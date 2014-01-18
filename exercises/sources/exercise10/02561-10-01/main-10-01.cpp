// 02561-10-01
#include <iostream>
#include <string>
#include <algorithm>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Angel.h"
#include "ObjLoader.h"
#include "TextureLoader.h"

using namespace std;
using namespace Angel;

int WINDOW_WIDTH = 500;
int WINDOW_HEIGHT = 500;

struct Shader{
	GLuint shaderProgram;
	GLuint projectionUniform,
		modelViewUniform,
		cubemapUniform,
		textureUniform,
		cameraPosUniform;
	GLuint positionAttribute;
};

struct MeshObject {
	Shader shader;
	GLuint vertexArrayObject;
	vector<int> indices;
	string name;

	MeshObject(){}

	MeshObject(Shader shader, GLuint vertexArrayObject, vector<int> indices, const char* name)
		:shader(shader),vertexArrayObject(vertexArrayObject),indices(indices),name(name){
	}

};

struct Vertex {
    vec3 position;
};

MeshObject skybox;

vec4 cameraPosition;
vector<MeshObject> vertexBufferObjects;
int selectedVertexBufferObject = 0;

GLuint cubemapTexture,
	normalmapTexture;

const int axisSize = 6;

// spherical coordinates of camera position (angles in radian)
float sphericalPolarAngle = 0;
float sphericalElevationAngle = 0;
float sphericalRadius = 3;

vec2 mousePos;

// forward declaration
void loadShader();
void display();
vec3 sphericalToCartesian(float polarAngle, float elevationAngle, float radius);

GLuint createVertexArrayObject(GLuint vertexBuffer, const Shader & shader){
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	GLuint vertexArrayObject;

    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

	glEnableVertexAttribArray(shader.positionAttribute);
    glVertexAttribPointer(shader.positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)0);

	return vertexArrayObject;
}

GLuint createVertexBuffer(Vertex* vertices, int vertexCount) {
	GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), vertices, GL_STATIC_DRAW);
    
	return vertexBuffer;
}

GLuint loadMesh(char *filename, vector<int> &indices, float scale = 1.0){
	vector<vec3> position;
	vector<vec3> normal; // unused
	vector<vec2> uv; // unused
	loadObject(filename,position,indices,normal,uv, scale); 
	Vertex* vertexData = new Vertex[position.size()];

	for (int i = 0; i < position.size(); i++) {
		vertexData[i].position = position[i];
	}
	GLuint vertexBuffer = createVertexBuffer(vertexData, position.size());
	delete [] vertexData;
	return vertexBuffer;
}

Shader loadShader(const char* vertShader, const char* fragShader){
	Shader shader;
	shader.shaderProgram = InitShader(vertShader, fragShader, "fragColor");
	// get uniform locations
	shader.projectionUniform = glGetUniformLocation(shader.shaderProgram, "projection");
	shader.modelViewUniform = glGetUniformLocation(shader.shaderProgram, "modelView");
	shader.cubemapUniform = glGetUniformLocation(shader.shaderProgram, "cubemap");
	shader.textureUniform = glGetUniformLocation(shader.shaderProgram, "textureBump");
	shader.cameraPosUniform = glGetUniformLocation(shader.shaderProgram, "cameraPos");
	
	// get attribute locations
	shader.positionAttribute = glGetAttribLocation(shader.shaderProgram, "position");
	return shader;
}


void drawMeshObject(mat4 & projection, mat4 & modelView, MeshObject& meshObject) {
	glUseProgram(meshObject.shader.shaderProgram);
	if (meshObject.shader.projectionUniform != GL_INVALID_INDEX){
		glUniformMatrix4fv(meshObject.shader.projectionUniform, 1, GL_TRUE, projection);
	}
	if (meshObject.shader.modelViewUniform !=  GL_INVALID_INDEX){
		glUniformMatrix4fv(meshObject.shader.modelViewUniform, 1, GL_TRUE, modelView);
	}
	if (meshObject.shader.cameraPosUniform != GL_INVALID_INDEX){
		glUniform3fv(meshObject.shader.cameraPosUniform, 1, cameraPosition);
	}
	if (meshObject.shader.cubemapUniform !=  GL_INVALID_INDEX){
		// bind cubemap to texture slot 0 and set the uniform value
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glUniform1i(meshObject.shader.cubemapUniform, 0);
	}
	if (meshObject.shader.textureUniform !=  GL_INVALID_INDEX){
		// bind texture to texture slot 1 and set the uniform value
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalmapTexture);
		glUniform1i(meshObject.shader.textureUniform, 1);
	}
	
	glBindVertexArray(meshObject.vertexArrayObject);
	glDrawElements(GL_TRIANGLES, meshObject.indices.size(), GL_UNSIGNED_INT, &meshObject.indices[0]);
}

void display() {	
    glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	mat4 projection = Perspective(60,WINDOW_WIDTH/(float)WINDOW_HEIGHT,0.1,1000);
	cameraPosition = sphericalToCartesian(sphericalPolarAngle, sphericalElevationAngle, sphericalRadius);
	vec4 at(0,0,0,1);
	vec4 up(0,1,0,0);
	mat4 modelView = LookAt(cameraPosition, at, up);

	drawMeshObject(projection, modelView, skybox);
	
	drawMeshObject(projection, modelView, vertexBufferObjects[selectedVertexBufferObject]);
	
	glutSwapBuffers();

	Angel::CheckError();
}

void reshape(int W, int H) {
    WINDOW_WIDTH = W;
	WINDOW_HEIGHT = H;
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void mouse(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		mousePos = vec2(x,y);
	}
}

void mouseMovement(int x, int y){
	float rotationSpeed = 0.01;
	vec2 newMousePos = vec2(x,y);
	vec2 mousePosDelta = mousePos - newMousePos;
	sphericalPolarAngle += mousePosDelta.x*rotationSpeed;
	float rotate89Degrees = 89*DegreesToRadians;
	sphericalElevationAngle = min(rotate89Degrees,max(-rotate89Degrees,sphericalElevationAngle + mousePosDelta.y*rotationSpeed));
	mousePos = vec2(x,y);
	glutPostRedisplay();
}

void initCubemapTexture() {
	
	const char* cube[] = {"textures/cm_left.bmp", 
								"textures/cm_right.bmp", 
								"textures/cm_top.bmp", 
								"textures/cm_bottom.bmp", 
								"textures/cm_back.bmp", 
								"textures/cm_front.bmp" };
	unsigned char * data[6];

	 // todo load cubemap into the variable cubemapTexture

	unsigned int width[6], height[6];
	for(int i=0;i<6;i++){
		 data[i] = loadBMPRaw(cube[i], width[i], height[i]);
	}

	glActiveTexture(GL_TEXTURE0); 
	// Create one OpenGL texture
	glGenTextures(1, &cubemapTexture);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0,GL_RGB, width[0], height[0], 0, GL_BGR, GL_UNSIGNED_BYTE, data[0]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0,GL_RGB, width[1], height[1], 0, GL_BGR, GL_UNSIGNED_BYTE, data[1]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0,GL_RGB, width[2], height[2], 0, GL_BGR, GL_UNSIGNED_BYTE, data[2]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0,GL_RGB, width[3], height[3], 0, GL_BGR, GL_UNSIGNED_BYTE, data[3]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0,GL_RGB, width[4], height[4], 0, GL_BGR, GL_UNSIGNED_BYTE, data[4]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0,GL_RGB, width[5], height[5], 0, GL_BGR, GL_UNSIGNED_BYTE, data[5]);
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

}

void initNormalmapTexture(){
	const char* normalmap = "textures/normalmap.bmp";
	glGenTextures(1, &normalmapTexture);
	glBindTexture(GL_TEXTURE_2D, normalmapTexture);
	unsigned int nWidth, nHeight;
	void* data = loadBMPRaw(normalmap, nWidth, nHeight); // note that the function both returns data, nWidth and nHeight when loading the image
	glTexImage2D(GL_TEXTURE_2D,
		0, //level
		GL_RGB8, //internal format
		nWidth, //width
		nHeight, //height
		0, //border
		GL_BGR, //format - Not BMP uses BGR not RGB
		GL_UNSIGNED_BYTE, //type
		data); //pixel data
	delete [] data;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void initSkyboxMeshAndShader(){
	Shader skyboxShader = loadShader("skybox.vert",  "skybox.frag");
	vector<int> skyboxIndices;
	
	GLuint cubeVertexBuffer = loadMesh("meshes/cube.obj", skyboxIndices, 50.0f);
	GLuint skyboxVertexArrayObject = createVertexArrayObject(cubeVertexBuffer, skyboxShader);
	skybox = MeshObject(
		skyboxShader,
		skyboxVertexArrayObject,
		skyboxIndices,
		"Skybox"
	);
}

void initSphereMeshAndShader(){
	const int SHADERS_SIZE = 12;
	string shaders[SHADERS_SIZE] = {
		"mirror.vert",  "mirror.frag", "Mirror shader",
		"glass.vert",  "glass.frag", "Glass shader",
		"bumpmap.vert",  "bumpmap.frag", "Bumpmap shader",
		"chromatic.vert", "chromatic.frag", "Chromatic shader"
	};
	vector<int> indices;
	vertexBufferObjects.clear();
	GLuint sphereVertexBuffer = loadMesh("meshes/sphere.obj", indices);
	for (int i=0;i<SHADERS_SIZE;i=i+3){
		Shader shader = loadShader(shaders[i].c_str(),  shaders[i+1].c_str());
		GLuint vertexArrayObject = createVertexArrayObject(sphereVertexBuffer, shader);
		vertexBufferObjects.push_back( MeshObject(
			shader,
			vertexArrayObject,
			indices,
			shaders[i+2].c_str()
		));
	}
}

void reloadShaders() {
	initSkyboxMeshAndShader();
	initSphereMeshAndShader();
}

void keyPress(unsigned char key, int x, int y) {
	switch (key){
	case 's':
		selectedVertexBufferObject = (selectedVertexBufferObject+1)%vertexBufferObjects.size();
		cout << "New shader "<< vertexBufferObjects[selectedVertexBufferObject].name <<endl;
		glutSetWindowTitle(vertexBufferObjects[selectedVertexBufferObject].name.c_str());
		break;
	case 'r':
		cout << "Reloading shaders" << endl;
		reloadShaders();
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char* argv[]) {
	cout << "---------------------------------------------------" << endl;
	cout << "Mouse drag to rotate camera around sphere" << endl;
	cout << "Press 's' to change shader" << endl;
	cout << "Press 'r' to reload shaders" << endl;
	cout << "---------------------------------------------------" << endl;
    glutInit(&argc, argv);
	glutInitContextVersion(3, 1);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    
	glutSetOption(
        GLUT_ACTION_ON_WINDOW_CLOSE,
        GLUT_ACTION_GLUTMAINLOOP_RETURNS
    );

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH|GLUT_3_2_CORE_PROFILE);
	glutCreateWindow("02561-10-01");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMovement);
	glutKeyboardFunc(keyPress);
	glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);

	Angel::InitOpenGL();

	initSkyboxMeshAndShader();
	initSphereMeshAndShader();
	initCubemapTexture();
	initNormalmapTexture();

	glEnable(GL_DEPTH_TEST);

	Angel::CheckError();

	glutMainLoop();
}

// Convert from spherical coordinates to cartesian coordinates
vec3 sphericalToCartesian(float polarAngle, float elevationAngle, float radius){
	float a = radius * cos(elevationAngle);
	vec3 cart;
	cart.x = a * cos(polarAngle);
	cart.y = radius * sin(elevationAngle);
	cart.z = a * sin(polarAngle);
	return cart;
}