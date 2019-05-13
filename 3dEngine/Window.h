#pragma once
#include <Windows.h>
#include <windowsx.h>
#include "Vector2.h"
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
public:
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
	RECT Rect();
	Vector2 TopLeft();
	Vector2 BottomRight();
	Vector2 Dimensions();
	void DrawPixel(int x, int y, UINT rgb);
	void Clear(int r, int g, int b);
	void ClearScreen(int r, int g, int b);
	void DrawTriangle(Vector2 p1, Vector2 p2, Vector2 p3, COLORREF c);
};