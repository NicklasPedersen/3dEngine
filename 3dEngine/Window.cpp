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

	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

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
		WS_OVERLAPPEDWINDOW,
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

	Vector2 dim = Dimensions();
	bitmap = new Gdiplus::Bitmap(dim.x, dim.y, PixelFormat32bppARGB);
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

RECT Window::Rect()
{
	RECT r;
	GetWindowRect(hwnd, &r);
	return r;
}

Vector2 Window::TopLeft()
{
	RECT r = Rect();
	return Vector2(r.left, r.top);
}

Vector2 Window::BottomRight()
{
	RECT r = Rect();
	return Vector2(r.right, r.bottom);
}

Vector2 Window::Dimensions()
{
	return BottomRight() - TopLeft();
}


void Window::DrawPixel(int x, int y, UINT argb)
{
	Gdiplus::Graphics g(hwnd);
	Gdiplus::SolidBrush b(Gdiplus::ARGB(0xFFFF0000));
	g.SetSmoothingMode(Gdiplus::SmoothingModeNone);
	g.FillRectangle(&b, x, y, 1, 1);
}


void Window::Clear(int r, int g, int b)
{
	Gdiplus::Graphics gr(hwnd);
	gr.Clear(Gdiplus::ARGB(0x00000000));
}

void Window::ClearScreen(int r, int g, int b)
{
	Gdiplus::Graphics gr(hwnd);
	gr.Clear(Gdiplus::ARGB(0x00000000));
}

void Window::DrawTriangle(Vector2 p1, Vector2 p2, Vector2 p3, COLORREF c)
{
	Gdiplus::SolidBrush b(Gdiplus::ARGB(0xFFFF0000));
	Gdiplus::Graphics gr(hwnd);
	Gdiplus::PointF ps[3] = { { p1.x, p1.y}, {p2.x, p2.y}, {p3.x, p3.y} };
	gr.FillPolygon(&b, ps, 3);
}
