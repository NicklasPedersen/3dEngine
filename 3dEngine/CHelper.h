#pragma once
inline void Swap(void **a, void **b)
{
	void *temp = *a;
	*a = *b;
	*b = temp;
}