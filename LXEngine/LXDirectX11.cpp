//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXDirectX11.h"
#include "LXConsoleManager.h"

// Direct2D
#if LX_D2D1
#include <atlcomcli.h>
#include <dxgi.h>
#include <D2d1_1.h>
#include <d2d1_1helper.h>
#include <Dwrite.h>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "Dwrite.lib")
using namespace D2D1;
#endif

// DirectX Graphics Infrastructure (DXGI)
#pragma comment(lib, "dxgi.lib")

// Direct3D 11
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

//
// Binded on variable and set from an INI file.
//

namespace
{
	LXConsoleCommandT<bool> CSet_CreateDeviceDebug(L"Engine.ini", L"D3D11", L"CreateDeviceDebug", L"false");
	LXConsoleCommandT<bool> CSet_VSync(L"Engine.ini", L"D3D11", L"VSync", L"false");

	// Use Direct2D to display statistics
	// not supported by Nsight
	bool gD2D1 = true;
};

//#define BackBufferFormat DXGI_FORMAT_B8G8R8A8_UNORM // Preferable ?
#define BackBufferFormat DXGI_FORMAT_R8G8B8A8_UNORM

ID3D11Device* g_d3d11Device = nullptr;
ID3D11DeviceContext* g_d3d11DeviceContext = nullptr;
#if LX_USE_D3D11_1
ID3D11DeviceContext1* g_d3d11DeviceContext1 = nullptr;
#endif
LXDirectX11* g_DirextX11 = nullptr;

bool NoOverwriteConstantBuffer = false;

LXDirectX11::LXDirectX11(HWND hWND)
{
	g_DirextX11 = this;

	bool bInit = InitD3D11AndD2D1(hWND);
//	bool bInit = Init(hWND);

	CHK(bInit);

	g_d3d11Device = _D3D11Device;
	g_d3d11DeviceContext = _D3D11DeviceContext;
	#if LX_USE_D3D11_1
	g_d3d11DeviceContext1 = _D3D11DeviceContext1;
	#endif
}

LXDirectX11::~LXDirectX11()
{
	CHK(IsRenderThread());

	LX_SAFE_RELEASE(_D3DUserDefinedAnnotation)
	LX_SAFE_RELEASE(_D3D11RenderTargetView)

#if LX_D2D1

	LX_SAFE_RELEASE(_D2D1TargetBitmap)

#endif
	
	LX_SAFE_RELEASE(_DXGISwapChain1)
	LX_SAFE_RELEASE(_DXGISwapChain)

#if LX_D2D1
	LX_SAFE_RELEASE(_D2D1DeviceContext)
	LX_SAFE_RELEASE(_D2D1Device)
#endif

	if (_D3D11DeviceContext)
	{
		_D3D11DeviceContext->ClearState();
		_D3D11DeviceContext->Flush();
		_D3D11DeviceContext->Release();
	}

#if LX_USE_D3D11_1
	LX_SAFE_RELEASE(_D3D11DeviceContext1)
#endif

	if (_D3D11Debug)
	{
		_D3D11Debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		_D3D11Debug->Release();
	}
	
	if (_D3D11Device)
	{
		ULONG count = _D3D11Device->Release();
		if (count)
		{
			LogW(LXDirectX11, L"Found %i living objects releasing the D3D11Device", count);
			LXObject::TraceAll();
			CHK(count == 0);
		} 
	}
		
	g_d3d11Device = nullptr;
	g_d3d11DeviceContext = nullptr;
#if LX_USE_D3D11_1
	g_d3d11DeviceContext1 = nullptr;
#endif
	g_DirextX11 = nullptr;
}

ID3D11Device* LXDirectX11::GetCurrentDevice()
{
	CHK(g_d3d11Device);
	return g_d3d11Device;
}

ID3D11DeviceContext* LXDirectX11::GetCurrentDeviceContext()
{
	CHK(g_d3d11DeviceContext);
	return g_d3d11DeviceContext;
}

#if LX_USE_D3D11_1
ID3D11DeviceContext1* LXDirectX11::GetCurrentDeviceContext1()
{
	return g_d3d11DeviceContext1;
}
#endif

LXDirectX11* LXDirectX11::GetCurrentDirectX11()
{
	CHK(g_DirextX11);
	return g_DirextX11;
}

