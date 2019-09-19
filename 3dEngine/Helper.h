#pragma once

template<class T>
class Helper
{
public:
	static inline void Swap(T *a, T *b)
	{
		T temp = *a;
		*a = *b;
		*b = temp;
	}
};