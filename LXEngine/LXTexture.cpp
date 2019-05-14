//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXTexture.h"
#include "LXBitmap.h"
#include "LXLogger.h"
#include "LXMSXMLNode.h"
#include "LXMaterial.h"
#include "LXProject.h"
#include "LXRenderer.h"
#include "LXSettings.h"
#include "LXXMLDocument.h"
#include "LXMemory.h" // --- Must be the last included ---

LXTexture::LXTexture()
{
	DefineProperties();
}

LXTexture::~LXTexture(void)
{
	LX_SAFE_DELETE_ARRAY(_Bitmap);
	if (_material)
	{
		delete _material;
	}

	ReleaseDeviceTexture();
}

void Blit(BYTE* pSrc, int widthSrc, int heightSrc, BYTE* pDst, int widthDst, int heightDst)
{
	CHK(widthDst >= widthSrc);
	CHK(heightDst >= heightSrc);
	
	for (int j = 0; j < heightSrc; j++)
	{
		for (int i = 0; i < widthSrc; i++)
		{
			BYTE src = pSrc[j * widthSrc + i];
			CHK((j * widthDst + i) < (widthDst * heightDst));
			pDst[j * widthDst + i] = src;
		}
	}
}

bool LXTexture::Load()
{
	if (State == EResourceState::LXResourceState_Loaded)
		return true;

	bool Result = false;
	Result = LoadWithMSXML(_filepath);

	if (Result)
	{
		LoadSource();
		State = EResourceState::LXResourceState_Loaded;
		CreateDeviceTexture();
	}

	return false;
}

bool LXTexture::LoadSource()
{
	// The source file path is relative
	LXFilepath strFilename;
	if (Owner == EResourceOwner::LXResourceOwner_Engine)
		strFilename = GetSettings().GetDataFolder() + _SourceFilepath;
	else if (Owner == EResourceOwner::LXResourceOwner_Project)
		strFilename = GetCore().GetProject()->GetAssetFolder() + _SourceFilepath;
	else
		CHK(0);
		
	if (!strFilename.IsFileExist())
	{
		LogE(L"LXTexture : Source file %s not found : ", strFilename.GetBuffer());
		return false;
	}

	LX_SAFE_DELETE_ARRAY(_Bitmap);

// 	if ((LXFilepath(strFilename).GetFilename().MakeLower() == L"cubemap.xml") || (LXFilepath(strFilename).GetExtension().MakeLower() == L"cm"))
// 	{
// 		m_eTarget = ETextureTarget::LXTextureCubeMap;
// 
// 		LXXMLDocument xml;
// 		if (xml.Load(strFilename))
// 		{
// 			LXMSXMLNode* root = xml.GetRoot();
// 			for (LXMSXMLNode e = root->begin(); e != root->end(); e++)
// 			{
// 				LXString str = e.name();
// 				LXString strTexture = LXFilepath(strFilename).GetFilepath() + LXString(e.attr(L"Value"));
// 
// 				if (str == "POSITIVE_X")
// 					m_pBitmap[(int)ECubeEnvironmentFace::Right].Load(strTexture);
// 				else if (str == "NEGATIVE_X")
// 					m_pBitmap[(int)ECubeEnvironmentFace::Left].Load(strTexture);
// 				else if (str == "POSITIVE_Y")
// 					m_pBitmap[(int)ECubeEnvironmentFace::top].Load(strTexture);
// 				else if (str == "NEGATIVE_Y")
// 					m_pBitmap[(int)ECubeEnvironmentFace::Down].Load(strTexture);
// 				else if (str == "POSITIVE_Z")
// 					m_pBitmap[(int)ECubeEnvironmentFace::Front].Load(strTexture);
// 				else if (str == "NEGATIVE_Z")
// 					m_pBitmap[(int)ECubeEnvironmentFace::Back].Load(strTexture);
// 				else
// 					CHK(0);
// 			}
// 		}
// 	}
// 	else
	{
		_Bitmap = new LXBitmap[1];
		if (_Bitmap->Load(strFilename))
		{
			_nWidth = _Bitmap->GetWidth();
			_nHeight = _Bitmap->GetHeight();
			_eInternalFormat = _Bitmap->GetInternalFormat();
		}
		else
		{
			delete _Bitmap;
			return false;
		}
	}

	return true;
}