void LXDirectX11::LogError(HRESULT hr)
{
	switch (hr)
	{
	case D3D11_ERROR_FILE_NOT_FOUND: LogE(LXDirectx11, L"The file was not found."); break;
	case D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS: LogE(LXDirectx11, L"There are too many unique instances of a particular type of state object."); break;
	case D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS: LogE(LXDirectx11, L"There are too many unique instances of a particular type of view object."); break;
	case D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD: LogE(LXDirectx11, L"The first call to ID3D11DeviceContext::Map after either ID3D11Device::CreateDeferredContext or ID3D11DeviceContext::FinishCommandList per Resource was not D3D11_MAP_WRITE_DISCARD."); break;
	case DXGI_ERROR_INVALID_CALL: LogE(LXDirectx11, L"The method call is invalid.For example, a method's parameter may not be a valid pointer."); break;
	case DXGI_ERROR_WAS_STILL_DRAWING: LogE(LXDirectx11, L"The previous blit operation that is transferring information to or from this surface is incomplete."); break;
	case E_FAIL: LogE(LXDirectx11, L"Attempted to create a device with the debug layer enabled and the layer is not installed."); break;
	case E_INVALIDARG: LogE(LXDirectx11, L"An invalid parameter was passed to the returning function."); break;
	case E_OUTOFMEMORY: LogE(LXDirectx11, L"Direct3D could not allocate sufficient memory to complete the call."); break;
	case E_NOTIMPL: LogE(LXDirectx11, L"The method call isn't implemented with the passed parameter combination."); break;
	case S_FALSE: LogE(LXDirectx11, L"Alternate success value, indicating a successful but nonstandard completion(the precise meaning depends on context)."); break;
	case E_NOINTERFACE: LogE(LXDirectx11, L"No such interface supported"); break;
	case S_OK: LogI(LXDirectx11, L"No error occurred."); break;
	default: LogE(LXDirectx11, L"Unknow error: %i", hr); break;
	}
}

bool LXDirectX11::CreateBuffers()
{

#if LX_D2D1
	CHK(_D2D1TargetBitmap == nullptr);
#endif
	
	CHK(_D3D11RenderTargetView == nullptr);

	HRESULT hr;
	
#if LX_D2D1

	if (gD2D1)
	{
		//
		// Create the D2DTargetBitmap
		//

		D2D1_BITMAP_PROPERTIES1 bitmapProperties = BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			PixelFormat(BackBufferFormat, D2D1_ALPHA_MODE_IGNORE),
			96, 96);
	
		IDXGISurface* dxgiBackBuffer = nullptr;
		if (_DXGISwapChain1)
		{
			hr = _DXGISwapChain1->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));
			if (hr != S_OK)
			{
				LogE(LXDirectX11, L"GetBuffer IDXGISurface failed");
				LogError(hr);
				return false;
			}

			hr = _D2D1DeviceContext->CreateBitmapFromDxgiSurface(dxgiBackBuffer, &bitmapProperties, &_D2D1TargetBitmap);
			if (hr != S_OK)
			{
				LogE(LXDirectX11, L"CreateBitmapFromDxgiSurface failed");
				LogError(hr);
				return false;
			}
			dxgiBackBuffer->Release();

			_D2D1DeviceContext->SetTarget(_D2D1TargetBitmap);
		}
	}
#endif
	
	//
	// Create the BackBuffer
	//
	
	ID3D11Texture2D* BackBuffer = nullptr;
	hr = _DXGISwapChain1->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);
	if (hr != S_OK)
	{
		LogE(LXDirectX11, L"GetBuffer failed");
		LogError(hr);
		return false;
	}

	//
	// Create the Render Target
	//

	hr = _D3D11Device->CreateRenderTargetView(BackBuffer, NULL, &_D3D11RenderTargetView);
	if (hr != S_OK)
	{
		LogE(LXDirectX11, L"CreateRenderTargetView failed");
		LogError(hr);
		return false;
	}
	BackBuffer->Release();

	return true;
}

void LXDirectX11::ReleaseBuffers()
{
	_D3D11DeviceContext->OMSetRenderTargets(0, 0, 0);


#if LX_D2D1
	if (_D2D1DeviceContext)
		_D2D1DeviceContext->SetTarget(0);
#endif

	LX_SAFE_RELEASE(_D3D11RenderTargetView);

#if LX_D2D1
	
	LX_SAFE_RELEASE(_D2D1TargetBitmap);

#endif
}

