#ifdef __APPLE__  // include Mac OS X verions of headers
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else // non-Mac OS X operating systems
#  include <GL/glew.h>
#  include <GL/freeglut.h>
#  include <GL/freeglut_ext.h>
#endif  // __APPLE__

#ifndef __TEAPOT_H
#define __TEAPOT_H

namespace Angel {
	// Loads teapot Vertex Array Object
	GLuint LoadTeapotVAO(int &outNumVertices, GLuint positionAttribute, 	int NumTimesToSubdivide = 3, int PatchesPerSubdivision = 4);
}
#endif // __TEAPOT_H