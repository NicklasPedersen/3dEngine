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
};
