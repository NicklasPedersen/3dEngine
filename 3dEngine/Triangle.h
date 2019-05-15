#pragma once
#include "Vector3.h"

class Triangle
{
public:
	Vector3 p[3];
	Triangle();
	Triangle(Vector3 p0, Vector3 p1, Vector3 p2);
};
