//	screenshot.cpp: All screenshoting functions
//

#include "stdafx.h"
#include "screenshot.h"


HBITMAP GetScreenshot(int x, int y, int width, int height, int flag) {
	HDC hdcScreen;
	HDC hdcMemory;
	HBITMAP hBitmap;

	// We need to set proccess as DPI aware to get right widht and height for scaled systems
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

	// Get the height and width of the screen if fullscreen flag is set
	if (flag == FULLSCREEN) {
		x = GetSystemMetrics(SM_XVIRTUALSCREEN);
		y = GetSystemMetrics(SM_YVIRTUALSCREEN);
		width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	}
	

	#ifdef _DEBUG
		TCHAR buffer[253];
		swprintf(buffer, sizeof(buffer), _T("widht: %d height: %d\nx: %d y: %d\n"), width, height, x, y);
		OutputDebugString(buffer);
	#endif // DEBUG

	// Get the device context of the screen
	hdcScreen = GetDC(NULL);

	// Create a device context to put it in
	hdcMemory = CreateCompatibleDC(hdcScreen);

	// Create a bitmap
	hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);

	// Get a new bitmap
	HBITMAP hOldBitmap = (HBITMAP) SelectObject(hdcMemory, hBitmap);

	// Copy from the screen DC to the compatible DC
	BitBlt(hdcMemory, 0, 0, width, height, hdcScreen, x, y, SRCCOPY);

	// Back to the original bitmap
	hBitmap = (HBITMAP)SelectObject(hdcMemory, hOldBitmap);

	// Set bitmap dimensions for later
	SetBitmapDimensionEx(hBitmap, width, height, NULL);
	
	// Clean up
	ReleaseDC(NULL, hdcScreen);
	DeleteObject(hdcMemory);
	DeleteDC(hdcMemory);

	return hBitmap;
}