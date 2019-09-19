#pragma once
#include <vector>
#include "Triangle.h"
#include <strstream>
#include <fstream>

class Mesh
{
public:
	std::vector<Triangle> tris;
	Mesh();
	Mesh(const char *sFilename);
};
