#include "Triangle.h"

Triangle::Triangle()
{
	p[0] = {}, p[1] = {}, p[2] = {};
}

Triangle::Triangle(Vector3 p0, Vector3 p1, Vector3 p2)
{
	p[0] = p0, p[1] = p1, p[2] = p2;
}
