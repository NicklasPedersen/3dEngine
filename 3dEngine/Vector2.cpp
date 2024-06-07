#include "Vector2.h"

Vector2::Vector2()
{
	x = 0, y = 0;
}


Vector2::Vector2(float x, float y)
{
	this->x = x, this->y = y;
}

Vector2::Vector2(POINT p)
{
	x = (float)p.x, y = (float)p.y;
}

float Vector2::Surface()
{
	return x * y;
}

float Vector2::Cross(Vector2 v)
{
	return x * v.y - y * v.x;
}

void Vector2::Round()
{
	x = Mathf::Round(x), y = Mathf::Round(y);
}

Vector2::Vector2(const Vector2 &v)
{
	x = v.x, y = v.y;
}

Vector2 Vector2::operator+(const Vector2& v)
{
	return Vector2(x + v.x, y + v.y);
}

Vector2 Vector2::operator-(const Vector2& v)
{
	return Vector2(x - v.x, y - v.y);
}
