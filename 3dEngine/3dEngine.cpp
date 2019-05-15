#include <windows.h>
#include <time.h>
#include <iostream>
#include "Window.h"
#include "Mesh.h"
#include "Matrix4.h"

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
	Mesh cube;
	cube.tris = { {Vector3(0,0,0), Vector3(0,1,0), Vector3(1,1,0)},
				{Vector3(0,0,0), Vector3(1,1,0), Vector3(1,0,0)}, 

				{Vector3(1,0,0), Vector3(1,1,0), Vector3(1,1,1)},
				{Vector3(1,0,0), Vector3(1,1,1), Vector3(1,0,1)}, 

				{Vector3(1,0,1), Vector3(1,1,1), Vector3(0,1,1)},
				{Vector3(1,0,1), Vector3(0,1,1), Vector3(0,0,1)},

				{Vector3(0,0,1), Vector3(0,1,1), Vector3(0,1,0)},
				{Vector3(0,0,1), Vector3(0,1,0), Vector3(0,0,0)}, 

				{Vector3(0,1,0), Vector3(0,1,1), Vector3(1,1,1)},
				{Vector3(0,1,0), Vector3(1,1,1), Vector3(1,1,0)},

				{Vector3(1,0,1), Vector3(0,0,1), Vector3(0,0,0)},
				{Vector3(1,0,1), Vector3(0,0,0), Vector3(1,0,0)}};
	

	float fNear = 0.1f;
	float fFar = 1000;
	float fFov = 1.57079632679f;
	float fAspectRatio = w->Dimensions().x / w->Dimensions().y;
	fFov = 1 / Mathf::Tan(0.5 * fFov);

	Matrix4 matProj;

	matProj.m[0][0] = fFov;
	matProj.m[1][1] = fFov * fAspectRatio;
	matProj.m[2][2] = fFar / (fFar - fNear);
	matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
	matProj.m[2][3] = 1;
	matProj.m[3][3] = 0.0f;

	current = GetTime();
	Vector2 a = { 10, 20 }, b = { 200, 300 }, c = { 400, 400 };
	float x = 0;
	while (w->IsAlive())
	{
		w->BitmapShow();
		w->Clear(Vector4(255,0,0,0));
		for(auto tri : cube.tris)
		{
			Triangle triProjected, triTranslated(tri);

			triTranslated.p[0].z += 3 + x;
			triTranslated.p[1].z += 3 + x;
			triTranslated.p[2].z += 3 + x;

			triProjected.p[0] = triTranslated.p[0] * matProj;
			triProjected.p[1] = triTranslated.p[1] * matProj;
			triProjected.p[2] = triTranslated.p[2] * matProj;

			triProjected.p[0] = triProjected.p[0] + 1;
			triProjected.p[1] = triProjected.p[1] + 1;
			triProjected.p[2] = triProjected.p[2] + 1;

			triProjected.p[0].x *= w->dimensions.x / 2;
			triProjected.p[0].y *= w->dimensions.y / 2;
			triProjected.p[1].x *= w->dimensions.x / 2;
			triProjected.p[1].y *= w->dimensions.y / 2;
			triProjected.p[2].x *= w->dimensions.x / 2;
			triProjected.p[2].y *= w->dimensions.y / 2;

			w->DrawTriangle(triProjected.p[0].xy(),
				triProjected.p[1].xy(),
				triProjected.p[2].xy(),
				Vector4(255,0,0,200),false);
		}
		last = current;
		current = GetTime();
		double diff = current - last;

		w->Update();
		std::cout << diff << std::endl;
		x+=diff * 0.05f;
	}
}

