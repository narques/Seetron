//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXTextureManager.h"
#include "LXFilepath.h"
#include "LXLogger.h"
#include "LXTexture.h"
#include "LXTextureD3D11.h"
#include "LXBitmap.h"

LXTextureManager::LXTextureManager()
{
}

LXTextureManager::~LXTextureManager()
{
	_MapTextureD3D11.clear();
}

const std::shared_ptr<LXTextureD3D11>& LXTextureManager::GetTexture(LXTexture* Texture)
{
	auto It = _MapTextureD3D11.find(Texture);

	if (It != _MapTextureD3D11.end())
	{
		return It->second;
	}
	else
	{
		std::shared_ptr<LXTextureD3D11> TextureD3D11 = CreateTexture(Texture);
		_MapTextureD3D11[Texture] = TextureD3D11;
		return _MapTextureD3D11[Texture];
	}
}

std::shared_ptr<LXTextureD3D11> LXTextureManager::CreateTexture(const LXTexture* Texture)
{
	if (!Texture)
	{
		LogE(LXTextureManager, L"CreateTexture fails, the TextureSamplerNode is null");
		return nullptr;
	}

	const LXFilepath& Filepath = Texture->GetFilepath();

	std::shared_ptr<LXTextureD3D11> TextureD3D11;

	if (Texture->TextureSource == ETextureSource::TextureSourceBitmap)
	{
		if (LXBitmap* Bitmap = Texture->GetBitmap(0))
		{
			DXGI_FORMAT Format = LXTextureD3D11::GetDXGIFormat(Bitmap->GetInternalFormat());
			TextureD3D11 = std::make_shared<LXTextureD3D11>(Bitmap->GetWidth(), Bitmap->GetHeight(), Format, Bitmap->GetPixels(), Bitmap->GetPixelSize(), 0, D3D11_FILTER_MIN_MAG_MIP_POINT);
		}
		else
		{
			LogE(LXTextureManager, L"CreateTexture fails, Bitmap is null");
		}
	}
	else if (Texture->TextureSource == ETextureSource::TextureSourceMaterial)
	{
		CHK(0); // Find a more generic way to access the rendered texture
		//TextureD3D11 = GetEngine().GetRenderer()->RenderPassDynamicTexture->AddGraph(Texture);
	}
	else
	{
		// Texture without source
		LogE(LXTextureManager, L"CreateTexture fails, No source specified");
		CHK(0);
	}

	if (TextureD3D11 == nullptr)
	{
		LogE(LXTextureManager, L"CreateTexture fails, unknown error");
	}

	return TextureD3D11;
}
