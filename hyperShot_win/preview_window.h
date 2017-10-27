#pragma once

#include "hyperShot_win.h"

// Usefull MACROS
#define MAX_LOADSTRING				100

// Forward declare functions
ATOM PreviewRegisterClass(HINSTANCE);
BOOL InitIPreview(HINSTANCE, int, HBITMAP);
LRESULT CALLBACK WndPreviewProc(HWND, UINT, WPARAM, LPARAM);