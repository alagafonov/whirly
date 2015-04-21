#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER              // Allow use of features specific to Windows 7 or later.
#define WINVER 0x0700       // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT        // Allow use of features specific to Windows 7 or later.
#define _WIN32_WINNT 0x0700 // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef UNICODE
#define UNICODE
#endif

// Exclude rarely-used items from Windows headers.
#define WIN32_LEAN_AND_MEAN

// Windows Header Files.
#include <windows.h>
#include <wincodec.h>

// C RunTime Header Files.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>

#include "Renderer.h"

using namespace Whirly::Core;

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

#define WIDTH 640
#define HEIGHT 480
#define FULLSCREEN 0
#define SCALE 0

// AppMain class declaration.
class AppMain
{
public:

	// Get instance of renderer class.
	static Renderer *getRenderer();

	// Initialize window.
	static HRESULT Initialize(float dpiX = 96.f, float dpiY = 96.f);

	// The windows procedure.
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:

	static Renderer *renderer;
};