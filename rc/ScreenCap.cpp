#include "ScreenCap.h"
#include <fstream>


ScreenCap::ScreenCap() {
	screen_dc = CreateDC(LPCWSTR(L"DISPLAY"), nullptr, nullptr, nullptr);
	memory_dc = CreateCompatibleDC(screen_dc);

	width = GetDeviceCaps(screen_dc, HORZRES);
	height = GetDeviceCaps(screen_dc, VERTRES);

	bitmap = CreateCompatibleBitmap(screen_dc, width, height);
	old_bitmap = (HBITMAP) SelectObject(memory_dc, bitmap);
}

ScreenCap::~ScreenCap() {
	DeleteDC(screen_dc);
	DeleteDC(memory_dc);
}

void ScreenCap::shot() {
	BitBlt(memory_dc, 0, 0, width, height, screen_dc, 0, 0, SRCCOPY);
	bitmap = (HBITMAP) SelectObject(memory_dc, old_bitmap);

	std::ofstream writer("out.txt");
}
