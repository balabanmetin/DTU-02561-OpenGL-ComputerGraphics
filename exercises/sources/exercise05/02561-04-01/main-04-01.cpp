// 02561-04-01
/* Based on newpaint.c from */
/* E. Angel, Interactive Computer Graphics */
/* A Top-Down Approach with OpenGL, Third Edition */
/* Addison-Wesley Longman, 2003 */
/* Ported to OpenGL 3.2 Core 2012 */
#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Angel.h"

using namespace std;
using namespace Angel;

int WINDOW_WIDTH = 500;
int WINDOW_HEIGHT = 500;

#define NULL 0
#define LINE 1
#define RECTANGLE 2
#define TRIANGLE  3
#define POINTS 4

GLuint shaderProgram;
GLuint projectionUniform,
	modelViewUniform,
	colorUniform;
GLuint positionAttribute;

bool fillPolygon = false;
int draw_mode = 0;
GLfloat pointSize = 3.0; 
vec4 color;

struct Vertex {
	vec4 position;
};

// forward declaration
void loadShader();
void display();
int pick(vec2 position);
void reshape(int W, int H);
GLuint loadBufferData(Vertex* vertices, int vertexCount, GLuint& outVertexBuffer);

void right_menu(int id) {
	if(id == 1) {
		exit(0);
	} else if (id == 2) {
		reshape(WINDOW_WIDTH, WINDOW_HEIGHT);
	}
}

void color_menu(int id){
	float r,g,b;
	if(id == 1) {r = 1.0; g = 0.0; b = 0.0;}
	else if(id == 2) {r = 0.0; g = 1.0; b = 0.0;}
	else if(id == 3) {r = 0.0; g = 0.0; b = 1.0;}
	else if(id == 4) {r = 0.0; g = 1.0; b = 1.0;}
	else if(id == 5) {r = 1.0; g = 0.0; b = 1.0;}
	else if(id == 6) {r = 1.0; g = 1.0; b = 0.0;}
	else if(id == 7) {r = 1.0; g = 1.0; b = 1.0;}
	else if(id == 8) {r = 0.0; g = 0.0; b = 0.0;}
	color = vec4(r,g,b,1.0);
}

void pixel_menu(int id) {
	if (id == 1) pointSize = 2 * pointSize;
	else if (pointSize > 1) pointSize = pointSize/2;
}

void fill_menu(int id) {
	if (id == 1) fillPolygon = 1; 
    else fillPolygon = 0;
}


void draw_rect(vec2 from, vec2 to) {
	GLuint vertexBuffer;
	GLuint rectVertexArrayObject;
	int size;
	int mode;
	if (fillPolygon) {
		size = 4;
		Vertex rectData[4] = {
			{ vec4( from[0],  from[1],  0, 1.0 ) },
			{ vec4( from[0],    to[1],  0, 1.0 ) },
			{ vec4(   to[0],    to[1],  0, 1.0 ) },
			{ vec4(   to[0],  from[1],  0, 1.0 ) },
		};
		rectVertexArrayObject = loadBufferData(rectData, size, vertexBuffer);
		mode = GL_TRIANGLE_FAN;
	} else {
		size = 4;
		Vertex rectData[6] = {
			{ vec4( from[0],  from[1],  0, 1.0 ) },
			{ vec4( from[0],    to[1],  0, 1.0 ) },
			{ vec4(   to[0],    to[1],  0, 1.0 ) },
			{ vec4(   to[0],  from[1],  0, 1.0 ) }
		};
		rectVertexArrayObject = loadBufferData(rectData, size, vertexBuffer);
		mode = GL_LINE_LOOP;
	}
	glDrawArrays(mode, 0, size);

	glDeleteVertexArrays(1,&rectVertexArrayObject);
	glDeleteBuffers(1,&vertexBuffer);
}

GLuint loadBufferData(Vertex* vertices, int vertexCount, GLuint& outVertexBuffer) {
	GLuint vertexArrayObject;

	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(positionAttribute);
	glVertexAttribPointer(positionAttribute, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)0);

	outVertexBuffer = vertexBuffer;
	return vertexArrayObject;
}

