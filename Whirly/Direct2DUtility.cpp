#include "Direct2DUtility.h"

using namespace Whirly::Helpers;

ComPtr<IWICImagingFactory> Direct2DHelper::m_pWICFactory = nullptr;

Direct2DHelper::Direct2DHelper()
{
}

Direct2DHelper::~Direct2DHelper()
{
}

// Creates a Direct2D bitmap from the specified file name.
HRESULT Direct2DHelper::LoadBitmapFromFile(ID2D1DeviceContext *renderTarget, const wchar_t *uri, unsigned int destinationWidth, unsigned int destinationHeight, ID2D1Bitmap1 **bitmap)
{
	// Resources required for loading and decoding images.
	ComPtr<IWICBitmapDecoder> decoder;
	ComPtr<IWICBitmapFrameDecode> bitmapSource;
	ComPtr<IWICStream> stream;
	ComPtr<IWICFormatConverter> converter;
	ComPtr<IWICBitmapScaler> scaler;
	ComPtr<IWICImagingFactory> wicFactory;

	// Get WIC factory.
	HR(GetWICFactory(&wicFactory));

	// Create decoder.
	HR(wicFactory->CreateDecoderFromFilename(uri, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder));

	// Create the initial frame.
	HR(decoder->GetFrame(0, &bitmapSource));

	// Convert the image format to 32bppPBGRA (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
	HR(wicFactory->CreateFormatConverter(&converter));
	
	// If a new width or height was specified, create an IWICBitmapScaler and use it to resize the image.
	if (destinationWidth != 0 || destinationHeight != 0)
	{
		unsigned int originalWidth, originalHeight;
		HR(bitmapSource->GetSize(&originalWidth, &originalHeight));
		
		// Auto calculate dimensions if one is not specified.
		if (destinationWidth == 0)
		{
			float scalar = static_cast<float>(destinationHeight) / static_cast<float>(originalHeight);
			destinationWidth = static_cast<unsigned int>(scalar * static_cast<float>(originalWidth));
		}
		else if (destinationHeight == 0)
		{
			float scalar = static_cast<float>(destinationWidth) / static_cast<float>(originalWidth);
			destinationHeight = static_cast<unsigned int>(scalar * static_cast<float>(originalHeight));
		}

		// Create bitmap scaler.
		HR(wicFactory->CreateBitmapScaler(&scaler));
		
		// Initialize scaler.
		HR(scaler->Initialize(bitmapSource.Get(), destinationWidth, destinationHeight, WICBitmapInterpolationModeCubic));

		// Initialize converter.
		HR(converter->Initialize(scaler.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.f, WICBitmapPaletteTypeMedianCut));
	}

	// Don't scale image.
	else
	{
		// Initialize converter.
		HR(converter->Initialize(bitmapSource.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.f, WICBitmapPaletteTypeMedianCut));
	}

	// Create a Direct2D bitmap from the WIC bitmap.
	HR(renderTarget->CreateBitmapFromWicBitmap(converter.Get(), nullptr, bitmap));

	return S_OK;
}

// Get a WIC Imaging factory.
HRESULT Direct2DHelper::GetWICFactory(IWICImagingFactory** factory)
{	
	// If WIC factory has not yet been initialized then create it.
	if (m_pWICFactory == nullptr)
	{
		HR(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWICFactory)));
	}
	
	*factory = m_pWICFactory.Get();

	return S_OK;
}

// Release WIC Imaging factory.
void Direct2DHelper::ReleaseWICFactory()
{
	if (m_pWICFactory != nullptr)
	{
		m_pWICFactory.Reset();
	}
}