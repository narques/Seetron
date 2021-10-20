//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXPlatform.h"

// --- Config ---
#define LX_D2D1 1
#define LX_USE_D3D11_1 0
// ---------------- 

#include <d3d11_1.h>

#if LX_D2D1
#include <D2d1_1.h>
#endif

extern LXENGINE_API bool NoOverwriteConstantBuffer; // TODO under LX_USE_D3D11_1 ?

class LXENGINE_API LXDirectX11
{

public:

	LXDirectX11(HWND hWND);
	~LXDirectX11();

	static ID3D11Device* GetCurrentDevice();
	static ID3D11DeviceContext* GetCurrentDeviceContext();
#if LX_USE_D3D11_1
	static ID3D11DeviceContext1* GetCurrentDeviceContext1();
#endif
	static LXDirectX11* GetCurrentDirectX11();

	void Resize(UINT, UINT);
	void Present();

	static void LogError(HRESULT hr);

private:

	bool CreateBuffers();
	void ReleaseBuffers();
	bool InitD3D11AndD2D1(HWND hWND);
	bool Init(HWND hWND);

public:
	
	ID3D11Device* _D3D11Device = nullptr;
	ID3D11DeviceContext* _D3D11DeviceContext = nullptr;
	ID3D11RenderTargetView* _D3D11RenderTargetView = nullptr;
	ID3D11Debug* _D3D11Debug = nullptr;
	IDXGISwapChain* _DXGISwapChain = nullptr;
	IDXGISwapChain1* _DXGISwapChain1 = nullptr;
	ID3DUserDefinedAnnotation* _D3DUserDefinedAnnotation = nullptr;

#if LX_D2D1
	ID2D1Device* _D2D1Device = nullptr;
	ID2D1DeviceContext* _D2D1DeviceContext = nullptr;
	ID2D1Bitmap1* _D2D1TargetBitmap = nullptr;
	IDWriteTextFormat* _DWriteTextFormat = nullptr;
#endif	

#if LX_USE_D3D11_1
	ID3D11DeviceContext1* _D3D11DeviceContext1 = nullptr;
#endif

private:

	void* _hWND = nullptr;
	UINT _Width = 1920;
	UINT _Height = 1080;
};

