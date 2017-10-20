#pragma once

#define WM_NOTIFYICON (WM_USER + 1)
#define NOTIFY_ID_MAIN 99

// Forward declare functions
BOOL InitNotifyIcon(HINSTANCE, HWND, UINT);
BOOL InitNotifyMenu(HINSTANCE);
void DeleteNotifyIcon(HWND, UINT);
void ShowPopup(HWND);