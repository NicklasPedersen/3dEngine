#pragma once
#include <Windows.h>

class Vector4
{
public:
	float x, y, z, w;

	Vector4();
	Vector4(int x, int y, int z, int w);
	Vector4(float x, float y, float z, float w);
	
	Vector4(const Vector4 &v);

	unsigned int argb();

	Vector4 operator+(const Vector4 &v);
	Vector4 operator-(const Vector4 &v);
};