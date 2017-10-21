// hyperShot_win.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "hyperShot_win.h"

#define MAX_LOADSTRING				100
#define W_WIDTH						300
#define W_HEIGHT					500
#define HOTKEY_SCREENSHOT			1001
#define HOTKEY_SELECTIVESCREENSHOT	1002
#define HOTKEY_WINDOWSCREENSHOT		1003

// Global Variables:
HINSTANCE hInst;                                // current instance
HWND hWnd;										// currend handler
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow) {

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_HYPERSHOTWIN, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

	// Create a notification icon
	InitNotifyIcon(hInstance, hWnd, NOTIFY_ID_MAIN);

	// Load the shortcut menu
	InitNotifyMenu(hInstance);

	// Add global hotkeys
	RegisterHotKey(hWnd, HOTKEY_SCREENSHOT, MOD_CONTROL | MOD_SHIFT | MOD_NOREPEAT, 0x33);
	RegisterHotKey(hWnd, HOTKEY_SELECTIVESCREENSHOT, MOD_CONTROL | MOD_SHIFT | MOD_NOREPEAT, 0x34);
	RegisterHotKey(hWnd, HOTKEY_WINDOWSCREENSHOT, MOD_CONTROL | MOD_SHIFT | MOD_NOREPEAT, 0x35);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HYPERSHOTWIN));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance) {
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HYPERSHOTWIN));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_HYPERSHOTWIN);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
      CW_USEDEFAULT, 0, W_WIDTH, W_HEIGHT, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message)
    {
	case WM_HOTKEY:
		{
			int wmId = LOWORD(wParam);

			switch (wmId)
			{
			case HOTKEY_SCREENSHOT:
				{
					// Take screenshot
					OutputDebugString(_T("ctrl+shift+3 was pressed\n"));
				}
				break;
			case HOTKEY_SELECTIVESCREENSHOT:
				{
					// Take selective screenshot
					OutputDebugString(_T("ctrl+shift+4 was pressed\n"));
				}
				break;
			case HOTKEY_WINDOWSCREENSHOT:
				{
					// Take window sccreenshot
					OutputDebugString(_T("ctrl+shift+5 was pressed\n"));
				}
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
				// Hides the window
				ShowWindow(hWnd, SW_HIDE);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_NOTIFYICON:
		switch (lParam) {
		case WM_LBUTTONDBLCLK:
			{
				OpenWindow(hWnd);
			}
			break;
		case WM_RBUTTONUP:
			// Open popup menu
			ShowPopup(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:		
		// Delete Icon
		DeleteNotifyIcon(hWnd, NOTIFY_ID_MAIN);
        PostQuitMessage(0);
        break;
	case WM_CLOSE:
		// Prevent window from closeing and hide it instead
		ShowWindow(hWnd, SW_HIDE);
		break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
