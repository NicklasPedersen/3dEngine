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
	isBuffered = false;

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

Window::Window(int width, int height)
{
	isBuffered = true;
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
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME,
		CW_USEDEFAULT, CW_USEDEFAULT, width, height,
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

	dimensions = Vector2(width, height);
	buffer = (BYTE *)malloc(width * height * 4);
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
	Vector2 v(r.left, r.top);
	return v;
}

Vector2 Window::BottomRight()
{
	RECT r = Rectangle();
	Vector2 v(r.right, r.bottom);
	return v;
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

void Window::BufferShow()
{
	Gdiplus::Graphics gr(hwnd);
	Gdiplus::BitmapData bmd;
	Gdiplus::Bitmap bm(dimensions.x, dimensions.y, PixelFormat32bppARGB);
	Gdiplus::Rect bmRect(0, 0, bm.GetWidth(), bm.GetHeight());
	if (Gdiplus::Ok == bm.LockBits(&bmRect, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bmd))
	{
		memcpy(bmd.Scan0, buffer, bm.GetWidth() * bm.GetHeight() * 4);
		bm.UnlockBits(&bmd);
		gr.DrawImage(&bm, Gdiplus::Rect(0, 0, bm.GetWidth(), bm.GetHeight()));
	}
}

void Window::BufferClear(Vector4 argb)
{
	for (int i = 0; i < dimensions.Surface(); i++)
	{
		*(buffer + i * 4) = argb.w;
		*(buffer + i * 4 + 1) = argb.z;
		*(buffer + i * 4 + 2) = argb.y;
		*(buffer + i * 4 + 3) = argb.x;
	}
}

void Window::BufferDrawLineLow(int x0, int y0, int x1, int y1, Vector4 argb)
{

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
		DrawPixel(buffer, x, y, dimensions.x, dimensions.y, argb);
		if (D > 0)
		{
			y = y + yi;
			D = D - 2 * dx;
		}
		D = D + 2 * dy;
	}
}

void Window::BufferDrawLineHigh(int x0, int y0, int x1, int y1, Vector4 argb)
{
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
		DrawPixel(buffer, x, y, dimensions.x, dimensions.y, argb);
		if (D > 0)
		{
			x += xi;
			D -= 2 * dy;
		}
		D += 2 * dx;
	}
}

void Window::BufferDrawVerticalLine(int y0, int y1, int x, Vector4 argb)
{
	if (y0 > y1)
		Helper<int>::Swap(&y0, &y1);
	for (int y = y0; y <= y1; y++)
		DrawPixel(buffer, x, y, dimensions.x, dimensions.y, argb);
}

void Window::BufferDrawHorizontalLine(int x0, int x1, int y, Vector4 argb)
{
	if (x0 > x1)
		Helper<int>::Swap(&x0, &x1);
	for (int x = x0; x <= x1; x++)
		DrawPixel(buffer, x, y, dimensions.x, dimensions.y, argb);
}

void Window::BufferDrawLine(Vector2 v0, Vector2 v1, Vector4 argb)
{
	v0.Round();
	v1.Round();
	Vector2 delta = v1 - v0;
	delta.Round();
	if (delta.x == 0)
	{
		BufferDrawVerticalLine(v0.y, v1.y, v0.x, argb);
	}
	else if (delta.y == 0)
	{
		BufferDrawHorizontalLine(v0.x, v1.x, v0.y, argb);
	}
	else if (Mathf::Abs(delta.x) > Mathf::Abs(delta.y))
	{
		if (v0.x > v1.x)
		{
			BufferDrawLineLow(v1.x, v1.y, v0.x, v0.y, argb);
		}
		else
		{
			BufferDrawLineLow(v0.x, v0.y, v1.x, v1.y, argb);
		}
	}
	else
	{
		if (v0.y > v1.y)
		{
			BufferDrawLineHigh(v1.x, v1.y, v0.x, v0.y, argb);
		}
		else
		{
			BufferDrawLineHigh(v0.x, v0.y, v1.x, v1.y, argb);
		}
	}
}

void Window::BufferDrawFlatBottomTriangle(Vector2 v0, Vector2 v1, Vector2 v2, Vector4 argb)
{
	float invslope1 = (v1.x - v0.x) / (v1.y - v0.y);
	float invslope2 = (v2.x - v0.x) / (v2.y - v0.y);

	float curx1 = v0.x;
	float curx2 = v0.x;

	for (int scanlineY = v0.y; scanlineY <= v1.y; scanlineY++)
	{
		BufferDrawHorizontalLine(curx1, curx2, scanlineY, argb);
		curx1 += invslope1;
		curx2 += invslope2;
	}
}

void Window::BufferDrawFlatTopTriangle(Vector2 v0, Vector2 v1, Vector2 v2, Vector4 argb)
{
	float invslope1 = (v2.x - v0.x) / (v2.y - v0.y);
	float invslope2 = (v2.x - v1.x) / (v2.y - v1.y);

	float curx1 = v2.x;
	float curx2 = v2.x;

	for (int scanlineY = v2.y; scanlineY > v0.y; scanlineY--)
	{
		BufferDrawHorizontalLine(curx1, curx2, scanlineY, argb);
		curx1 -= invslope1;
		curx2 -= invslope2;
	}
}

void Window::BufferDrawTriangle(Vector2 v0, Vector2 v1, Vector2 v2, Vector4 argb)
{
}

