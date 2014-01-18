#ifndef __axis_h
#define __axis_h

#include "Angel.h"

class Axis {
public:
	Axis(void);
	~Axis(void);
	void renderWorldAxis(mat4& projection, vec4 eye, vec4 at, vec4 up, int window_width, int window_height, float width = 0.2, float heigh = 0.2);
private:
	void initialize();
	GLuint shaderProgram;
	GLuint projectionUniform,
		modelViewUniform,
		colorUniform;
	GLuint vertexArrayObject;
	GLuint vertexCount;
};

#endif // __axis_h