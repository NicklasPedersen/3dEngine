#include "Vector3.h"

Vector3::Vector3()
{
	x = 0, y = 0, z = 0;
}

Vector3::Vector3(float x, float y, float z)
{
	this->x = x, this->y = y, this->z = z;
}

Vector2 Vector3::xy()
{
	return Vector2(x, y);
}

Vector2 Vector3::xz()
{
	return Vector2(x, z);
}

Vector2 Vector3::yx()
{
	return Vector2(y, x);
}

Vector2 Vector3::yz()
{
	return Vector2(y, z);
}

Vector2 Vector3::zx()
{
	return Vector2(z, x);
}

Vector2 Vector3::zy()
{
	return Vector2(z, y);
}

Vector3 Vector3::operator+(const float & f)
{
	return Vector3(x + f, y + f, z + f);
}

Vector3 Vector3::operator*(const Matrix4 & m)
{
	float ox =		  x * m.m[0][0] + y * m.m[1][0] + z * m.m[2][0] + m.m[3][0];
	float oy =		  x * m.m[0][1] + y * m.m[1][1] + z * m.m[2][1] + m.m[3][1];
	float oz =		  x * m.m[0][2] + y * m.m[1][2] + z * m.m[2][2] + m.m[3][2];
	float ow = x * m.m[0][3] + y * m.m[1][3] + z * m.m[2][3] + m.m[3][3];
	if(ow != 0.0f)
	{
		ox /= ow;
		oy /= ow;
		oz /= ow;
	}
	return Vector3(ox, oy, oz);
}
