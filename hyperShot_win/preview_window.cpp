//	preview_window.cpp: All functions and features concerning the preview window
//

#include "stdafx.h"
#include "preview_window.h"

HWND hWndPreview;
WCHAR szPreviewTitle[MAX_LOADSTRING];                  // The preview title bar text
WCHAR szPreviewWindowClass[MAX_LOADSTRING];            // the preview window class name
HBITMAP hScreenshot;
SIZE bitmapSize;


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
//   PURPOSE: Saves instance handle and creates preview window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitIPreview(HINSTANCE hInstance, int nCmdShow, HBITMAP hBitmap) {

	hScreenshot = hBitmap;

	GetBitmapDimensionEx(hScreenshot, &bitmapSize);

	hWndPreview = CreateWindowW(szPreviewWindowClass, szPreviewTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_THICKFRAME | WS_HSCROLL | WS_VSCROLL,
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
//  WM_DESTROY  - delete the screenshot bitmap
//
//
LRESULT CALLBACK WndPreviewProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	HDC hdc;
	PAINTSTRUCT ps;
	SCROLLINFO si;

	static HDC hdcWin;
	static HDC hdcMemory;
	static HBITMAP staticBitmap;
	static HBITMAP hOldBitmap;
	static BOOL fBlt;			// TRUE if BitBlt occured
	static BOOL fScroll;		// TRUE if scrolling occured
	static BOOL fSize;			// TRUE if fBlt & WM_SIZE
	static BOOL fCreated;

	// These variables are required for horizontal scrolling
	static int xMinScroll;
	static int xCurrentScroll;
	static int xMaxScroll;

	// These variables are required for vertical scrolling
	static int yMinScroll;
	static int yCurrentScroll;
	static int yMaxScroll;

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
	case WM_CREATE:
	{

		staticBitmap = hScreenshot;
		hdc = GetDC(hWnd);
		hdcMemory = CreateCompatibleDC(hdc);

		// select the new bitmap
		hOldBitmap = (HBITMAP)SelectObject(hdcMemory, staticBitmap);

		fBlt = FALSE;
		fScroll = FALSE;
		fSize = FALSE;
		fCreated = TRUE;

		xMinScroll = 0;
		xCurrentScroll = 0;
		xMaxScroll = 0;

		yMinScroll = 0;
		yCurrentScroll = 0;
		yMaxScroll = 0;
	}
	break;
	case WM_SIZE:
	{
		int xNewSize;
		int yNewSize;

		xNewSize = LOWORD(lParam);
		yNewSize = HIWORD(lParam);

		
		fSize = TRUE;

		// The horizontal scrolling range is defined by
		// (bitmap_width) - (client_width). The current horizontal
		// scroll value remains within the horizontal scrolling range.
		xMaxScroll = max(bitmapSize.cx - xNewSize, 0);
		xCurrentScroll = min(xCurrentScroll, xMaxScroll);
		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
		si.nMin = xMinScroll;
		si.nMax = bitmapSize.cx;
		si.nPage = xNewSize;
		si.nPos = xCurrentScroll;
		SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);

		// The vertical scrolling range is define by
		// (bitmap_height) - (client_height). The current vertical
		// scroll valueremains within the vertical scrolling range.
		yMaxScroll = max(bitmapSize.cy - yNewSize, 0);
		yCurrentScroll = min(yCurrentScroll, yMaxScroll);
		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
		si.nMin = yMinScroll;
		si.nMax = bitmapSize.cy;
		si.nPage = yNewSize;
		si.nPos = yCurrentScroll;
		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
	}
	break;
	case WM_PAINT:
	{
		PRECT prect;

		hdc = BeginPaint(hWnd, &ps);
		
		// If the window has been resized and the user has
		// capture the screen, use the follow call to
		// BitBlt to paint window's client area.
		if (fSize) {
			BitBlt(ps.hdc, 0, 0, bitmapSize.cx, bitmapSize.cy, hdcMemory, xCurrentScroll, yCurrentScroll, SRCCOPY);

			fSize = FALSE;
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
		if (fScroll) {
			prect = &ps.rcPaint;

			BitBlt(ps.hdc, prect->left, prect->top, (prect->right - prect->left), (prect->bottom - prect->top), hdcMemory, prect->left + xCurrentScroll, prect->top + yCurrentScroll, SRCCOPY);

			fScroll = FALSE;
		}

		if (fCreated) {
			OutputDebugString(_T("is created\n"));
			BitBlt(ps.hdc, 0, 0, bitmapSize.cx, bitmapSize.cy, hdcMemory, xCurrentScroll, yCurrentScroll, SRCCOPY);

			fCreated = FALSE;
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
	case WM_HSCROLL:
	{
		int xDelta;     // xDelta = new_pos - current_pos  
		int xNewPos;    // new position 
		int yDelta = 0;

		switch (LOWORD(wParam))
		{
			// User clicked the scroll bar shaft left of the scroll box. 
		case SB_PAGEUP:
			xNewPos = xCurrentScroll - 50;
		break;
			// User clicked the scroll bar shaft right of the scroll box. 
		case SB_PAGEDOWN:
			xNewPos = xCurrentScroll + 50;
		break;
			// User clicked the left arrow. 
		case SB_LINEUP:
			xNewPos = xCurrentScroll - 5;
		break;
			// User clicked the right arrow. 
		case SB_LINEDOWN:
			xNewPos = xCurrentScroll + 5;
		break;
		case SB_THUMBTRACK:
			xNewPos = HIWORD(wParam);
		break;
		default:
			xNewPos = xCurrentScroll;
		}

		// New position must be between 0 and the screen width. 
		xNewPos = max(0, xNewPos);
		xNewPos = min(xMaxScroll, xNewPos);

		// If the current position does not change, do not scroll.
		if (xNewPos == xCurrentScroll)
			break;

		// Set the scroll flag to TRUE. 
		fScroll = TRUE;

		// Determine the amount scrolled (in pixels). 
		xDelta = xNewPos - xCurrentScroll;

		// Reset the current scroll position. 
		xCurrentScroll = xNewPos;

		// Scroll the window. (The system repaints most of the 
		// client area when ScrollWindowEx is called; however, it is 
		// necessary to call UpdateWindow in order to repaint the 
		// rectangle of pixels that were invalidated.) 
		ScrollWindowEx(hWnd, -xDelta, -yDelta, NULL, NULL, NULL, NULL, SW_INVALIDATE);
		UpdateWindow(hWnd);

		// Reset the scroll bar. 
		si.cbSize = sizeof(si);
		si.fMask = SIF_POS;
		si.nPos = xCurrentScroll;
		SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);
	}
	break;
	case WM_VSCROLL:
	{
		int xDelta = 0;
		int yDelta;     // yDelta = new_pos - current_pos 
		int yNewPos;    // new position 

		switch (LOWORD(wParam))
		{
			// User clicked the scroll bar shaft above the scroll box. 
		case SB_PAGEUP:
			yNewPos = yCurrentScroll - 50;
		break;
			// User clicked the scroll bar shaft below the scroll box. 
		case SB_PAGEDOWN:
			yNewPos = yCurrentScroll + 50;
		break;
			// User clicked the top arrow. 
		case SB_LINEUP:
			yNewPos = yCurrentScroll - 5;
		break;
			// User clicked the bottom arrow. 
		case SB_LINEDOWN:
			yNewPos = yCurrentScroll + 5;
		break;
		case SB_THUMBTRACK:
			yNewPos = HIWORD(wParam);
		break;
		default:
			yNewPos = yCurrentScroll;
		}

		// New position must be between 0 and the screen height. 
		yNewPos = max(0, yNewPos);
		yNewPos = min(yMaxScroll, yNewPos);

		// If the current position does not change, do not scroll.
		if (yNewPos == yCurrentScroll)
			break;

		// Set the scroll flag to TRUE. 
		fScroll = TRUE;

		// Determine the amount scrolled (in pixels). 
		yDelta = yNewPos - yCurrentScroll;

		// Reset the current scroll position. 
		yCurrentScroll = yNewPos;

		// Scroll the window. (The system repaints most of the 
		// client area when ScrollWindowEx is called; however, it is 
		// necessary to call UpdateWindow in order to repaint the 
		// rectangle of pixels that were invalidated.) 
		ScrollWindowEx(hWnd, -xDelta, -yDelta, NULL, NULL, NULL, NULL, SW_INVALIDATE);
		UpdateWindow(hWnd);

		// Reset the scroll bar. 
		si.cbSize = sizeof(si);
		si.fMask = SIF_POS;
		si.nPos = yCurrentScroll;
		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
	}
	break;
	case WM_DESTROY:
		DeleteObject(hScreenshot);
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}