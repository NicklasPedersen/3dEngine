#pragma once
#include <Windows.h>

class Vector2
{
public:
	float x, y;

	Vector2();
	Vector2(int x, int y);
	Vector2(float x, float y);
	Vector2(POINT p);
	
	Vector2(const Vector2 &v);
};