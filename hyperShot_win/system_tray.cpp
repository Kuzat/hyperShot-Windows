//	system_tray.cpp: Helper functions for notification icon
//

#include "stdafx.h"
#include "hyperShot_win.h"

// Global variables
PNOTIFYICONDATA pNotifyIconData;
HMENU hMenu;
HINSTANCE hInstance;

//
//    FUNCTION: InitNotifyIcon(HINSTANCE, HWND, UINT)
//
//    PURPOSE: Saves the notificationdata pointer and creates a notification icon
//
BOOL InitNotifyIcon(HINSTANCE hInstance, HWND hWndMain, UINT uId) {
	hInstance = hInstance;
	NOTIFYICONDATA nIcon;
	nIcon.cbSize = sizeof(NOTIFYICONDATA);
	nIcon.hWnd = hWndMain;
	nIcon.uID = uId;
	nIcon.uVersion = NOTIFYICON_VERSION_4;
	nIcon.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	nIcon.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HYPERSHOTWIN));
	nIcon.uCallbackMessage = WM_NOTIFYICON;
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
//		FUNCTION: InitNotifyMenu(HINSTANNCE)
//
//		PURPOSE: Loads the notify menu and assigns it to the right window so it is ready to be called used.
//
BOOL InitNotifyMenu(HINSTANCE hInstance) {
	hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDC_NOTIFYMENU));
	if (hMenu == NULL) {
		return FALSE;
	}
	hMenu = GetSubMenu(hMenu, 0);
	return TRUE;
}

void ShowPopup(HWND hWndMain) {
	POINT point;
	GetCursorPos(&point);
	UINT fuFlags = TPM_BOTTOMALIGN | TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_NOANIMATION | TPM_VERTICAL;
	if (GetSystemMetrics(SM_MENUDROPALIGNMENT)) {
		fuFlags |= TPM_RIGHTALIGN;
	} else {
		fuFlags |= TPM_LEFTALIGN;
	}
		
	SetForegroundWindow(hWndMain);
	int command = TrackPopupMenuEx(hMenu, fuFlags, point.x, point.y, hWndMain, NULL);

	// Handle the userinput from the popup menu
	switch (command) {
		case ID_OPENWINDOW:
			OpenWindow(hWndMain);
			break;
		case ID_TAKESELECTIVESCREENSHOT:
			// Take selective screenshot
			break;
		case ID_TAKESCREENSHOT:
			{
			// Take screenshot
			HBITMAP hScreenshot = GetScreenshot(0, 0, 0, 0, FULLSCREEN);
			InitIPreview(hInstance, SW_SHOW, hScreenshot);
			//PreviewWindow(hScreenshot, hInst);
			// Need to open preview window to show picture
			// Could put option for it to got straight to upload and skip preview/editing step
			}
			break;
		case ID_SETTINGS:
			// Show settings
			break;
		case IDM_ABOUT:
			PostMessage(hWndMain, WM_COMMAND, IDM_ABOUT, 0);
			break;
		case ID_QUIT:
			//Destory the window
			DestroyWindow(hWndMain);
			break;
		default:
			return;
	}
}

// 
//		FUNCTION: DeleteNotifyIcon(HWND, UINT)
//
//		PURPOSE: Delete the notification icon specified by hWndMain and uId;
//
void DeleteNotifyIcon(HWND hWndMain, UINT uId) {
	NOTIFYICONDATA nIcon;
	nIcon.cbSize = sizeof(NOTIFYICONDATA);
	nIcon.hWnd = hWndMain;
	nIcon.uID = uId;
	Shell_NotifyIcon(NIM_DELETE, &nIcon);
}