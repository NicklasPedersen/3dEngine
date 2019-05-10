#include "Window.h"

int Window::windows = 0;

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* me = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	if (me) return me->RealWndProc(hwnd, msg, wParam, lParam);
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::RealWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		key = wParam;
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

Window::Window()
{
	instance = GetModuleHandle(nullptr);

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = instance;
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = g_szClassName + windows++;
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(nullptr, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return;
	}

	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		g_szClassName,
		"The title of my window",
		0,
		CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
		nullptr, nullptr, instance, nullptr);

	if (!hwnd)
	{
		MessageBox(nullptr, "Window Creation Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return;
	}
	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG)this);

	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);
}

void Window::Update()
{
	while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

bool Window::IsAlive()
{
	return IsWindow(hwnd);
}

void Window::SetTitle(const char *str)
{
	SetWindowText(hwnd, str);
}

void Window::DrawPixel(int x, int y, COLORREF c)
{
	HDC hDc = GetDC(hwnd);
	SetPixel(hDc, x, y, c);
	ReleaseDC(hwnd, hDc);
	DeleteDC(hDc);
}

RECT Window::GetRect()
{
	RECT r;
	GetWindowRect(hwnd, &r);
	return r;
}

Vector2 Window::GetTopLeft()
{
	RECT r = GetRect();
	return Vector2(r.left, r.top);
}

Vector2 Window::GetBottomRight()
{
	RECT r = GetRect();
	return Vector2(r.right, r.bottom);
}

Vector2 Window::GetDimensions()
{
	return GetBottomRight() - GetTopLeft();
}

void Window::Clear(COLORREF c)
{
        BITMAP bm;
        PAINTSTRUCT ps;

        HDC hdc = BeginPaint(hwnd, &ps);

        HDC hdcMem = CreateCompatibleDC(hdc);
		Vector2 size = GetDimensions();
		char black[4] = { 0, 0, 0, 0 };
		BITMAP k = { 0, size.x, size.y, 2, 1, 32, black };
		HBITMAP b = (HBITMAP) k;

        BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

        SelectObject(hdcMem, hbmOld);
        DeleteDC(hdcMem);

        EndPaint(hwnd, &ps);
}