void LXTexture::SetSource(const LXFilepath& Filepath)
{
	_SourceFilepath = Filepath;
}

LXBitmap* LXTexture::GetBitmap( int index ) const 
{
	if (!_Bitmap)// && (State == LXResource::ResourceState_Unloaded))
		const_cast<LXTexture*>(this)->Load();
	
	if (_Bitmap)
		return &(_Bitmap[index]);
	else
		return nullptr;
}

void LXTexture::SetBitmap(LXBitmap* Bitmap)
{
	CHK(!_Bitmap);
	_Bitmap = Bitmap;
}

void LXTexture::SetMaterial(LXMaterial* material) 
{ 
	CHK(!_material);
	_material = material; 
}

void LXTexture::DefineProperties()
{
	//--------------------------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Texture");
	//--------------------------------------------------------------------------------------------------------------------------------

	LXPropertyEnum* PropType = DefinePropertyEnum(L"TextureType", GetAutomaticPropertyID(), (uint*)&_eTarget);
	PropType->AddChoice(L"Texture 1D", (uint)ETextureTarget::LXTexture1D);
	PropType->AddChoice(L"Texture 2D", (uint)ETextureTarget::LXTexture2D);
	PropType->AddChoice(L"Texture 2D", (uint)ETextureTarget::LXTexture3D);
	PropType->AddChoice(L"Texture CubeMap", (uint)ETextureTarget::LXTextureCubeMap);
	PropType->SetReadOnly(true);

	LXPropertyEnum* pPropSource = DefinePropertyEnum(L"TextureSource", GetAutomaticPropertyID(), (uint*)&TextureSource);
	pPropSource->AddChoice(L"Bitmap", (uint)ETextureSource::TextureSourceBitmap);
	pPropSource->AddChoice(L"Material", (uint)ETextureSource::TextureSourceMaterial);
	pPropSource->SetPersistent(false);
	pPropSource->SetReadOnly(true);

	LXPropertyEnum* pPropFormat = DefinePropertyEnum(L"Format", GetAutomaticPropertyID(), (uint*)&_eInternalFormat);
	pPropFormat->AddChoice(L"R16_USHORT", (uint)ETextureFormat::LX_R16_USHORT);
	pPropFormat->AddChoice(L"R32_FLOAT", (uint)ETextureFormat::LX_R32_FLOAT);
	pPropFormat->AddChoice(L"R16G16_USHORT", (uint)ETextureFormat::LX_R16G16_USHORT);
	pPropFormat->AddChoice(L"R16G16_FLOAT", (uint)ETextureFormat::LX_R16G16_FLOAT);
	pPropFormat->AddChoice(L"R32G32_FLOAT", (uint)ETextureFormat::LX_R32G32_FLOAT);
	pPropFormat->AddChoice(L"RGB8", (uint)ETextureFormat::LX_RGB8);
	pPropFormat->AddChoice(L"RGB32F", (uint)ETextureFormat::LX_RGB32F);
	pPropFormat->AddChoice(L"RGBA8", (uint)ETextureFormat::LX_RGBA8);
	pPropFormat->AddChoice(L"RGBA32F", (uint)ETextureFormat::LX_RGBA32F);

	pPropFormat->SetPersistent(false);
	pPropFormat->SetReadOnly(true);

	LXProperty* PropWidth = DefineProperty("Width", &_nWidth);
	PropWidth->SetPersistent(false);
	PropWidth->SetReadOnly(true);

	LXProperty* PropHeight = DefineProperty("Height", &_nHeight);
	PropHeight->SetPersistent(false);
	PropHeight->SetReadOnly(true);

	LXPropertyFilepath* PropFilePath = DefinePropertyFilepath("SourceFile", GetAutomaticPropertyID(), &_SourceFilepath);
	PropHeight->SetReadOnly(true);
}

void LXTexture::CreateDeviceTexture()
{
	CHK(_textureD3D11 == nullptr);
	if (GetRenderer())
		GetRenderer()->CreateDeviceTexture(this);
}

void LXTexture::ReleaseDeviceTexture()
{
	if (_textureD3D11 && GetRenderer())
	{
		GetRenderer()->ReleaseDeviceTexture(this);
	}
}