bool LXDirectX11::InitD3D11AndD2D1(HWND hWND)
{
	bool gCreateDeviceDebug = CSet_CreateDeviceDebug.GetValue();

	if (!hWND)
	{
		return false;
	}
	
	bool bD2D1 = false;
	_hWND = hWND;

	HRESULT hr;

	//
	// Find an adapter.
	//

#if 0

	IDXGIFactory1*  DXGIFactory;
	hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void **)&DXGIFactory);
	if (FAILED(hr))
	{
		LogE(LXDirectX11, L"Can't create DXGIFactory");
		LogError(hr);
		return false;
	}

	IDXGIAdapter1* DXGIAdapter = nullptr;
	if (DXGIFactory->EnumAdapters1(0, &DXGIAdapter) == DXGI_ERROR_NOT_FOUND)
	{
		LogE(LXDirectX11, L"No adapters found");
		return false;
	}

	IDXGIOutput* DXGIOutput;
	if (DXGIAdapter->EnumOutputs(0, &DXGIOutput) == DXGI_ERROR_NOT_FOUND)
	{
		LogE(LXDirectX11, L"No outputs found");
		return false;
	}
	
#endif
		
	//
	// Create the Devices...
	//

	//
	// D3DDevice & D3DDeviceContext
	//

	UINT CreationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;// | D3D11_CREATE_DEVICE_SINGLETHREADED;

	if (gCreateDeviceDebug == true)
		CreationFlags |= D3D11_CREATE_DEVICE_DEBUG;

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
		
	D3D_FEATURE_LEVEL CreatedFeatureLevel;

	hr = D3D11CreateDevice(
		nullptr,                    // specify null to use the default adapter
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		CreationFlags,              // optionally set debug and Direct2D compatibility flags
		featureLevels,              // list of feature levels this app can support
		ARRAYSIZE(featureLevels),   // number of possible feature levels
		D3D11_SDK_VERSION,
		&_D3D11Device,              // returns the Direct3D device created
		&CreatedFeatureLevel,       // returns feature level of device created
		&_D3D11DeviceContext        // returns the device immediate context
	);

	if(hr != S_OK)
	{
		LogE(DirectX11, L"D3D11CreateDevice Failed");
		LogError(hr);
		return false;
	}


#if LX_USE_D3D11_1

	hr = _D3D11DeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1), (void**)&_D3D11DeviceContext1);
	if (hr != S_OK)
	{
		LogE(LXDirectX11, L"QueryInterface ID3D11DeviceContext1 failed");
		LogError(hr);
		return false;
	}
	
	D3D11_FEATURE_DATA_D3D11_OPTIONS FeatureDataD3D11Options = { 0 };
	hr = _D3D11Device->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS, &FeatureDataD3D11Options, sizeof(D3D11_FEATURE_DATA_D3D11_OPTIONS));
	if (hr != S_OK)
	{
		LogE(LXDirectX11, L"QueryInterface ID3D11Debug failed");
		LogError(hr);
		return false;
	}
		
	NoOverwriteConstantBuffer &= FeatureDataD3D11Options.MapNoOverwriteOnDynamicConstantBuffer == TRUE; //MapNoOverwriteOnDynamicBufferSRV.
	
#endif
	
	IDXGIDevice* dxgiDevice = nullptr;
	hr = _D3D11Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
	if (hr != S_OK)
	{
		LogE(LXDirectX11, L"QueryInterface IDXGIDevice failed");
		LogError(hr);
		return false;
	}

	//
	// D3D11Debug
	//

	if (gCreateDeviceDebug == true)
	{
		hr = _D3D11Device->QueryInterface(IID_PPV_ARGS(&_D3D11Debug));
		if (hr != S_OK)
		{
			LogE(LXDirectX11, L"QueryInterface ID3D11Debug failed");
			LogError(hr);
			return false;
		}
	}

	//
	// D3DUserDefinedAnnotation 
	//

	hr = _D3D11DeviceContext->QueryInterface(__uuidof(ID3DUserDefinedAnnotation), (void**)&_D3DUserDefinedAnnotation);
	if (hr != S_OK)
	{
		LogE(LXDirectX11, L"QueryInterface ID3DUserDefinedAnnotation failed");
		LogError(hr);
		return false;
	}

	//
	// D2D1Device D2D1DeviceContext
	//

#if LX_D2D1
	if (gD2D1)
	{
		D2D1_FACTORY_OPTIONS options;
		ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));
