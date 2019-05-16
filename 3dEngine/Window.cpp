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
		CW_USEDEFAULT, CW_USEDEFAULT, 500, 500,
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

	dimensions = Dimensions();
	bitmap = new Gdiplus::Bitmap((INT)dimensions.x, (INT)dimensions.y, PixelFormat32bppARGB);
}

void Window::Update()
{
	dimensions = Dimensions();
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

RECT Window::Rectangle()
{
	RECT r;
	GetWindowRect(hwnd, &r);
	return r;
}

Vector2 Window::TopLeft()
{
	RECT r = Rectangle();
	return Vector2(r.left, r.top);
}

Vector2 Window::BottomRight()
{
	RECT r = Rectangle();
	return Vector2(r.right, r.bottom);
}

Vector2 Window::Dimensions()
{
	return BottomRight() - TopLeft();
}


void Window::DrawPixel(Vector2 position, Vector4 argb)
{
	Gdiplus::Graphics g(hwnd);
	Gdiplus::SolidBrush b(Gdiplus::Color(argb.x, argb.y, argb.z, argb.w));
	g.SetSmoothingMode(Gdiplus::SmoothingModeNone);
	g.FillRectangle(&b, (INT)position.x, (INT)position.y, 1, 1);
}

void Window::BitmapSetPixel(Vector2 position, Vector4 argb)
{
	bitmap->SetPixel(position.x, position.y, Gdiplus::Color(argb.x, argb.y, argb.z, argb.w));
}

void Window::BitmapShow()
{

	Gdiplus::Graphics gr(hwnd);
	Gdiplus::Rect r = { 0, 0, (INT)dimensions.x, (INT)dimensions.y };
	gr.DrawImage(bitmap, r);
}


void Window::Clear(Vector4 argb)
{
	Gdiplus::BitmapData bmd;
	Gdiplus::Rect r = { 0, 0, (INT)dimensions.x, (INT)dimensions.y };
	if(Gdiplus::Ok == bitmap->LockBits(&Gdiplus::Rect(0, 0, bitmap->GetWidth(), bitmap->GetHeight()), Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bmd))
	{
		BYTE* dptr = (BYTE *) bmd.Scan0;
		for(int i = 0; i < bmd.Width * bmd.Height; i++)
		{
			*(dptr + i * 4) = argb.w;
			*(dptr + i * 4 + 1) = argb.z;
			*(dptr + i * 4 + 2) = argb.y;
			*(dptr + i * 4 + 3) = argb.x;
		}
		bitmap->UnlockBits(&bmd);
	}
}

void Window::DrawLineLow(int x0, int y0, int x1, int y1, Vector4 argb)
{
	Gdiplus::BitmapData bmd;
	if (Gdiplus::Ok != bitmap->LockBits(&Gdiplus::Rect(0, 0, bitmap->GetWidth(), bitmap->GetHeight()), Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bmd))
		return;
	BYTE* dptr = (BYTE *)bmd.Scan0;

	int dx = x1 - x0;
	int dy = y1 - y0;
	int yi = 1;
	if (dy < 0)
	{
		yi = -1;
		dy = -dy;
	}
	float D = 2 * dy - dx;
	float y = y0;
	for (int x = x0; x <= x1; x++)
	{
		DrawPixel(dptr, x, y, bmd.Width, bmd.Width, argb);
		if (D > 0)
		{
			y = y + yi;
			D = D - 2 * dx;
		}
		D = D + 2 * dy;
	}
	bitmap->UnlockBits(&bmd);
}

void Window::DrawLineHigh(int x0, int y0, int x1, int y1, Vector4 argb)
{
	Gdiplus::BitmapData bmd;
	if (Gdiplus::Ok != bitmap->LockBits(&Gdiplus::Rect(0, 0, bitmap->GetWidth(), bitmap->GetHeight()), Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bmd))
		return;
	BYTE* dptr = (BYTE *)bmd.Scan0;

	int dx = x1 - x0;
	int dy = y1 - y0;
	float xi = 1;
	if (dx < 0)
	{
		xi = -1;
		dx = -dx;
	}
	float D = 2 * dx - dy;
	float x = x0;
	for (int y = y0; y <= y1; y++)
	{
		DrawPixel(dptr, x, y, bmd.Width, bmd.Width, argb);
		if (D > 0)
		{
			x += xi;
			D -= 2 * dy;
		}
		D += 2 * dx;
	}
	bitmap->UnlockBits(&bmd);
}

void Window::DrawVerticalLine(int y0, int y1, int x, Vector4 argb)
{
	Gdiplus::BitmapData bmd;
	if (Gdiplus::Ok != bitmap->LockBits(&Gdiplus::Rect(0, 0, bitmap->GetWidth(), bitmap->GetHeight()), Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bmd))
		return;
	BYTE* dptr = (BYTE *)bmd.Scan0;

	if (y0 > y1)
	{
		int temp = y0;
		y0 = y1;
		y1 = temp;
	}
	for (int y = y0; y <= y1; y++)
		DrawPixel(dptr, x, y, bmd.Width, bmd.Height, argb);
	
	bitmap->UnlockBits(&bmd);
}

void Window::DrawHorizontalLine(int x0, int x1, int y, Vector4 argb)
{
	Gdiplus::BitmapData bmd;
	if (Gdiplus::Ok != bitmap->LockBits(&Gdiplus::Rect(0, 0, bitmap->GetWidth(), bitmap->GetHeight()), Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bmd))
		return;
	BYTE* dptr = (BYTE *)bmd.Scan0;

	if (x0 > x1)
	{
		int temp = x0;
		x0 = x1;
		x1 = temp;
	}
	for (int x = x0; x <= x1; x++)
		DrawPixel(dptr, x, y, bmd.Width, bmd.Height, argb);

	bitmap->UnlockBits(&bmd);
}

void Window::DrawPixel(BYTE *data, int x, int y, int width, int height, Vector4 argb)
{
	if (x < 0 || x >= width || y < 0 || y >= width)
		return;
	BYTE *start = data + (int)x * 4 + (int)y * width * 4;
	*(start) = argb.w;
	*(start + 1) = argb.z;
	*(start + 2) = argb.y;
	*(start + 3) = argb.x;
}

void Window::DrawLine(Vector2 p0, Vector2 p1, Vector4 argb)
{
	p0.ToInt();
	p1.ToInt();
	Vector2 delta = p1 - p0;
	delta.ToInt();
	if (delta.x == 0)
	{
		DrawVerticalLine(p0.y, p1.y, p0.x, argb);
	}
	else if (delta.y == 0)
	{
		DrawHorizontalLine(p0.x, p1.x, p0.y, argb);
	}
	else if (Mathf::Abs(delta.x) > Mathf::Abs(delta.y))
	{
		if (p0.x > p1.x)
		{
			DrawLineLow(p1.x, p1.y, p0.x, p0.y, argb);
		}
		else
		{
			DrawLineLow(p0.x, p0.y, p1.x, p1.y, argb);
		}
	}
	else
	{
		if (p0.y > p1.y)
		{
			DrawLineHigh(p1.x, p1.y, p0.x, p0.y, argb);
		}
		else
		{
			DrawLineHigh(p0.x, p0.y, p1.x, p1.y, argb);
		}
	}
}

void Window::DrawLine1(Vector2 p1, Vector2 p2, Vector4 argb)
{
	p1 = Vector2((int)p1.x, (int)p1.y);
	p2 = Vector2((int)p2.x, (int)p2.y);
	Vector2 delta = p2 - p1;
	if(delta.x == 0)
	{
		if(p1.y < p2.y)
			DrawVerticalLine(p1, Mathf::Abs(delta.y), argb);
		else
			DrawVerticalLine(p2, Mathf::Abs(delta.y), argb);
		return;
	}
	
	Gdiplus::BitmapData bmd;
	Gdiplus::Rect r = { 0, 0, (INT)dimensions.x, (INT)dimensions.y };
	if(Gdiplus::Ok == bitmap->LockBits(&Gdiplus::Rect(0, 0, bitmap->GetWidth(), bitmap->GetHeight()), Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bmd))
	{
		float deltaerr = Mathf::Abs(delta.y / delta.x);
		float error = 0;
		int y = p1.y;
		BYTE* dptr = (BYTE *) bmd.Scan0;
		for(int x = p1.x; x <= (int)p2.x; x++)
		{
			DrawPixel(dptr, x, y, bmd.Width, bmd.Height, argb);
			error += deltaerr;
			while(error >= 0.5f)
			{
				y += Mathf::Sign((int)delta.y);
				error -= 1.0f;
			}
		}
		bitmap->UnlockBits(&bmd);
	}
}

void Window::DrawVerticalLine(Vector2 p1, int length, Vector4 argb)
{
	Gdiplus::BitmapData bmd;
	if(Gdiplus::Ok == bitmap->LockBits(&Gdiplus::Rect(0, 0, bitmap->GetWidth(), bitmap->GetHeight()), Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bmd))
	{
		BYTE* dptr = (BYTE *) bmd.Scan0;
		for(int i = 0; i < length; i++)
		{
			DrawPixel(dptr, p1.x, p1.y + i, bmd.Width, bmd.Height, argb);
		}
		bitmap->UnlockBits(&bmd);
	}
}

void Window::DrawFlatBottomTriangle(Vector2 v1, Vector2 v2, Vector2 v3, Vector4 argb)
{
	float invslope1 = (v2.x - v1.x) / (v2.y - v1.y);
	float invslope2 = (v3.x - v1.x) / (v3.y - v1.y);

	float curx1 = v1.x;
	float curx2 = v1.x;

	for (int scanlineY = v1.y; scanlineY <= v2.y; scanlineY++)
	{
		DrawHorizontalLine(curx1, curx2, scanlineY, argb);
		curx1 += invslope1;
		curx2 += invslope2;
	}
}

void Window::DrawFlatTopTriangle(Vector2 v1, Vector2 v2, Vector2 v3, Vector4 argb)
{
	float invslope1 = (v3.x - v1.x) / (v3.y - v1.y);
	float invslope2 = (v3.x - v2.x) / (v3.y - v2.y);

	float curx1 = v3.x;
	float curx2 = v3.x;

	for (int scanlineY = v3.y; scanlineY > v1.y; scanlineY--)
	{
		DrawHorizontalLine(curx1, curx2, scanlineY, argb);
		curx1 -= invslope1;
		curx2 -= invslope2;
	}
}

void Window::DrawTriangle(Vector2 p0, Vector2 p1, Vector2 p2, Vector4 argb, bool fill)
{
	p0.ToInt();
	p1.ToInt();
	p2.ToInt();
	if (p0.y > p1.y)
	{
		Vector2 temp = p0;
		p0 = p1;
		p1 = temp;
	}
	if (p0.y > p2.y)
	{
		Vector2 temp = p0;
		p0 = p2;
		p2 = temp;
	}
	if (p1.y > p2.y)
	{
		Vector2 temp = p1;
		p1 = p2;
		p2 = temp;
	}

	DrawLine(p0, p1, argb);
	DrawLine(p0, p2, argb);
	DrawLine(p1, p2, argb);

	if(!fill)
		return;

	if(p1.y == p2.y)
	{
		DrawFlatBottomTriangle(p0, p1, p2, argb);
	}
	else if(p0.y == p1.y)
	{
		DrawFlatTopTriangle(p0, p1, p2, argb);
	}
	else
	{
		Vector2 v3 = Vector2((p0.x + ((p1.y - p0.y) / (p2.y - p0.y)) * (p2.x - p0.x)), p1.y);
		DrawFlatBottomTriangle(p0, p1, v3, argb);
		DrawFlatTopTriangle(p1, v3, p2, argb);
	}
}

void Window::DrawTriangleSlow(Vector2 p1, Vector2 p2, Vector2 p3, Vector4 argb)
{
	Gdiplus::BitmapData bmd;
	if (!Gdiplus::Ok == bitmap->LockBits(&Gdiplus::Rect(0, 0, bitmap->GetWidth(), bitmap->GetHeight()), Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bmd))
		return;

	BYTE* dptr = (BYTE *)bmd.Scan0;

	int minX = Mathf::Min(p1.x, Mathf::Min(p2.x, p3.x));
	int maxX = Mathf::Max(p1.x, Mathf::Max(p2.x, p3.x));
	int minY = Mathf::Min(p1.y, Mathf::Min(p2.y, p3.y));
	int maxY = Mathf::Max(p1.y, Mathf::Max(p2.y, p3.y));

	Vector2 vs1 = Vector2(p2.x - p1.x, p2.y - p1.y);
	Vector2 vs2 = Vector2(p3.x - p1.x, p3.y - p1.y);

	for (int x = minX; x <= maxX; x++)
	{
		for (int y = minY; y <= maxY; y++)
		{
			Vector2 q = Vector2(x - p1.x, y - p1.y);

			float s = q.Cross(vs2) / vs1.Cross(vs2);
			float t = vs1.Cross(q) / vs1.Cross(vs2);

			if ((s >= 0) && (t >= 0) && (s + t <= 1))
			{ /* inside triangle */
				DrawPixel(dptr, x, y, bmd.Width, bmd.Height, argb);
			}
		}
	}

	bitmap->UnlockBits(&bmd);
}
