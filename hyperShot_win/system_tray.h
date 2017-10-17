#pragma once

#define WM_NOTIFYICON (WM_USER + 1)
#define NOTIFY_ID_MAIN 99

// Forward declare functions
BOOL InitNotifyIcon(HINSTANCE, HWND, UINT);
void DeleteNotifyIcon(HWND, UINT);