#ifdef _DEBUG
		options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
		ID2D1Factory1* D2D1Factory = nullptr;
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &options, (void**)&D2D1Factory);
		if (hr != S_OK)
		{
			LogE(LXDirectX11, L"D2D1CreateFactory failed");
			LogError(hr);
			return false;
		}

		hr = D2D1Factory->CreateDevice(dxgiDevice, &_D2D1Device);
		if (hr != S_OK)
		{
			LogE(LXDirectX11, L"CreateDevice ID2D1Device failed");
			LogError(hr);
			return false;
		}
		D2D1Factory->Release();

		hr = _D2D1Device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &_D2D1DeviceContext);
		if (hr != S_OK)
		{
			LogE(LXDirectX11, L"CreateDeviceContext ID2D1DeviceContext failed");
			LogError(hr);
			return false;
		}
	}
#endif

	//
	// Misc
	//

	// Allocate a descriptor.
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
	swapChainDesc.Width = _Width;
	swapChainDesc.Height = _Height;
	swapChainDesc.Format = BackBufferFormat; 
	swapChainDesc.Stereo = false;
	swapChainDesc.SampleDesc.Count = 1;                // don't use multi-sampling
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;// 2;                     // use double buffering to enable flip
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL; // DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // all apps must use this SwapEffect
	swapChainDesc.Flags = 0;
	
	{
		IDXGIAdapter* dxgiAdapter = nullptr;
		hr = dxgiDevice->GetAdapter(&dxgiAdapter);

		IDXGIFactory2* DXGIFactory;
		hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&DXGIFactory));

		hr = DXGIFactory->CreateSwapChainForHwnd(
			_D3D11Device,
			(HWND)hWND,
			&swapChainDesc,
			nullptr,
			nullptr,
			&_DXGISwapChain1);

		DXGIFactory->Release();
		dxgiAdapter->Release();
		dxgiDevice->Release();
	}


	//dxgiDevice->SetMaximumFrameLatency(1);

#if LX_D2D1
	if (gD2D1)
	{
		// DirectWrite
		{
			IDWriteFactory* DWriteFactory = nullptr;
			hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&DWriteFactory));

			if (hr != S_OK)
			{
				LogE(LXDirectX11, L"DWriteCreateFactory failed");
				LogError(hr);
				return false;
			}
			
			hr = DWriteFactory->CreateTextFormat(L"Consolas",
						NULL,
						DWRITE_FONT_WEIGHT_NORMAL,
						DWRITE_FONT_STYLE_NORMAL,
						DWRITE_FONT_STRETCH_NORMAL,
						LX_CONSOLE_FONT_SIZE,
						L"", //locale
						&_DWriteTextFormat);

			if (hr != S_OK)
			{
				LogE(LXDirectX11, L"CreateTextFormat failed");
				LogError(hr);
				return false;
			}
			DWriteFactory->Release();
			//m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			//m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		}

	}
#endif

return CreateBuffers();
}

bool LXDirectX11::Init(HWND hWND)
{
	bool gCreateDeviceDebug = CSet_CreateDeviceDebug.GetValue();

	CHK(hWND);
	if (!hWND)
	{
		LogE(LXDirectX11, L"HWND is null");
		return false;
	}

	_hWND = hWND;
	HRESULT hr;

	//
	// Device & Swap chain
	//

	if (1)
	{


#if 0

		IDXGIFactory1*  DXGIFactory;
		hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void **)&DXGIFactory);
		if (FAILED(hr))
		{
			LogE(LXDirectX11, L"Can't create DXGIFactory");
			LogError(hr);
			return false;
		}

		IDXGIAdapter1* DXGIAdapter = nullptr;
		if (DXGIFactory->EnumAdapters1(0, &DXGIAdapter) == DXGI_ERROR_NOT_FOUND)
		{
			LogE(LXDirectX11, L"No adapters found");
			return false;
		}

		IDXGIOutput* DXGIOutput;
		if (DXGIAdapter->EnumOutputs(0, &DXGIOutput) == DXGI_ERROR_NOT_FOUND)
		{
			LogE(LXDirectX11, L"No outputs found");
			return false;
		}

