#include <windows.h>
#include <iostream>
#include "Window.h"

Window *w = new Window();


int main()
{
	while(w->IsAlive())
	{
		w->Update();
		w->Clear(RGB(0,0,0));
		w->SetTitle("Hello world!");
		if(w->key == 'K')
		{
			for(int i = 0; i < 10; i++)
				for(int j = 0; j < 10; j++)
					w->DrawPixel(i,j, RGB(255,0,0));
		}
	}
}

