#pragma once
#include "Matrix4.h"
#include "Vector4.h"
#include "Vector2.h"

class Vector3
{
public:
	float x = 0, y = 0, z = 0;

	Vector3();
	Vector3(float x, float y, float z);

	Vector2 xy();
	Vector2 xz();
	Vector2 yx();
	Vector2 yz();
	Vector2 zx();
	Vector2 zy();


	Vector3 operator+(const float &f);

	Vector3 operator*(const Matrix4 &m);
};