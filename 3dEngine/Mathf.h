#pragma once
#include <math.h>

class Mathf
{
public:
	inline static float Abs(float val)
	{
		return val < 0 ? -val : val;
	}
	inline static float Sign(float val)
	{
		return val < 0 ? -1 : (val > 0 ? 1 : 0); 
	}
	inline static float Sqrt(float x)
	{
		float xhalf = 0.5f*x;
		union
		{
			float x;
			int i;
		} u;
		u.x = x;
		u.i = 0x5f375a86 - (u.i >> 1);
		u.x = u.x * (1.5f - xhalf * u.x * u.x);
		return 1 / u.x;
	}
	inline static float Tan(float rad)
	{
		return tanf(rad);
	}
	inline static float Cos(float rad)
	{
		return cosf(rad);
	}
	inline static float Sin(float rad)
	{
		return sinf(rad);
	}
	inline static float Min(float a, float b)
	{
		return a < b ? a : b;
	}
	inline static float Max(float a, float b)
	{
		return a > b ? a : b;
	}
	inline static float Round(float f)
	{
		return (int)(f + 0.5f);
	}
	inline static float Clamp(float f, float min, float max)
	{
		return f < min ? min : (f > max ? max : f);
	}
};
