#ifndef __SELECT_H
#define __SELECT_H

#include <string>
#include <GL/glew.h>
#include "Angel.h"

// Select buffer renders object ids into an offscreen buffer that can be used to find
// which object is on a given pixel in screen coordinates.
// The pseudocode for using the Select buffer is as follows:
//
// SelectBuffer *selectBuffer;
// GLuint colorUniform; // should point to the color uniform (vec4) of the current shader
// void renderScene(bool selectModel){
//    for each object o in objectsInScene){ 
//      if (selectModel){
//            selectBuffer->setId(o.id);
//      } else {
//            setColor(o);
//      }
//      renderObject(o);
//    }
// }
// void onMouseClick(int x, int y){
//   selectBuffer->setColorUniform(colorUniform);
//   selectBuffer->bind();
//   renderScene(true);
//   int objectId = selectBuffer->getId(x, y);
//   cout << "Found object " << objectId << endl;
//   selectBuffer->release();
// }
//
// The important part of the pseudocode is to call selectBuffer->setId instead of the setting the color (and change the shader). 
//
// Internally the Select buffer uses a FrameBufferObject that writes to a offscrene texture.
// The select buffer needs a passthrough shader to be bound where the output
// color (vec4) can be set using the colorUniform.
class SelectBuffer {
public:
	SelectBuffer(int width, int height, GLuint colorUniform = 0);
	~SelectBuffer();
	// binds the selectBuffer
	void bind();
	// releases the select buffer
	void release();
	// set the id of the next object to be rendered (used only when select buffer is bound)
	void setId(unsigned short id);
	// returns the id of the pixel x,y (or -1 if no object found)
	int getId(int x, int y);
	// change the color uniform of the current bound shader (must be a vec4 uniform which is written directly to fragment shader output).
	void setColorUniform(GLuint colorUniform) { this->colorUniform = colorUniform; }
private:
	void buildTexture();
	void buildFrameBufferObject();
	std::string getFrameBufferStatusString(GLenum code);
	GLuint framebufferObjectId;
	GLuint renderBufferId;
	GLuint textureId;
	GLuint colorUniform;
	int width;
	int height;
};

#endif