void Window::BufferFillTriangle(Vector2 v0, Vector2 v1, Vector2 v2, Vector4 argb)
{
	v0.Round();
	v1.Round();
	v2.Round();

	if (v0.y > v1.y)
		Helper<Vector2>::Swap(&v0, &v1);
	if (v0.y > v2.y)
		Helper<Vector2>::Swap(&v0, &v2);
	if (v1.y > v2.y)
		Helper<Vector2>::Swap(&v1, &v2);

	if (v1.y == v2.y)
	{
		BufferDrawFlatBottomTriangle(v0, v1, v2, argb);
	}
	else if (v0.y == v1.y)
	{
		BufferDrawFlatTopTriangle(v0, v1, v2, argb);
	}
	else
	{
		Vector2 v3 = Vector2((v0.x + ((v1.y - v0.y) / (v2.y - v0.y)) * (v2.x - v0.x)), v1.y);
		BufferDrawFlatBottomTriangle(v0, v1, v3, argb);
		BufferDrawFlatTopTriangle(v1, v3, v2, argb);
	}
}

void Window::Clear(Vector4 argb)
{
	Gdiplus::BitmapData bmd;
	Gdiplus::Rect bmRect(0, 0, bitmap->GetWidth(), bitmap->GetHeight());
	if(Gdiplus::Ok == bitmap->LockBits(&bmRect, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bmd))
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
	Gdiplus::Rect bmRect(0, 0, bitmap->GetWidth(), bitmap->GetHeight());
	if (Gdiplus::Ok != bitmap->LockBits(&bmRect, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bmd))
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
	Gdiplus::Rect bmRect(0, 0, bitmap->GetWidth(), bitmap->GetHeight());
	if (Gdiplus::Ok != bitmap->LockBits(&bmRect, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bmd))
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
	Gdiplus::Rect bmRect(0, 0, bitmap->GetWidth(), bitmap->GetHeight());
	if (Gdiplus::Ok != bitmap->LockBits(&bmRect, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bmd))
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
	Gdiplus::Rect bmRect(0, 0, bitmap->GetWidth(), bitmap->GetHeight());

	if (Gdiplus::Ok != bitmap->LockBits(&bmRect, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bmd))
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
	p0.Round();
	p1.Round();
	Vector2 delta = p1 - p0;
	delta.Round();
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
	Gdiplus::Rect bmRect(0, 0, bitmap->GetWidth(), bitmap->GetHeight());
	if(Gdiplus::Ok == bitmap->LockBits(&bmRect, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bmd))
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
	Gdiplus::Rect bmRect(0, 0, bitmap->GetWidth(), bitmap->GetHeight());
	if(Gdiplus::Ok == bitmap->LockBits(&bmRect, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bmd))
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

void Window::DrawTriangle(Vector2 v0, Vector2 v1, Vector2 v2, Vector4 argb, bool fill)
{
	v0.Round();
	v1.Round();
	v2.Round();

	DrawLine(v0, v1, argb);
	DrawLine(v0, v2, argb);
	DrawLine(v1, v2, argb);

	if (!fill)
		return;

	if (v0.y > v1.y)
		Helper<Vector2>::Swap(&v0, &v1);
	if (v0.y > v2.y)
		Helper<Vector2>::Swap(&v0, &v2);
	if (v1.y > v2.y)
		Helper<Vector2>::Swap(&v1, &v2);


	if (v1.y == v2.y)
	{
		DrawFlatBottomTriangle(v0, v1, v2, argb);
	}
	else if (v0.y == v1.y)
	{
		DrawFlatTopTriangle(v0, v1, v2, argb);
	}
	else
	{
		Vector2 v3 = Vector2((v0.x + ((v1.y - v0.y) / (v2.y - v0.y)) * (v2.x - v0.x)), v1.y);
		DrawFlatBottomTriangle(v0, v1, v3, argb);
		DrawFlatTopTriangle(v1, v3, v2, argb);
	}
}

void Window::DrawTriangle(Vector2 v0, Vector2 v1, Vector2 v2, Vector4 argb)
{
	v0.Round();
	v1.Round();
	v2.Round();

	DrawLine(v0, v1, argb);
	DrawLine(v0, v2, argb);
	DrawLine(v1, v2, argb);
}

void Window::FillTriangle(Vector2 v0, Vector2 v1, Vector2 v2, Vector4 argb)
{
	v0.Round();
	v1.Round();
	v2.Round();

	if (v0.y > v1.y)
		Helper<Vector2>::Swap(&v0, &v1);
	if (v0.y > v2.y)
		Helper<Vector2>::Swap(&v0, &v2);
	if (v1.y > v2.y)
		Helper<Vector2>::Swap(&v1, &v2);

	if (v1.y == v2.y)
	{
		DrawFlatBottomTriangle(v0, v1, v2, argb);
	}
	else if (v0.y == v1.y)
	{
		DrawFlatTopTriangle(v0, v1, v2, argb);
	}
	else
	{
		Vector2 v3 = Vector2((v0.x + ((v1.y - v0.y) / (v2.y - v0.y)) * (v2.x - v0.x)), v1.y);
		DrawFlatBottomTriangle(v0, v1, v3, argb);
		DrawFlatTopTriangle(v1, v3, v2, argb);
	}
}

void Window::DrawTriangleSlow(Vector2 p1, Vector2 p2, Vector2 p3, Vector4 argb)
{
	Gdiplus::BitmapData bmd;
	Gdiplus::Rect bmRect(0, 0, bitmap->GetWidth(), bitmap->GetHeight());
	if (!Gdiplus::Ok == bitmap->LockBits(&bmRect, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bmd))
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
