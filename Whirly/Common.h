#pragma once

#include <string>
#include <wrl.h>
#include <atlbase.h>
#include <atlwin.h>
#include <d2d1_1.h>
#include <d3d11_1.h>
#include <dwrite.h>
#include <wincodec.h>

#pragma comment(lib, "d2d1")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "dwrite")

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

// Function to get current directory.
inline std::wstring GetCurrentDir()
{
	WCHAR buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
	return std::wstring(buffer).substr(0, pos);
}