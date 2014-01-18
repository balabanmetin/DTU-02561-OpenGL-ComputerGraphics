#include "vtk_reader.h"

#include<iostream>
#include<fstream>

using namespace std;
using namespace Angel;


void splitLine(char *line, std::vector<float> &vector);
void splitPolygon(char *line, std::vector<unsigned int> &vector);

bool readVTKFile(const char* filename, std::vector<float> &position, std::vector<float> &color, std::vector<float> &normal, std::vector<unsigned int> &indices){
	indices.clear();
	enum ReadType {None, Points, Polygons, Colors, Normals};
	ifstream myReadFile;
	myReadFile.open(filename);
	const int length = 255;
	char line[length];
	ReadType type = None;
	if (myReadFile.is_open()) {
		while (!myReadFile.eof()) {
			myReadFile.getline(line, length);
			if (myReadFile.good()){
				if (strncmp("COLOR_SCALARS", line, 12)==0){
					type = Colors;
					cout << "Loading color" << endl;
				} else if (strncmp("NORMALS", line, 7)==0){
					type = Normals;
					cout << "Loading normals" << endl;
				} else if (strncmp("POLYGONS", line, 8)==0){
					type = Polygons;
					cout << "Loading polygons"<< endl;
				} else if (strncmp("POINTS", line, 6)==0){
					type = Points;
					cout << "Loading points" << endl;
				} else {
					switch (type){
					case Points:
						splitLine(line, position);
						break;
					case Polygons:
						splitPolygon(line, indices);
						break;
					case Colors:
						splitLine(line, color);
						break;
					case Normals:
						splitLine(line, normal);
						break;
					}
				}
			}
		}
	}
	myReadFile.close();
	cout << "Positions "<<position.size()<<endl;
	cout << "Color "<<color.size()<<endl;
	cout << "Normal "<<normal.size()<<endl;
	cout << "Indices "<<indices.size()<<endl;
	return position.size() > 0;
}


void splitLine(char *line, std::vector<float> &vector){
	string s(line);
	size_t indexFrom = 0;
	size_t lastIndex = s.find_first_of(' ');
	while (lastIndex != string::npos) {
		float f = atof(s.substr(indexFrom,lastIndex).c_str());
		vector.push_back(f);
		indexFrom = lastIndex+1;
		lastIndex = s.find_first_of(' ',indexFrom); 
	}
}

void splitPolygon(char *line, std::vector<unsigned int> &vector){
	string s(line);
	size_t indexFrom = 0;
	size_t lastIndex = s.find_first_of(' ');
	while (lastIndex != string::npos) {
		int i = atoi(s.substr(indexFrom,lastIndex).c_str());
		if (indexFrom != 0) {
			vector.push_back(i);
		}
		indexFrom = lastIndex+1;
		lastIndex = s.find_first_of(' ',indexFrom); 
	}
}
