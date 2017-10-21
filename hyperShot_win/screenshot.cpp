//	screenshot.cpp: All screenshoting functions
//

#include "stdafx.h"
#include "screenshot.h"


HBITMAP GetScreenshot(int x, int y, int width, int height) {
	HDC hdcScreen;
	HDC hdcMemory;
	HBITMAP hBitmap;

	// We need to set proccess as DPI aware to get right widht and height for scaled systems
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

	// Get the height and width of the screen 
	if (width == 0) width = GetSystemMetrics(SM_CXSCREEN);
	if (height == 0) height = GetSystemMetrics(SM_CYSCREEN);

	#ifdef _DEBUG
		TCHAR buffer[253];
		swprintf(buffer, sizeof(buffer), _T("widht: %d height: %d\n"), width, height);
		OutputDebugString(buffer);
	#endif // DEBUG

	// Get the device context of the screen
	hdcScreen = GetDC(NULL);

	// Create a device context to put it in
	hdcMemory = CreateCompatibleDC(hdcScreen);

	// Create a bitmap
	hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);

	// Get a new bitmap
	HBITMAP holdBitmap = (HBITMAP) SelectObject(hdcMemory, hBitmap);

	// Copy from the screen DC to the compatible DC
	BitBlt(hdcMemory, x, y, width, height, hdcScreen, 0, 0, SRCCOPY);

	// Back to the original bitmap
	hBitmap = (HBITMAP)SelectObject(hdcMemory, holdBitmap);
	
	// Clean up
	ReleaseDC(NULL, hdcScreen);
	DeleteObject(hdcMemory);
	DeleteDC(hdcMemory);
	
	return hBitmap;
}