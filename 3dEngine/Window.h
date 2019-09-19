#pragma once
#include <Windows.h>
#include <windowsx.h>
#include "Vector2.h"
#include "Vector4.h"
#include "Mathf.h"
#include "Helper.h"
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
	bool isBuffered;
	BYTE *buffer;
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
	Window(int width, int height);
	void Update();
	bool IsAlive();
	RECT Rectangle();
	Vector2 TopLeft();
	Vector2 BottomRight();
	Vector2 Dimensions();
	void DrawPixel(Vector2 position, Vector4 argb);
	void DrawPixel(BYTE *data, int x, int y, int width, int height, Vector4 argb);
	void BitmapSetPixel(Vector2 position, Vector4 argb);
	void BitmapShow();
	void BufferShow();
	void BufferClear(Vector4 argb);
	void BufferDrawLineLow(int x0, int y0, int x1, int y1, Vector4 argb);
	void BufferDrawLineHigh(int x0, int y0, int x1, int y1, Vector4 argb);
	void BufferDrawVerticalLine(int y0, int y1, int x, Vector4 argb);
	void BufferDrawHorizontalLine(int x0, int x1, int y, Vector4 argb);
	void BufferDrawLine(Vector2 v0, Vector2 v1, Vector4 argb);
	void BufferDrawFlatBottomTriangle(Vector2 v0, Vector2 v1, Vector2 v2, Vector4 argb);
	void BufferDrawFlatTopTriangle(Vector2 v0, Vector2 v1, Vector2 v2, Vector4 argb);
	void BufferDrawTriangle(Vector2 v0, Vector2 v1, Vector2 v2, Vector4 argb);
	void BufferFillTriangle(Vector2 v0, Vector2 v1, Vector2 v2, Vector4 argb);
	void Clear(Vector4 argb);
	void DrawLineLow(int x0, int y0, int x1, int y1, Vector4 argb);
	void DrawLineHigh(int x0, int y0, int x1, int y1, Vector4 argb);
	void DrawVerticalLine(int y0, int y1, int x, Vector4 argb);
	void DrawHorizontalLine(int x0, int x1, int y, Vector4 argb);
	void DrawLine1(Vector2 v0, Vector2 v1, Vector4 argb);
	void DrawLine(Vector2 v0, Vector2 v1, Vector4 argb);
	void DrawVerticalLine(Vector2 v, int length, Vector4 argb);
	void DrawFlatBottomTriangle(Vector2 v0, Vector2 v1, Vector2 v2, Vector4 argb);
	void DrawFlatTopTriangle(Vector2 v0, Vector2 v1, Vector2 v2, Vector4 argb);
	void DrawTriangle(Vector2 v0, Vector2 v1, Vector2 v2, Vector4 argb, bool fill);
	void DrawTriangle(Vector2 v0, Vector2 v1, Vector2 v2, Vector4 argb);
	void FillTriangle(Vector2 v0, Vector2 v1, Vector2 v2, Vector4 argb);
	void DrawTriangleSlow(Vector2 v0, Vector2 v1, Vector2 v2, Vector4 argb);
};