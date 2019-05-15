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
};
