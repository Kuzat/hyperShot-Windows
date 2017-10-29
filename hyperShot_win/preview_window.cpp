//	preview_window.cpp: All functions and features concerning the preview window
//

#include "stdafx.h"
#include "preview_window.h"
#include <unordered_map>

typedef struct PreviewWindowInfo {
	HBITMAP hScreenshot;
	HDC hdcMemory;
	SIZE bitmapSize;
	int xMinScroll;
	int xCurrentScroll;
	int xMaxScroll;
	int yMinScroll;
	int yCurrentScroll;
	int yMaxScroll;
	BOOL fBlt;			// TRUE if BitBlt occured
	BOOL fScroll;		// TRUE if scrolling occured
	BOOL fSize;			// TRUE if fBlt & WM_SIZE
	BOOL fCreated;
} PreviewWindowInfo;

HWND hWndPreview;
WCHAR szPreviewTitle[MAX_LOADSTRING];                  // The preview title bar text
WCHAR szPreviewWindowClass[MAX_LOADSTRING];            // the preview window class name
std::unordered_map<HWND, PreviewWindowInfo> preview_windows;



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
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PREVIEWMENU);
	wcex.lpszClassName = szPreviewWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitIPreview(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates preview window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitIPreview(HINSTANCE hInstance, int nCmdShow, HBITMAP hBitmap) {



	hWndPreview = CreateWindowW(szPreviewWindowClass, szPreviewTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_THICKFRAME | WS_HSCROLL | WS_VSCROLL,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWndPreview)
	{
		return FALSE;
	}

	PreviewWindowInfo winInfo;
	
	GetBitmapDimensionEx(hBitmap, &winInfo.bitmapSize);
	winInfo.hScreenshot = hBitmap;

	preview_windows[hWndPreview] = winInfo;

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
//  WM_DESTROY  - delete the screenshot bitmap
//
//
LRESULT CALLBACK WndPreviewProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	HDC hdc;
	PAINTSTRUCT ps;
	SCROLLINFO si;

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
			// Closes the window
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_SHOWWINDOW:
	{	
		PreviewWindowInfo info = preview_windows.find(hWnd)->second;
		hdc = GetDC(hWnd);
		info.hdcMemory = CreateCompatibleDC(hdc);

		// select the new bitmap
		SelectObject(info.hdcMemory, info.hScreenshot);

		info.fBlt = FALSE;
		info.fScroll = FALSE;
		info.fSize = FALSE;
		info.fCreated = TRUE;

		info.xMinScroll = 0;
		info.xCurrentScroll = 0;
		info.xMaxScroll = 0;

		info.yMinScroll = 0;
		info.yCurrentScroll = 0;
		info.yMaxScroll = 0;

		preview_windows[hWnd] = info;
	}
	break;
	case WM_SIZE:
	{
		PreviewWindowInfo info = preview_windows.find(hWnd)->second;
		int xNewSize;
		int yNewSize;

		xNewSize = LOWORD(lParam);
		yNewSize = HIWORD(lParam);

		
		info.fSize = TRUE;

		// The horizontal scrolling range is defined by
		// (bitmap_width) - (client_width). The current horizontal
		// scroll value remains within the horizontal scrolling range.
		info.xMaxScroll = max(info.bitmapSize.cx - xNewSize, 0);
		info.xCurrentScroll = min(info.xCurrentScroll, info.xMaxScroll);
		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
		si.nMin = info.xMinScroll;
		si.nMax = info.bitmapSize.cx;
		si.nPage = xNewSize;
		si.nPos = info.xCurrentScroll;
		SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);

		// The vertical scrolling range is define by
		// (bitmap_height) - (client_height). The current vertical
		// scroll valueremains within the vertical scrolling range.
		info.yMaxScroll = max(info.bitmapSize.cy - yNewSize, 0);
		info.yCurrentScroll = min(info.yCurrentScroll, info.yMaxScroll);
		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
		si.nMin = info.yMinScroll;
		si.nMax = info.bitmapSize.cy;
		si.nPage = yNewSize;
		si.nPos = info.yCurrentScroll;
		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

		preview_windows[hWnd] = info;
	}
	break;
	case WM_PAINT:
	{
		PreviewWindowInfo info = preview_windows.find(hWnd)->second;
		PRECT prect;

		hdc = BeginPaint(hWnd, &ps);

		// If the window has been resized and the user has
		// capture the screen, use the follow call to
		// BitBlt to paint window's client area.
		if (info.fSize) {
			BitBlt(hdc, 0, 0, info.bitmapSize.cx, info.bitmapSize.cy, info.hdcMemory, info.xCurrentScroll, info.yCurrentScroll, SRCCOPY);

			info.fSize = FALSE;
		}

		// If scrolling hash occurred, use the following call to
		// BitBlt to paint the invalid rectangle.
		//
		// The coordinates of this rectangle are specified in the
		// RECT structure to which rect points.
		//
		// Note the it is  necessary to increment the seventh
		// argument (prect->left) by xCurrentScroll in
		// order to map the correct pixells from the source bitmap.
		if (info.fScroll) {
			prect = &ps.rcPaint;

			BitBlt(hdc, prect->left, prect->top, (prect->right - prect->left), (prect->bottom - prect->top), info.hdcMemory, prect->left + info.xCurrentScroll, prect->top + info.yCurrentScroll, SRCCOPY);

			info.fScroll = FALSE;
		}

		if (info.fCreated) {

			BitBlt(hdc, 0, 0, info.bitmapSize.cx, info.bitmapSize.cy, info.hdcMemory, info.xCurrentScroll, info.yCurrentScroll, SRCCOPY);

			info.fCreated = FALSE;
		}

		// OPTION: Make scaling an option
		/*
		double previewScale = 2.7; // OPTION: Make this an option

		int width = bitmapSize.cx / previewScale;
		int height = bitmapSize.cy / previewScale;

		#ifdef _DEBUG
		TCHAR buffer[253];
		swprintf(buffer, sizeof(buffer), _T("Unscaled: width: %d height: %d\nScaled: width: %d: height: %d\n"), bitmapSize.cx, bitmapSize.cy, width, height);
		OutputDebugString(buffer);
		#endif // DEBUG


		SetStretchBltMode(hdc, COLORONCOLOR);

		// Copy the bits from the memory DC into the current dc
		StretchBlt(hdc, 0, 0, width, height, hdcMemory, 0, 0, bitmapSize.cx, bitmapSize.cy, SRCCOPY);
		*/

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_MOUSEWHEEL:
	{
		PreviewWindowInfo info = preview_windows.find(hWnd)->second;
		int xDelta = 0;
		int yDelta = 0;
		int newPos;

		// Check if shift is down so we can scroll in horizontal direction
		if (GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT) {
			xDelta = GET_WHEEL_DELTA_WPARAM(wParam);

			newPos = info.xCurrentScroll - xDelta;

			// New position must be between 0 and the screen width. 
			newPos = max(0, newPos);
			newPos = min(info.xMaxScroll, newPos);

			xDelta = newPos - info.xCurrentScroll;

			info.fScroll = TRUE;

			info.xCurrentScroll = newPos;

			preview_windows[hWnd] = info;
			ScrollWindowEx(hWnd, -xDelta, -yDelta, NULL, NULL, NULL, NULL, SW_INVALIDATE);
			UpdateWindow(hWnd);

			// Reset the scroll bar. 
			si.cbSize = sizeof(si);
			si.fMask = SIF_POS;
			si.nPos = info.xCurrentScroll;
			SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);
			preview_windows[hWnd] = info;
		} else {
			//
			yDelta = GET_WHEEL_DELTA_WPARAM(wParam);

			newPos = info.yCurrentScroll - yDelta;

			// New position must be between 0 and the screen width. 
			newPos = max(0, newPos);
			newPos = min(info.yMaxScroll, newPos);

			yDelta = newPos - info.yCurrentScroll;

			info.fScroll = TRUE;

			info.yCurrentScroll = newPos;

			preview_windows[hWnd] = info;
			ScrollWindowEx(hWnd, -xDelta, -yDelta, NULL, NULL, NULL, NULL, SW_INVALIDATE);
			UpdateWindow(hWnd);

			// Reset the scroll bar. 
			si.cbSize = sizeof(si);
			si.fMask = SIF_POS;
			si.nPos = info.yCurrentScroll;
			SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
			preview_windows[hWnd] = info;
		}
	}
	break;
	case WM_HSCROLL:
	{
		PreviewWindowInfo info = preview_windows.find(hWnd)->second;
		int xDelta;     // xDelta = new_pos - current_pos  
		int xNewPos;    // new position 
		int yDelta = 0;

		switch (LOWORD(wParam))
		{
			// User clicked the scroll bar shaft left of the scroll box. 
		case SB_PAGEUP:
			xNewPos = info.xCurrentScroll - 50;
		break;
			// User clicked the scroll bar shaft right of the scroll box. 
		case SB_PAGEDOWN:
			xNewPos = info.xCurrentScroll + 50;
		break;
			// User clicked the left arrow. 
		case SB_LINEUP:
			xNewPos = info.xCurrentScroll - 5;
		break;
			// User clicked the right arrow. 
		case SB_LINEDOWN:
			xNewPos = info.xCurrentScroll + 5;
		break;
		case SB_THUMBTRACK:
			xNewPos = HIWORD(wParam);
		break;
		default:
			xNewPos = info.xCurrentScroll;
		}

		// New position must be between 0 and the screen width. 
		xNewPos = max(0, xNewPos);
		xNewPos = min(info.xMaxScroll, xNewPos);

		// If the current position does not change, do not scroll.
		if (xNewPos == info.xCurrentScroll) {
			preview_windows[hWnd] = info;
			break;
		}
			

		// Set the scroll flag to TRUE. 
		info.fScroll = TRUE;

		// Determine the amount scrolled (in pixels). 
		xDelta = xNewPos - info.xCurrentScroll;

		// Reset the current scroll position. 
		info.xCurrentScroll = xNewPos;

		// Scroll the window. (The system repaints most of the 
		// client area when ScrollWindowEx is called; however, it is 
		// necessary to call UpdateWindow in order to repaint the 
		// rectangle of pixels that were invalidated.) 
		preview_windows[hWnd] = info;
		ScrollWindowEx(hWnd, -xDelta, -yDelta, NULL, NULL, NULL, NULL, SW_INVALIDATE);
		UpdateWindow(hWnd);

		// Reset the scroll bar. 
		si.cbSize = sizeof(si);
		si.fMask = SIF_POS;
		si.nPos = info.xCurrentScroll;
		SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);
		preview_windows[hWnd] = info;
	}
	break;
	case WM_VSCROLL:
	{
		PreviewWindowInfo info = preview_windows.find(hWnd)->second;
		int xDelta = 0;
		int yDelta;     // yDelta = new_pos - current_pos 
		int yNewPos;    // new position 

		switch (LOWORD(wParam))
		{
			// User clicked the scroll bar shaft above the scroll box. 
		case SB_PAGEUP:
			yNewPos = info.yCurrentScroll - 50;
		break;
			// User clicked the scroll bar shaft below the scroll box. 
		case SB_PAGEDOWN:
			yNewPos = info.yCurrentScroll + 50;
		break;
			// User clicked the top arrow. 
		case SB_LINEUP:
			yNewPos = info.yCurrentScroll - 5;
		break;
			// User clicked the bottom arrow. 
		case SB_LINEDOWN:
			yNewPos = info.yCurrentScroll + 5;
		break;
		case SB_THUMBTRACK:
			yNewPos = HIWORD(wParam);
		break;
		default:
			yNewPos = info.yCurrentScroll;
		}

		// New position must be between 0 and the screen height. 
		yNewPos = max(0, yNewPos);
		yNewPos = min(info.yMaxScroll, yNewPos);

		// If the current position does not change, do not scroll.
		if (yNewPos == info.yCurrentScroll) {
			preview_windows[hWnd] = info;
			break;
		}

		// Set the scroll flag to TRUE. 
		info.fScroll = TRUE;

		// Determine the amount scrolled (in pixels). 
		yDelta = yNewPos - info.yCurrentScroll;

		// Reset the current scroll position. 
		info.yCurrentScroll = yNewPos;

		// Scroll the window. (The system repaints most of the 
		// client area when ScrollWindowEx is called; however, it is 
		// necessary to call UpdateWindow in order to repaint the 
		// rectangle of pixels that were invalidated.) 
		preview_windows[hWnd] = info;
		ScrollWindowEx(hWnd, -xDelta, -yDelta, NULL, NULL, NULL, NULL, SW_INVALIDATE);
		UpdateWindow(hWnd);

		// Reset the scroll bar. 
		si.cbSize = sizeof(si);
		si.fMask = SIF_POS;
		si.nPos = info.yCurrentScroll;
		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
		preview_windows[hWnd] = info;
	}
	break;
	case WM_DESTROY:
		//Cleanup
		preview_windows.erase(hWnd);
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}