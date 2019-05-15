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

void Window::DrawLine(Vector2 p1, Vector2 p2, Vector4 argb)
{
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
		float y = p1.y;
		BYTE* dptr = (BYTE *) bmd.Scan0;
		for(int x = p1.x; x <= p2.x; x++)
		{
			if(x < 0 || x >= bmd.Width || y < 0 || y >= bmd.Height)
				continue;
			BYTE *start = dptr + x * 4 + (int)y * bmd.Width * 4;
			*(start) = argb.w;
			*(start + 1) = argb.z;
			*(start + 2) = argb.y;
			*(start + 3) = argb.x;
			error += deltaerr;
			while(error >= 0.5f)
			{
				y += Mathf::Sign(delta.y);
				error -= 1.0f;
			}
		}
		bitmap->UnlockBits(&bmd);
	}
}

void Window::DrawVerticalLine(Vector2 p1, int length, Vector4 argb)
{
	Gdiplus::BitmapData bmd;
	Gdiplus::Rect r = { 0, 0, (INT)dimensions.x, (INT)dimensions.y };
	if(Gdiplus::Ok == bitmap->LockBits(&Gdiplus::Rect(0, 0, bitmap->GetWidth(), bitmap->GetHeight()), Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bmd))
	{
		BYTE* dptr = (BYTE *) bmd.Scan0;
		for(int i = 0; i < length; i++)
		{
			if(p1.x < 0 || p1.x >= bmd.Width || (p1.y + i) < 0 || (p1.y + i) >= bmd.Height)
				continue;
			BYTE *start = dptr + (INT)p1.x * 4 + ((INT)p1.y + i) * bmd.Width * 4;
			*(start) = argb.w;
			*(start + 1) = argb.z;
			*(start + 2) = argb.y;
			*(start + 3) = argb.x;
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
		DrawLine(Vector2((int)curx1, scanlineY), Vector2((int)curx2, scanlineY), argb);
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
		DrawLine(Vector2((int)curx1, scanlineY), Vector2((int)curx2, scanlineY), argb);
		curx1 -= invslope1;
		curx2 -= invslope2;
	}
}

void Window::DrawTriangle(Vector2 p1, Vector2 p2, Vector2 p3, Vector4 argb, bool fill)
{
	Vector2 v[3] = { p1, p2, p3 };
	bool isSorted;
	do
	{
		isSorted = true;
		for(int i = 0; i < 2; i++)
		{
			if(v[i].y > v[i + 1].y)
			{
				isSorted = false;
				Vector2 temp = Vector2(v[i]);
				v[i] = Vector2(v[i + 1]);
				v[i + 1] = Vector2(v[i]);
			}
		}
	} while(!isSorted);

	DrawLine(p1, p2, argb);
	DrawLine(p1, p3, argb);
	DrawLine(p2, p3, argb);

	if(!fill)
		return;

	if(v[1].y == v[2].y)
	{
		DrawFlatBottomTriangle(v[0], v[1], v[2], argb);
	}
	else if(v[0].y == v[1].y)
	{
		DrawFlatTopTriangle(v[0], v[1], v[2], argb);
	}
	else
	{
		Vector2 v4 = Vector2((v[0].x + ((v[1].y - v[0].y) / (v[2].y - v[0].y)) * (v[2].x - v[0].x)), v[1].y);
		DrawFlatBottomTriangle(v[0], v[1], v4, argb);
		DrawFlatTopTriangle(v[1], v4, v[2], argb);
	}
}
