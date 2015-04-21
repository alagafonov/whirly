#include "AppMain.h"

// Renderer instance should be undefined on initialization.
Renderer *AppMain::renderer = NULL;

// Provides the application entry point.
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// Use HeapSetInformation to specify that the process should terminate if the heap manager detects an error in any heap used
	// by the process. The return value is ignored, because we want to continue running in the unlikely event that HeapSetInformation fails.
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	if (SUCCEEDED(CoInitialize(NULL)))
	{
		// Get the renderer instance.
		Renderer *renderer = AppMain::getRenderer();
		if (SUCCEEDED(AppMain::Initialize(renderer->dpiX, renderer->dpiY)))
		{
			// Run main application loop.
			MSG msg;
			while (true)
			{
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
					if (msg.message == WM_QUIT)
					{
						// Release resources.
						renderer->Release();
						delete renderer;
						renderer = NULL;
						break;
					}
				}
				else
				{
					// Render graphics frame.
					renderer->Render();
				}
			}
		}
		CoUninitialize();
	}

	return 0;
}

// Get instance of renderer class.
Renderer *AppMain::getRenderer()
{
	if (!renderer)
	{
		renderer = new Renderer();
	}

	return renderer;
}

// Creates the application window and device-independent resources.
HRESULT AppMain::Initialize(float dpiX, float dpiY)
{	
	HRESULT hr = S_FALSE;

	// Register the window class.
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = AppMain::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR);
	wcex.hInstance = HINST_THISCOMPONENT;
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
	wcex.lpszClassName = L"D2DDemoApp";
	RegisterClassEx(&wcex);

	// Create the window.
	HWND m_hwnd = CreateWindow(
		L"D2DDemoApp",
		L"Direct2D Demo App",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		static_cast<UINT>(ceil(WIDTH * dpiX / 96.f)),
		static_cast<UINT>(ceil(HEIGHT * dpiY / 96.f)),
		NULL,
		NULL,
		HINST_THISCOMPONENT,
		NULL
		);
		
	if (m_hwnd)
	{
		ShowWindow(m_hwnd, SW_SHOWNORMAL);
		UpdateWindow(m_hwnd);
		hr = S_OK;
	}

	return hr;
}

// Handles window messages.
LRESULT CALLBACK AppMain::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Get renderer instance pointer.
	Renderer *renderer = AppMain::getRenderer();

	// Process messages.
	LRESULT result = 0;
	switch (message)
	{
		case WM_CREATE:
			// Initialize DirectX.
			renderer->Initialize(hwnd);
			break;

		case WM_SIZE:
			renderer->Resize(LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_DISPLAYCHANGE:
			InvalidateRect(hwnd, NULL, FALSE);
			break;

		case WM_PAINT:
			ValidateRect(hwnd, NULL);
			renderer->Render();
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}