//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXTextureManager.h"
#include "LXFilepath.h"
#include "LXTexture.h"
#include "LXTextureD3D11.h"
#include "LXBitmap.h"
#include "LXMemory.h" // --- Must be the last included ---

LXTextureManager::LXTextureManager()
{
}

LXTextureManager::~LXTextureManager()
{
	_MapTextureD3D11.clear();
}

const shared_ptr<LXTextureD3D11>& LXTextureManager::GetTexture(LXTexture* Texture)
{
	auto It = _MapTextureD3D11.find(Texture);

	if (It != _MapTextureD3D11.end())
	{
		return It->second;
	}
	else
	{
		shared_ptr<LXTextureD3D11> TextureD3D11 = CreateTexture(Texture);
		_MapTextureD3D11[Texture] = TextureD3D11;
		return _MapTextureD3D11[Texture];
	}
}

shared_ptr<LXTextureD3D11> LXTextureManager::CreateTexture(const LXTexture* Texture)
{
	if (!Texture)
	{
		LogE(LXTextureManager, L"CreateTexture fails, the TextureSamplerNode is null");
		return nullptr;
	}

	const LXFilepath& Filepath = Texture->GetFilepath();

	shared_ptr<LXTextureD3D11> TextureD3D11;

	if (Texture->TextureSource == ETextureSource::TextureSourceBitmap)
	{
		CHK(Texture->GetGraph() == nullptr);
		if (LXBitmap* Bitmap = Texture->GetBitmap(0))
		{
			DXGI_FORMAT Format = LXTextureD3D11::GetDXGIFormat(Bitmap->GetInternalFormat());
			TextureD3D11 = make_shared<LXTextureD3D11>(Bitmap->GetWidth(), Bitmap->GetHeight(), Format, Bitmap->GetPixels(), Bitmap->GetPixelSize(), 0, D3D11_FILTER_MIN_MAG_MIP_POINT);
		}
		else
		{
			LogE(LXTextureManager, L"CreateTexture fails, Bitmap is null");
		}
	}
	else if (Texture->TextureSource == ETextureSource::TextureSourceDynamic)
	{
		CHK(Texture->GetGraph());
		CHK(0); // Find a more generic way to access the rendered texture
		//TextureD3D11 = GetCore().GetRenderer()->RenderPassDynamicTexture->AddGraph(Texture);
	}
	else
	{
		// Texture without source
		LogE(LXTextureManager, L"CreateTexture fails, No source specified");
		CHK(0);
	}

	if (TextureD3D11)
		TextureD3D11->Slot = -1;
	else
		LogE(LXTextureManager, L"CreateTexture fails, unknown error");

	return TextureD3D11;
}
