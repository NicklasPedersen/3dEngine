#include "Vector4.h"

Vector4::Vector4()
{
	x = 0, y = 0, z = 0, w = 0;
}

Vector4::Vector4(int x, int y, int z, int w)
{
	this->x = x, this->y = y, this->z = z, this->w = w;
}

Vector4::Vector4(float x, float y, float z, float w)
{
	this->x = x, this->y = y, this->z = z, this->w = w;
}

Vector4::Vector4(const Vector4 & v)
{
	x = v.x, y = v.y, z = v.z, w = v.w;
}

unsigned int Vector4::argb()
{
	return (((unsigned int) x) << 24) + (((unsigned int) x) << 16) + (((unsigned int) x) << 8) + ((unsigned int) x);
}

Vector4 Vector4::operator+(const Vector4 & v)
{
	return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
}

Vector4 Vector4::operator-(const Vector4 & v)
{
	return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
}
