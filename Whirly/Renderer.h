#pragma once

#include <wrl.h>
#include <atlbase.h>
#include <atlwin.h>
#include <d2d1_1.h>
#include <d3d11_1.h>

#pragma comment(lib, "d2d1")
#pragma comment(lib, "d3d11")

using namespace Microsoft::WRL;
using namespace D2D1;

// Error handling macros.
#define ASSERT ATLASSERT
#define VERIFY ATLVERIFY
#ifdef _DEBUG
#define HR(expression) ASSERT(S_OK == (expression))
#else
struct ComException
{
	HRESULT const hr;
	ComException(HRESULT const value) : hr(value) {}
};
inline void HR(HRESULT const hr)
{
	if (S_OK != hr) throw ComException(hr);
}
#endif

// Renders Direct2D and 3D content on the screen.
namespace Whirly
{
	namespace Core
	{
		class Renderer
		{
		private:
			// Direct3D objects.
			ComPtr<ID3D11Device> m_d3dDevice;
			ComPtr<ID3D11DeviceContext> m_d3dContext;
			ComPtr<IDXGISwapChain1> m_swapChain;

			// Direct3D rendering objects. Required for 3D.
			ComPtr<ID3D11RenderTargetView> m_d3dRenderTargetView;

			// Direct2D drawing components.
			ComPtr<ID2D1Device> m_d2dDevice;
			ComPtr<ID2D1DeviceContext> m_d2dContext;
			ComPtr<ID2D1Factory1> m_d2dFactory;
			ComPtr<ID2D1Bitmap1> m_d2dTargetBitmap;

			// Cached device properties.
			D3D_FEATURE_LEVEL m_d3dFeatureLevel;

			// Contains the debugging level of an ID2D1Factory object.
			D2D1_FACTORY_OPTIONS options;
			D2D1_BITMAP_PROPERTIES1 bitmapProperties;
			DXGI_PRESENT_PARAMETERS dxgiPresentParameters;




			ComPtr<ID2D1Bitmap1> __testBmp;

			// Creates device swap chain bitmap.
			void CreateDeviceSwapChainBitmap();

		public:

			// Screen DPI properties.
			float dpiX, dpiY;

			// Renderer constructor.
			Renderer();

			// Renderer destructor.
			~Renderer();

			// Initilizes device resources.
			void Initialize(HWND hwnd);
			
			// Render single frame of graphics.
			void Render();

			// Handles screen resize.
			void Resize(UINT width, UINT height);

			// Release resources.
			void Release();
		};
	}
}
