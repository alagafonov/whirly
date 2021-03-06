﻿#include "Renderer.h"

using namespace Whirly::Core;

// Renderer constructor.
Renderer::Renderer()
{
	// Create ID2D1Factory1 factory to create Direct2D resources.
	HR(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &options, &m_d2dFactory));

	// Get current screen DPI.
	m_d2dFactory->GetDesktopDpi(&dpiX, &dpiY);
}

// Renderer destructor.
Renderer::~Renderer()
{
}

// Loads and initializes DirectX componenets and application assets when the application is loaded.
void Renderer::Initialize(HWND hwnd)
{
	// This flag adds support for surfaces with a different color channel ordering
	// than the API default. It is required for compatibility with Direct2D.
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	// This array defines the set of DirectX hardware feature levels this app  supports.
	// The ordering is important and you should  preserve it.
	// Don't forget to declare your app's minimum required feature level in its
	// description.  All apps are assumed to support 9.1 unless otherwise stated.
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	// Create the Direct3D 11 API device object and a corresponding context.
	HRESULT hr = D3D11CreateDevice(
		nullptr,					// Specify nullptr to use the default adapter.
		D3D_DRIVER_TYPE_HARDWARE,	// Create a device using the hardware graphics driver.
		0,							// Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
		creationFlags,				// Set debug and Direct2D compatibility flags.
		featureLevels,				// List of feature levels this app can support.
		ARRAYSIZE(featureLevels),	// Size of the list above.
		D3D11_SDK_VERSION,			// Always set this to D3D11_SDK_VERSION for Windows Store apps.
		&m_d3dDevice,					// Returns the Direct3D device created.
		&m_d3dFeatureLevel,			// Returns feature level of device created.
		&m_d3dContext					// Returns the device immediate context.
		);

	// Obtain the underlying DXGI device of the Direct3D11 device.
	ComPtr<IDXGIDevice1> dxgiDevice;
	HR(m_d3dDevice.As(&dxgiDevice));

	// Obtain the Direct2D device for 2-D rendering.
	HR(m_d2dFactory->CreateDevice(dxgiDevice.Get(), &m_d2dDevice));

	// Get Direct2D device's corresponding device context object.
	HR(m_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_d2dContext));

	// Allocate a descriptor.
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;                                  // use double buffering to enable flip
	swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;              // this is the most common swapchain format
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;    // all apps must use this SwapEffect
	swapChainDesc.SampleDesc.Count = 1;                             // don't use multi-sampling
	swapChainDesc.SampleDesc.Quality = 0;

	// Identify the physical adapter (GPU or card) this device is runs on.
	ComPtr<IDXGIAdapter> dxgiAdapter;
	HR(dxgiDevice->GetAdapter(&dxgiAdapter));

	// Get the factory object that created the DXGI device.
	ComPtr<IDXGIFactory2> dxgiFactory;
	HR(dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)));

	// Get the final swap chain for this window from the DXGI factory.
	HR(dxgiFactory->CreateSwapChainForHwnd(m_d3dDevice.Get(), hwnd, &swapChainDesc, nullptr, nullptr, &m_swapChain));

	// Ensure that DXGI doesn't queue more than one frame at a time.
	HR(dxgiDevice->SetMaximumFrameLatency(1));

	// Now we set up the Direct2D render target bitmap linked to the swapchain. 
	// Whenever we render to this bitmap, it is directly rendered to the 
	// swap chain associated with the window.
	bitmapProperties = BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
		dpiX,
		dpiY
		);

	// Creates device swapchain bitmap.
	CreateDeviceSwapChainBitmap();

	// Create DirectWrite factory.
	HR(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &m_dWriteFactory));

	// Create DirectWrite font format.
	HR(m_dWriteFactory->CreateTextFormat(L"Arial", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_MEDIUM, 15.0, L"en-US", &m_dWriteTextFormat));

	//TCHAR path[MAX_PATH + 1] = L"";
	//DWORD len = GetCurrentDirectory(MAX_PATH, path);

	//Direct2DHelper::LoadBitmapFromFile(m_d2dContext.Get(), L"Assets\\Images\\14960114.jpg", 300, 300, &__testBmp);
	Direct2DHelper::LoadBitmapFromFile(m_d2dContext.Get(), L"Assets\\Images\\circle.png", 16, 16, &__testBmp);

	m_d2dContext->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Brown),
		&pBlackBrush
		);
}

// Creates device swapchain bitmap.
void Renderer::CreateDeviceSwapChainBitmap()
{
	// Direct2D needs the dxgi version of the backbuffer surface pointer.
	ComPtr<IDXGISurface> dxgiBackBuffer;
	HR(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer)));

	// Get a D2D surface from the DXGI back buffer to use as the D2D render target.
	HR(m_d2dContext->CreateBitmapFromDxgiSurface(dxgiBackBuffer.Get(), &bitmapProperties, &m_d2dTargetBitmap));

	// Now we can set the Direct2D render target.
	m_d2dContext->SetTarget(m_d2dTargetBitmap.Get());
}

