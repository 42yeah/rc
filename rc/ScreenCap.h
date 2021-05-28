#pragma once

#include <sstream>
#include <Windows.h>
#include <SDL.h>
#include <turbojpeg.h>


// TurboJPEG JPEG buffer, managed by... This struct.
struct Buffer {
public:
	Buffer(unsigned char* data, int len);

	~Buffer();

	unsigned char* data;
	int len;
};

class ScreenCap {
public:
	ScreenCap(SDL_Renderer* renderer);

	~ScreenCap();

	void shot(SDL_Renderer* renderer);

	SDL_Texture* get_texture();

	std::unique_ptr<Buffer> get_jpeg_data() const;

private:
	SDL_Texture* texture;
	HDC screen_dc, memory_dc;
	int width, height, x, y;
	HBITMAP bitmap;
	BYTE* bbits;
	SDL_Surface* surface;
	tjhandle tj_instance;
};