#endif

		DXGI_SWAP_CHAIN_DESC scd;
		ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));


		scd.BufferCount = 1;                                    // one back buffer
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
		scd.OutputWindow = hWND;                                // the window to be used
		scd.SampleDesc.Count = 4;                               // how many multisamples
		scd.Windowed = TRUE;                                    // windowed/full-screen mode

																// create a device, device context and swap chain using the information in the scd struct
		hr = D3D11CreateDeviceAndSwapChain(
			nullptr/*DXGIAdapter*/,
			/*D3D_DRIVER_TYPE_UNKNOWN*/D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			NULL,
			NULL,
			NULL,
			D3D11_SDK_VERSION,
			&scd,
			&_DXGISwapChain,
			&_D3D11Device,
			NULL,
			&_D3D11DeviceContext);

		if (hr != S_OK)
		{
			LogE(LXDirectX11, L"D3D11CreateDeviceAndSwapChain failed");
			return false;
		}

		return true;

	}
	else
	{

		//
		// D3DDevice & D3DDeviceContext & DXGISwapChain
		//

		DXGI_MODE_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));
		bufferDesc.Width = 1024;// Width;
		bufferDesc.Height = 1024;// Height;
		bufferDesc.RefreshRate.Numerator = 60;
		bufferDesc.RefreshRate.Denominator = 1;
		bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		// SwapChain
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
		swapChainDesc.BufferDesc = bufferDesc;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.OutputWindow = (HWND)hWND;
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		UINT CreationFlags = 0;

		if (gCreateDeviceDebug == true)
			CreationFlags |= D3D11_CREATE_DEVICE_DEBUG;

		hr = D3D11CreateDeviceAndSwapChain(
			nullptr,					// Specify nullptr to use the default adapter.
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			CreationFlags,				// creationFlags,
			0,
			0,
			D3D11_SDK_VERSION,			// Windows Store apps must set this to D3D11_SDK_VERSION.
			&swapChainDesc,				// Returns the Direct3D device created.
			&_DXGISwapChain,
			&_D3D11Device,
			NULL,
			&_D3D11DeviceContext);

		if (hr != S_OK)
		{
			LogE(LXDirectX11, L"D3D11CreateDeviceAndSwapChain failed");
			return false;
		}

		//
		// D3D11Debug
		//

		if (gCreateDeviceDebug == true)
		{
			hr = _D3D11Device->QueryInterface(IID_PPV_ARGS(&_D3D11Debug));
			if (hr != S_OK)
			{
				LogE(LXDirectX11, L"QueryInterface ID3D11Debug failed");
				LogError(hr);
				return false;
			}
		}

		//
		// Annotation 
		//

		hr = _D3D11DeviceContext->QueryInterface(__uuidof(ID3DUserDefinedAnnotation), (void**)&_D3DUserDefinedAnnotation);
		if (hr != S_OK)
		{
			LogE(LXDirectX11, L"QueryInterface ID3DUserDefinedAnnotation failed");
			return false;
		}

		//
		// Create the BackBuffer
		//

		ID3D11Texture2D* BackBuffer;
		hr = _DXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);
		if (hr != S_OK)
		{
			LogE(LXDirectX11, L"GetBuffer failed");
			return false;
		}

		//
		// Create our Render Target
		//

		hr = _D3D11Device->CreateRenderTargetView(BackBuffer, NULL, &_D3D11RenderTargetView);
		if (hr != S_OK)
		{
			LogE(LXDirectX11, L"CreateRenderTargetView failed");
			return false;
		}
		BackBuffer->Release();

		return true;
	}
}

void LXDirectX11::Resize(UINT Width, UINT Height)
{
	CHK(IsRenderThread());

	_Width = Width;
	_Height = Height;
	
	ReleaseBuffers();
	
	HRESULT hr;

	if (_DXGISwapChain1)
	{
		hr = _DXGISwapChain1->ResizeBuffers(1, _Width, _Height, DXGI_FORMAT_UNKNOWN, 0);
		if(hr != S_OK)
		{
			LogE(LXDirectX11, L"ResizeBuffers failed");
			LogError(hr);
			return;
		}
	}

	if (_DXGISwapChain)
	{
		hr = _DXGISwapChain->ResizeBuffers(1, _Width, _Height, DXGI_FORMAT_UNKNOWN, 0);
		if (hr != S_OK)
		{
			LogE(LXDirectX11, L"ResizeBuffers failed");
			LogError(hr);
			return;
		}
	}

	VRF(CreateBuffers());
}

void LXDirectX11::Present()
{
	bool gVSync = CSet_VSync.GetValue();

	if (_DXGISwapChain1)
		_DXGISwapChain1->Present(gVSync ? 1 : 0, 0);
	else if (_DXGISwapChain)
		_DXGISwapChain->Present(gVSync ? 1 : 0, 0);
}
