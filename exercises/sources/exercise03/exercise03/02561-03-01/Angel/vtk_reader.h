#ifndef __VTK_READER_H__
#define __VTK_READER_H__

#include <vector>
#include "Angel.h"

// loads the file 'filename' using the VTK file format (a DTU-IMM file format) and
// stores the result in the position/color/normal/indices vectors
bool readVTKFile(const char* filename, 
	std::vector<float> &position, 
	std::vector<float> &color, 
	std::vector<float> &normal, 
	std::vector<unsigned int> &indices);

#endif