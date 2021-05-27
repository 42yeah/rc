#pragma once

#include <Windows.h>

class ScreenCap {
public:
	ScreenCap();

	~ScreenCap();

	void shot();

private:
	HDC screen_dc, memory_dc;
	int width, height;
	HBITMAP bitmap, old_bitmap;
};

