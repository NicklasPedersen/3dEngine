#include <windows.h>
#include <time.h>
#include <iostream>
#include "Window.h"

Window *w = new Window();
double current;
double last;

static double g_freq;
static bool g_timerInitialized = false;


double GetTime()
{
	if (!g_timerInitialized)
	{
		LARGE_INTEGER li;
		if (!QueryPerformanceFrequency(&li))
			std::cerr << "QueryPerformanceFrequency failed in timer initialization" << std::endl;

		g_freq = double(li.QuadPart);
		g_timerInitialized = true;
	}

	LARGE_INTEGER li;
	if (!QueryPerformanceCounter(&li))
		std::cerr << "QueryPerformanceCounter failed in get time!" << std::endl;

	return double(li.QuadPart) / g_freq;
}

int main()
{
	current = GetTime();
	Vector2 a = { 0, 0 }, b = { 0, 10 }, c = { 10, 0 };
	while (w->IsAlive())
	{
		w->Clear(0, 0, 0);
		for (int i = 0; i < 100; i++)
		{
			w->DrawPixel(i, 10, 0xFFFFFF00);
		}
		last = current;
		current = GetTime();
		double diff = current - last;

		w->Update();
		std::cout << diff << std::endl;
	}
}

