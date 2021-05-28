#pragma once

#include <Windows.h>
#include <SDL.h>


class ScreenCap {
public:
	ScreenCap(SDL_Renderer* renderer);

	~ScreenCap();

	void shot(SDL_Renderer* renderer);

	SDL_Texture* get_texture();

	const char* get_data() const;

	unsigned int get_data_size() const;

private:
	SDL_Texture* texture;
	HDC screen_dc, memory_dc;
	int width, height, x, y;
	HBITMAP bitmap;
	BYTE* bbits;
	SDL_Surface* surface;
};

