//	preview_window.cpp: All functions and features concerning the preview window
//

#include "stdafx.h"
#include "preview_window.h"

HWND hWndPreview;
WCHAR szPreviewTitle[MAX_LOADSTRING];                  // The preview title bar text
WCHAR szPreviewWindowClass[MAX_LOADSTRING];            // the preview window class name
HBITMAP hScreenshot;

//
//  FUNCTION: PreviewRegisterClass()
//
//  PURPOSE: Registers the preview window class.
//
ATOM PreviewRegisterClass(HINSTANCE hInstance) {
	LoadStringW(hInstance, IDS_PREVIEW_TITLE, szPreviewTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_PREVIEWWIN, szPreviewWindowClass, MAX_LOADSTRING);

	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndPreviewProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HYPERSHOTWIN));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_HYPERSHOTWIN);
	wcex.lpszClassName = szPreviewWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitIPreview(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitIPreview(HINSTANCE hInstance, int nCmdShow, HBITMAP hBitmap) {

	hScreenshot = hBitmap;

	hWndPreview = CreateWindowW(szPreviewWindowClass, szPreviewTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_THICKFRAME,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWndPreview)
	{
		return FALSE;
	}

	ShowWindow(hWndPreview, nCmdShow);
	UpdateWindow(hWndPreview);

	return TRUE;
}

//
//  FUNCTION: WndPreviewProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the preview window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndPreviewProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(NULL, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			// Hides the window
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		HDC hdcMemory = CreateCompatibleDC(hdc);

		// select the new bitmap
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMemory, hScreenshot);

		// Not all off this needs to be done every paint call
		SIZE bitmapSize;

		GetBitmapDimensionEx(hScreenshot, &bitmapSize);

		double previewScale = 2.7; // OPTION: Make this an option

		int width = bitmapSize.cx / previewScale;
		int height = bitmapSize.cy / previewScale;

		#ifdef _DEBUG
				TCHAR buffer[253];
				swprintf(buffer, sizeof(buffer), _T("Unscaled: width: %d height: %d\nScaled: width: %d: height: %d\n"), bitmapSize.cx, bitmapSize.cy, width, height);
				OutputDebugString(buffer);
		#endif // DEBUG

		SetStretchBltMode(hdc, HALFTONE);

		// Copy the bits from the memory DC into the current dc
		StretchBlt(hdc, 0, 0, width, height, hdcMemory, 0, 0, bitmapSize.cx, bitmapSize.cy, SRCCOPY);

		// Restore the old bitmap
		SelectObject(hdcMemory, hOldBitmap);
		DeleteObject(hOldBitmap);

		DeleteDC(hdcMemory);
		DeleteDC(hdc);
		EndPaint(hWnd, &ps);
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}