void loadShader() {
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

void draw_point(vec2 pos, float size) {
	vec2 offset(size, size);
	fillPolygon = true;
	draw_rect(pos,pos+offset);
}

void draw_line(vec2 from, vec2 to) {
	GLuint vertexBuffer;
	GLuint lineVertexArrayObject;

	Vertex rectData[2] = {
		{ vec4( from[0],  from[1],  0, 1.0 ) },
		{ vec4(   to[0],    to[1],  0, 1.0 ) }
	};
	lineVertexArrayObject = loadBufferData(rectData, 2, vertexBuffer);
	glDrawArrays(GL_LINES, 0, 2);

	glDeleteVertexArrays(1,&lineVertexArrayObject);
	glDeleteBuffers(1,&vertexBuffer);
}

void draw_triangle(vec2 v1, vec2 v2, vec2 v3) {
	GLuint vertexBuffer;
	GLuint triangleVertexArrayObject;
	const int size = 3;
	int mode;
	Vertex triangleData[size] = {
		{ vec4( v1[0],  v1[1],  0, 1.0 ) },
		{ vec4( v2[0],  v2[1],  0, 1.0 ) },
		{ vec4( v3[0],  v3[1],  0, 1.0 ) }
	};
	triangleVertexArrayObject = loadBufferData(triangleData, size, vertexBuffer);
	if (fillPolygon) {
		mode = GL_TRIANGLES;
	} else {
		mode = GL_LINE_LOOP;
	}
	glDrawArrays(mode, 0, size);

	glDeleteVertexArrays(1,&triangleVertexArrayObject);
	glDeleteBuffers(1,&vertexBuffer);
}


void mouse(int button, int state, int x, int y) {
	static int count;
	int menu;
	static vec2 prev_positions[2];
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		vec2 position(x,WINDOW_HEIGHT-y);
		menu = pick(vec2(x,y));
		if (menu != 0) {
			count = 0;
			draw_mode = menu;
		} else {
			glUniform4fv(colorUniform, 1, color);
			switch(draw_mode) {
			case(LINE):
				if(count == 0) {
					count++;
					prev_positions[0] = position;
				} else {
					draw_line(prev_positions[0], position);
					count=0;
				}
				break;
			case(RECTANGLE):
				if(count == 0) {
					count++;
					prev_positions[0] = position;
				} else {
					draw_rect(prev_positions[0], position);
					count=0;
				}
				break;
			case (TRIANGLE):
				switch(count) {
				case(0):
					count++;
					prev_positions[0] = position;
					break;
				case(1):
					count++;
					prev_positions[1] = position;
					break;
				case(2): 
					draw_triangle(prev_positions[0], prev_positions[1], position);
					count=0;
				}
				break;
			case(POINTS):
				{
					draw_point(position,pointSize);
				}
				break;
			}
		}
		glFlush();
	}
}

