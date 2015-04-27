#pragma once

#include "Common.h"

using namespace Microsoft::WRL;

namespace Whirly
{
	namespace Helpers
	{
		// This class provides various utility functions to work with Direct2D and related APIs such as WIC and DirectWrite
		class Direct2DHelper
		{
		public:

			// Load bitmap from file.
			static HRESULT LoadBitmapFromFile(ID2D1DeviceContext *renderTarget, const wchar_t *uri, unsigned int destinationWidth, unsigned int destinationHeight, ID2D1Bitmap1 **bitmap);
			
			//Get WIC factory.
			static HRESULT GetWICFactory(__out IWICImagingFactory** factory);

			//Release WIC factory.
			static void ReleaseWICFactory();

		private:

			// Direct2DHelper constructor.
			Direct2DHelper();
			
			// Direct2DHelper destructor.
			~Direct2DHelper();

			// WIC factory instance.
			static ComPtr<IWICImagingFactory> m_pWICFactory;
		};
	}
}