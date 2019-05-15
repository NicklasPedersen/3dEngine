#pragma once
#include <Windows.h>
#include <windowsx.h>
#include "Vector2.h"
#include "Vector4.h"
#include "Mathf.h"
#include <iostream>
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")
class Window
{
public:
	static int windows;
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	bool isMoving;
	Vector2 mousePos;
	LRESULT CALLBACK RealWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	Gdiplus::Bitmap *bitmap;
	Gdiplus::ColorMatrix clrMatrix;
public:
	Vector2 dimensions;
	int key;
	const char g_szClassName[14] = "myWindowClass";
	WNDCLASSEX wc;
	HWND hwnd;
	MSG msg;
	HINSTANCE instance;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput = { };
	ULONG_PTR gdiplusToken = NULL;
	Window();
	void Update();
	bool IsAlive();
	RECT Rectangle();
	Vector2 TopLeft();
	Vector2 BottomRight();
	Vector2 Dimensions();
	void DrawPixel(Vector2 position, Vector4 argb);
	void BitmapSetPixel(Vector2 position, Vector4 argb);
	void BitmapShow();
	void Clear(Vector4 argb);
	void DrawLine(Vector2 p1, Vector2 p2, Vector4 argb);
	void DrawVerticalLine(Vector2 p1, int length, Vector4 argb);
	void DrawFlatBottomTriangle(Vector2 v1, Vector2 v2, Vector2 v3, Vector4 argb);
	void DrawFlatTopTriangle(Vector2 v1, Vector2 v2, Vector2 v3, Vector4 argb);
	void DrawTriangle(Vector2 p1, Vector2 p2, Vector2 p3, Vector4 argb, bool fill);
};