// Updates the application state once per frame.
void Renderer::Update()
{
	// Update scene objects.
	stepTimer.Tick([&]()
	{
		// Update logic goes here.
	});
}

// This function renders a single frame of graphics.
void Renderer::Render()
{
	// Start drawing.
	m_d2dContext->BeginDraw();

	// Clear screen.
	m_d2dContext->Clear(ColorF(0.0f, 0.0f, 0.0f));

	
	

	

	
	//m_d2dContext->DrawTextW(GetCurrentDir().c_str(), GetCurrentDir().length(), m_dWriteTextFormat.Get(), D2D1::RectF(10, 100, 600, 100), pBlackBrush);
	//m_d2dContext->DrawBitmap(__testBmp.Get(), D2D1::RectF(10, 10, 26, 26));
	//m_d2dContext->DrawTextW(L"ELENA", 5, m_dWriteTextFormat.Get(), D2D1::RectF(10, 100, 600, 100), pBlackBrush);


	ComPtr<ID2D1Effect> colorMatrixEffect;
	HR(m_d2dContext->CreateEffect(CLSID_D2D1ColorMatrix, &colorMatrixEffect));

	colorMatrixEffect->SetInput(0, __testBmp.Get());

	D2D1_MATRIX_5X4_F matrix = D2D1::Matrix5x4F(0.5, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	HR(colorMatrixEffect->SetValue(D2D1_COLORMATRIX_PROP_COLOR_MATRIX, matrix));

	/*255 0 0 0 1 x 1 0 0 1
				  0 0 0 0
				  0 0 0 0
				  0 0 0 0
				  0 0 0 0*/

	m_d2dContext->DrawImage(colorMatrixEffect.Get());




	


	/*m_d2dContext->SetTransform(D2D1::Matrix3x2F::Identity());
	m_d2dContext->Clear(D2D1::ColorF(D2D1::ColorF::White));



	
	m_d2dContext->DrawTextW(std::to_wstring(stepTimer.GetFramesPerSecond()).c_str(), 4, m_dWriteTextFormat.Get(), D2D1::RectF(10, 10, 50, 20), pBlackBrush);

	D2D1_SIZE_F rtSize = m_d2dContext->GetSize();

	// Draw a grid background.
	int width = static_cast<int>(rtSize.width);
	int height = static_cast<int>(rtSize.height);

	for (int x = 0; x < width; x += 10)
	{
		m_d2dContext->DrawLine(
			D2D1::Point2F(static_cast<FLOAT>(x), 0.0f),
			D2D1::Point2F(static_cast<FLOAT>(x), rtSize.height),
			pBlackBrush,
			0.5f
			);
	}

	for (int y = 0; y < height; y += 10)
	{
		m_d2dContext->DrawLine(
			D2D1::Point2F(0.0f, static_cast<FLOAT>(y)),
			D2D1::Point2F(rtSize.width, static_cast<FLOAT>(y)),
			pBlackBrush,
			0.5f
			);
	}

	// Draw two rectangles.
	D2D1_RECT_F rectangle1 = D2D1::RectF(
		rtSize.width / 2 - rand() % 50,
		rtSize.height / 2 - rand() % 50,
		rtSize.width / 2 + 50.0f,
		rtSize.height / 2 + 50.0f
		);

	D2D1_RECT_F rectangle2 = D2D1::RectF(
		rtSize.width / 2 - 100.0f,
		rtSize.height / 2 - 100.0f,
		rtSize.width / 2 + 100.0f,
		rtSize.height / 2 + 100.0f
		);


	// Draw a filled rectangle.
	m_d2dContext->FillRectangle(&rectangle1, pBlackBrush);

	// Draw the outline of a rectangle.
	m_d2dContext->DrawRectangle(&rectangle2, pBlackBrush);*/

	// End drawing.
	m_d2dContext->EndDraw();

	// Swap buffers.
	m_swapChain->Present1(1, 0, &dxgiPresentParameters);
}

// This function handles screen resizing.
void Renderer::Resize(UINT width, UINT height)
{
	// If the swap chain already exists, resize it.
	if (m_swapChain != nullptr)
	{
		// Release all resources first.
		m_d2dContext->SetTarget(nullptr);
		m_d2dTargetBitmap.Reset();

		// Resize buffers.
		if (SUCCEEDED(m_swapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_B8G8R8A8_UNORM, 0)))
		{
			CreateDeviceSwapChainBitmap();
		}
		else
		{
			Release();
		}
	}
}

// Release resources.
void Renderer::Release()
{
	Direct2DHelper::ReleaseWICFactory();
	m_dWriteFactory.Reset();
	m_dWriteTextFormat.Reset();

	m_d2dTargetBitmap.Reset();
	m_d2dFactory.Reset();
	m_d2dContext.Reset();
	m_d2dDevice.Reset();

	m_d3dRenderTargetView.Reset();
	m_swapChain.Reset();
	m_d3dContext.Reset();
	m_d3dDevice.Reset();
}