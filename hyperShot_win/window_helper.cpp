//	window_helper.cpp: Helper functions for windows
//

#include "stdafx.h"
#include "window_helper.h"

void OpenWindow(HWND hWnd) {
	// Check if window is minimized or TODO: closed.
	WINDOWPLACEMENT winPlace;
	winPlace.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(hWnd, &winPlace);


	#ifdef _DEBUG
		TCHAR buffer[253];
		swprintf(buffer, sizeof(buffer), _T("%d\n"), winPlace.showCmd);
		OutputDebugString(buffer);
	#endif // DEBUG


	if (winPlace.showCmd == SW_SHOWMINIMIZED) {
		ShowWindow(hWnd, SW_RESTORE);
	}
	else if (!IsWindowVisible(hWnd)) {
		ShowWindow(hWnd, SW_SHOW);
	}

	// Set the window as foreground window and give it focus
	SetForegroundWindow(hWnd);
}