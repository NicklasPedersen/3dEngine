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
	/*cube.tris = { {Vector3(0,0,1), Vector3(0,1,1), Vector3(1,0,1)} };*/
	

	float fNear = 0.1f;
	float fFar = 1000.0f;
	float fFov = 75;
	float fAspectRatio = w->Dimensions().y / w->Dimensions().x;
	float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

	Matrix4 matProj;

	matProj.m[0][0] = fAspectRatio * fFovRad;
	matProj.m[1][1] = fFovRad;
	matProj.m[2][2] = fFar / (fFar - fNear);
	matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
	matProj.m[2][3] = 1.0f;
	matProj.m[3][3] = 0.0f;

	current = GetTime();
	Vector2 a = { 10, 20 }, b = { 200, 300 }, c = { 400, 400 };

	float fTheta = 0;

	while (w->IsAlive())
	{
		w->BitmapShow();
		w->Clear(Vector4(255, 0, 0, 0));

		last = current;
		current = GetTime();
		double diff = current - last;

		// Set up rotation matrices
		Matrix4 matRotZ, matRotX;
		fTheta += 1.0f * diff;

		// Rotation Z
		matRotZ.m[0][0] = cosf(fTheta);
		matRotZ.m[0][1] = sinf(fTheta);
		matRotZ.m[1][0] = -sinf(fTheta);
		matRotZ.m[1][1] = cosf(fTheta);
		matRotZ.m[2][2] = 1;
		matRotZ.m[3][3] = 1;

		// Rotation X
		matRotX.m[0][0] = 1;
		matRotX.m[1][1] = cosf(fTheta * 0.5f);
		matRotX.m[1][2] = sinf(fTheta * 0.5f);
		matRotX.m[2][1] = -sinf(fTheta * 0.5f);
		matRotX.m[2][2] = cosf(fTheta * 0.5f);
		matRotX.m[3][3] = 1;

		for(auto tri : cube.tris)
		{
			Triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

			// Rotate in Z-Axis
			triRotatedZ.p[0] = tri.p[0] * matRotZ;
			triRotatedZ.p[1] = tri.p[1] * matRotZ;
			triRotatedZ.p[2] = tri.p[2] * matRotZ;

			// Rotate in X-Axis
			triRotatedZX.p[0] = triRotatedZ.p[0] * matRotX;
			triRotatedZX.p[1] = triRotatedZ.p[1] * matRotX;
			triRotatedZX.p[2] = triRotatedZ.p[2] * matRotX;

			// Offset into the screen
			triTranslated = triRotatedZX;
			triTranslated.p[0].z += 3.0f;
			triTranslated.p[1].z += 3.0f;
			triTranslated.p[2].z += 3.0f;

			// Project triangles from 3D --> 2D
			triProjected.p[0] = triTranslated.p[0] * matProj;
			triProjected.p[1] = triTranslated.p[1] * matProj;
			triProjected.p[2] = triTranslated.p[2] * matProj;

			// Scale into view
			triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
			triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
			triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;

			triProjected.p[0].x *= 0.5f * w->dimensions.x;
			triProjected.p[0].y *= 0.5f * w->dimensions.y;
			triProjected.p[1].x *= 0.5f * w->dimensions.x;
			triProjected.p[1].y *= 0.5f * w->dimensions.y;
			triProjected.p[2].x *= 0.5f * w->dimensions.x;
			triProjected.p[2].y *= 0.5f * w->dimensions.y;
			w->DrawTriangle(triProjected.p[0].xy(),
				triProjected.p[1].xy(),
				triProjected.p[2].xy(),
				Vector4(255,255,0,0), true);
			std::cout << diff << std::endl;
		}

		w->Update();
	}
}