void display() {	
	glUseProgram(shaderProgram);

	mat4 projection = Ortho(0.0, (GLdouble)WINDOW_WIDTH, 0.0, (GLdouble)WINDOW_HEIGHT, -1.0, 1.0);
	glUniformMatrix4fv(projectionUniform, 1, GL_TRUE, projection);
	mat4 modelView;
	glUniformMatrix4fv(modelViewUniform, 1, GL_TRUE, modelView);
	bool fillPolygonOld = fillPolygon;
	// draw menu background
	glUniform4fv(colorUniform, 1, vec4(1.0, 1.0, 1.0, 1.0));
	draw_point(vec2(0,WINDOW_HEIGHT-WINDOW_WIDTH/10),WINDOW_WIDTH/10);
	glUniform4fv(colorUniform, 1, vec4(1.0, 0.0, 0.0, 1.0));
	draw_point(vec2(WINDOW_WIDTH/10,WINDOW_HEIGHT-WINDOW_WIDTH/10),WINDOW_WIDTH/10);
	glUniform4fv(colorUniform, 1, vec4(0.0, 1.0, 0.0, 1.0));
	draw_point(vec2(WINDOW_WIDTH/5,WINDOW_HEIGHT-WINDOW_WIDTH/10),WINDOW_WIDTH/10);
	glUniform4fv(colorUniform, 1, vec4(0.0, 0.0, 1.0, 1.0));
	draw_point(vec2(3*WINDOW_WIDTH/10,WINDOW_HEIGHT-WINDOW_WIDTH/10),WINDOW_WIDTH/10);

	// fill menu symbols
	glUniform4fv(colorUniform, 1, vec4(0.0, 0.0, 0.0, 1.0));
	draw_line(vec2(WINDOW_WIDTH/40,WINDOW_HEIGHT-WINDOW_WIDTH/20),vec2(WINDOW_HEIGHT/40+WINDOW_WIDTH/20,WINDOW_HEIGHT-WINDOW_WIDTH/20));
	draw_point(vec2(WINDOW_WIDTH/10+WINDOW_WIDTH/40,WINDOW_HEIGHT-WINDOW_WIDTH/10+WINDOW_WIDTH/40),WINDOW_WIDTH/20);
	draw_triangle(vec2(WINDOW_WIDTH/5+WINDOW_WIDTH/40,WINDOW_HEIGHT-WINDOW_WIDTH/10+WINDOW_WIDTH/40),
	vec2(WINDOW_WIDTH/5+WINDOW_WIDTH/20,WINDOW_HEIGHT-WINDOW_WIDTH/40),
	vec2(WINDOW_WIDTH/5+3*WINDOW_WIDTH/40,WINDOW_HEIGHT-WINDOW_WIDTH/10+WINDOW_WIDTH/40));
	draw_point(vec2(3*WINDOW_WIDTH/10+WINDOW_WIDTH/20-2, WINDOW_HEIGHT-WINDOW_WIDTH/20-2),4);
	fillPolygon = fillPolygonOld;
	glFlush();

	Angel::CheckError();
}

void reshape(int W, int H) {
	glViewport(0,0,W,H);
	glClearColor (0.8, 0.8, 0.8, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	display();

	WINDOW_WIDTH = W;
	WINDOW_HEIGHT = H;
}

int pick(vec2 position) {
	int x = position[0], 
		y  = position[1];
	y = WINDOW_HEIGHT - y;
	if(y < WINDOW_HEIGHT-WINDOW_WIDTH/10) return 0;
	else if(x < WINDOW_WIDTH/10) return LINE;
	else if(x < WINDOW_WIDTH/5) return RECTANGLE;
	else if(x < 3*WINDOW_WIDTH/10) return TRIANGLE;
	else if(x < 2*WINDOW_WIDTH/5) return POINTS;
	else return 0;
}


void initMenu(){
	int c_menu, p_menu, f_menu;
	c_menu = glutCreateMenu(color_menu);
	glutAddMenuEntry("Red",1);
	glutAddMenuEntry("Green",2);
	glutAddMenuEntry("Blue",3);
	glutAddMenuEntry("Cyan",4);
	glutAddMenuEntry("Magenta",5);
	glutAddMenuEntry("Yellow",6);
	glutAddMenuEntry("White",7);
	glutAddMenuEntry("Black",8);
	p_menu = glutCreateMenu(pixel_menu);
	glutAddMenuEntry("increase pixel size", 1);
	glutAddMenuEntry("decrease pixel size", 2);
	f_menu = glutCreateMenu(fill_menu);
	glutAddMenuEntry("fill on", 1);
	glutAddMenuEntry("fill off", 2);
	glutCreateMenu(right_menu);
	glutAddMenuEntry("quit",1);
	glutAddMenuEntry("clear",2);
	glutAddSubMenu("Colors", c_menu);
	glutAddSubMenu("Pixel Size", p_menu);
	glutAddSubMenu("Fill", f_menu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
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

	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE|GLUT_3_2_CORE_PROFILE);
	glutCreateWindow("02561-04-01");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutMouseFunc(mouse);
	
	Angel::InitOpenGL();
	
	initMenu();
	loadShader();

	Angel::CheckError();

	glutMainLoop();
}