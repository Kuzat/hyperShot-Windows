//	system_tray.cpp: Helper functions for notification icon
//

#include "stdafx.h"
#include "hyperShot_win.h"

// Global variables
PNOTIFYICONDATA pNotifyIconData;

//
//    FUNCTION: InitNotifyIcon(HINSTANCE, HWND, UINT)
//
//    PURPOSE: Saves the notificationdata pointer and creates a notification icon
//
BOOL InitNotifyIcon(HINSTANCE hInstance, HWND hWnd, UINT uId) {
	NOTIFYICONDATA nIcon;
	nIcon.cbSize = sizeof(NOTIFYICONDATA);
	nIcon.hWnd = hWnd;
	nIcon.uID = uId;
	nIcon.uVersion = NOTIFYICON_VERSION_4;
	nIcon.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	nIcon.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HYPERSHOTWIN));
	nIcon.uCallbackMessage = WM_MYMESSAGE;
	wcscpy_s(nIcon.szTip, _T("hyperShot"));


	// Save it to a global pointer variable
	pNotifyIconData = &nIcon;

	if (!Shell_NotifyIcon(NIM_ADD, pNotifyIconData)) {
		printf("Failed to add notification icon!\n");
		return FALSE;
	}

	return TRUE;
}

// 
//		FUNCTION: DeleteNotifyIcon(HWND, UINT)
//
//		PURPOSE: Delete the notification icon specified by hWnd and uId;
//
void DeleteNotifyIcon(HWND hWnd, UINT uId) {
	NOTIFYICONDATA nIcon;
	nIcon.cbSize = sizeof(NOTIFYICONDATA);
	nIcon.hWnd = hWnd;
	nIcon.uID = uId;
	Shell_NotifyIcon(NIM_DELETE, &nIcon);
}