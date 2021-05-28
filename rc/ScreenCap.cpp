#include "ScreenCap.h"
#include <iostream>
#include <cassert>
#include "common.h"


ScreenCap::ScreenCap(SDL_Renderer* renderer) {
    BITMAPINFOHEADER bi_header;
    BITMAPINFO b_info;
    HGDIOBJ temp_bitmap;
    BITMAP all_desktops;
    bbits = nullptr;
    HANDLE hHeap = GetProcessHeap();
    DWORD count_bits, dw_written = 0;
    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = GetSystemMetrics(SM_YVIRTUALSCREEN);

    ZeroMemory(&bi_header, sizeof(BITMAPINFOHEADER));
    ZeroMemory(&b_info, sizeof(BITMAPINFO));
    ZeroMemory(&all_desktops, sizeof(BITMAP));

    screen_dc = GetDC(NULL);
    temp_bitmap = GetCurrentObject(screen_dc, OBJ_BITMAP);
    GetObjectW(temp_bitmap, sizeof(BITMAP), &all_desktops);

    width = all_desktops.bmWidth;
    height = all_desktops.bmHeight;

    DeleteObject(temp_bitmap);

    bi_header.biSize = sizeof(BITMAPINFOHEADER);
    bi_header.biBitCount = 24;
    bi_header.biCompression = BI_RGB;
    bi_header.biPlanes = 1;
    bi_header.biWidth = width;
    bi_header.biHeight = height;

    b_info.bmiHeader = bi_header;

    count_bits = (((24 * width + 31) & ~31) / 8) * height;

    memory_dc = CreateCompatibleDC(screen_dc);
    bitmap = nullcheck(CreateDIBSection(screen_dc, &b_info, DIB_RGB_COLORS, (VOID**) &bbits, NULL, 0));
    SelectObject(memory_dc, bitmap);

    BitBlt(memory_dc, 0, 0, width, height, screen_dc, x, y, SRCCOPY);
    surface = SDL_CreateRGBSurfaceFrom(bbits, width, height, 24, width * 3, 0xff0000, 0x00ff00, 0x0000ff, 0x000000);
    texture = SDL_CreateTexture(renderer, surface->format->format, SDL_TEXTUREACCESS_STREAMING, surface->w, surface->h);
}

ScreenCap::~ScreenCap() {
    SDL_DestroyTexture(texture);
    ReleaseDC(NULL, screen_dc);
	DeleteDC(memory_dc);
    DeleteObject(bitmap);
}

void ScreenCap::shot(SDL_Renderer* renderer) {
    BitBlt(memory_dc, 0, 0, width, height, screen_dc, x, y, SRCCOPY);

    void* locked_ptr = nullptr;
    int pitch;
    assert(SDL_LockTexture(texture, nullptr, &locked_ptr, &pitch) >= 0);
    std::memcpy(locked_ptr, surface->pixels, surface->pitch * surface->h);
    SDL_UnlockTexture(texture);
}

SDL_Texture* ScreenCap::get_texture() {
    return texture;
}

const char* ScreenCap::get_data() const {
    return (const char*) bbits;
}

unsigned int ScreenCap::get_data_size() const{
    return surface->pitch * surface->h;
}
