#pragma once
#include <Windows.h>
#include <windowsx.h>
#include <iostream>
#include "Vector2.h"
class Window
{
public:
	static int windows;
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	bool isMoving;
	Vector2 mousePos;
	LRESULT CALLBACK RealWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
public:
	int key;
	const char g_szClassName[14] = "myWindowClass";
	WNDCLASSEX wc;
	HWND hwnd;
	MSG msg;
	HINSTANCE instance;
	Window();
	void Update();
	bool IsAlive();
	void DrawPixel(int x, int y, COLORREF c);
};