//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXBitmap.h"
#include "LXCore.h"
#include "LXLogger.h"
#include "FreeImage3170/FreeImage.h"
#include "LXMemory.h" // --- Must be the last included ---

#pragma comment(lib, "freeimage.lib")

// Returns the format type size in byte.
int TextureFormatSize(ETextureFormat Format)
{
	switch (Format)
	{
	case ETextureFormat::LX_R16_USHORT: return 2;
	case ETextureFormat::LX_R16G16_USHORT: return 4;
	case ETextureFormat::LX_R16G16_FLOAT: return 4;
	case ETextureFormat::LX_R32G32_FLOAT: return 8;
	case ETextureFormat::LX_RGB8: return 3;
	case ETextureFormat::LX_RGBA8: return 4;
	case ETextureFormat::LX_R32_FLOAT: return 4;
	case ETextureFormat::LX_RGB32F: return 12;
	case ETextureFormat::LX_RGBA32F: return 16;
	default: CHK(0); return -1;
	}
}

LXBitmap::LXBitmap(void)
{
}

LXBitmap::LXBitmap(uint Width, uint Height, ETextureFormat Format) :
	m_nWidth(Width),
	m_nHeight(Height),
	m_eInternalFormat(Format)
{
	m_nComponents = GetComponentCount(Format);
	uint BytePerPixel = TextureFormatSize(Format);
	int size = Width * Height * BytePerPixel;
	m_pBytes = new BYTE[size];
	ZeroMemory(m_pBytes, size);
}

LXBitmap::~LXBitmap(void)
{
	LX_SAFE_DELETE(m_pBytes);
}

bool LXBitmap::Load( const LXFilepath& strFilename )
{
	if (!strFilename.IsFileExist())
	{
		LogW(Bitmap, L"File not found: %s", strFilename.GetBuffer()); 
		return false;
	}

	m_strFilename = strFilename;
	LXString strExtension = strFilename.GetExtension();

	//
	// FreeImage
	//

	FreeImage_Initialise();

	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	fif = FreeImage_GetFileTypeU(strFilename, 0);
	//if still unknown, try to guess the file format from the file extension
	if (fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilenameU(strFilename);
	//if still unkown, return failure
	if (fif == FIF_UNKNOWN)
		return false;
	
	FIBITMAP* dib = nullptr;

	//check that the plugin has reading capabilities and load the file
	if (FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_LoadU(fif, strFilename);
	if (!dib)
		return false;

	uint BPP = FreeImage_GetBPP(dib);
	FREE_IMAGE_COLOR_TYPE ColorType = FreeImage_GetColorType(dib);

	// Convert RGB to RGBA
	if ((ColorType == FIC_RGB))
	{
		if (BPP == 24)
		{
			if (FIBITMAP* newdib = FreeImage_ConvertTo32Bits(dib))
			{
				FreeImage_Unload(dib);
				dib = newdib;
			}
		}
		else if (BPP == 96)
		{
			if (FIBITMAP* newdib = FreeImage_ConvertToRGBAF(dib))
			{
				FreeImage_Unload(dib);
				dib = newdib;
			}
		}
	}	

	FreeImage_FlipVertical(dib);
	BYTE* bits = FreeImage_GetBits(dib);
	uint width = FreeImage_GetWidth(dib);
	uint height = FreeImage_GetHeight(dib);
	BPP = FreeImage_GetBPP(dib);
	ColorType = FreeImage_GetColorType(dib);
	
	if((bits == 0) || (width == 0) || (height == 0))
		return false;

	if (!LXBitmap::IsPowerOfTwo(width) || !LXBitmap::IsPowerOfTwo(height))
	{
		if (FIBITMAP* newdib = FreeImage_Rescale(dib, 2048, 2048))
		{
			FreeImage_Unload(dib);
			dib = newdib;
			bits = FreeImage_GetBits(dib);
			width = FreeImage_GetWidth(dib);
			height = FreeImage_GetHeight(dib);
		}
	}

	switch (ColorType)
	{
	case FIC_MINISBLACK: // Standard Greyscale Image
		{
			m_nComponents = 1;
			m_eInternalFormat = ETextureFormat::LX_R16_USHORT;
		}
	break;

	case FIC_RGB: // 16 24 or 32 Bit
	{
		switch (BPP)
		{
		case 24:
		{
			m_nComponents = 3;
			m_eInternalFormat = ETextureFormat::LX_RGB8;
		}
		break;

		case 32:
		{
			m_nComponents = 4;
			m_eInternalFormat = ETextureFormat::LX_RGBA8;
		}
		break;

		case 96:
		{
			m_nComponents = 3;
			m_eInternalFormat = ETextureFormat::LX_RGB32F;
		}
		break;

		case 128:
		{
			m_nComponents = 4;
			m_eInternalFormat = ETextureFormat::LX_RGBA32F;
		}
		break;

		default:
			CHK(0);

		}
	}
	break;

	case FIC_RGBALPHA: // 32 Bits, RGBA16 or RGBAF
	{
		if (BPP == 32)
		{
			m_nComponents = 4;
			m_eInternalFormat = ETextureFormat::LX_RGBA8;
		}
		else if (BPP == 128)
		{
			m_nComponents = 4;
			m_eInternalFormat = ETextureFormat::LX_RGBA32F;
		}
		else
		{
			CHK(0);
		}

	}
	break;

	default:
		{
			LogW(Bitmap,L"Unsupported File format: %s", strFilename);
			FreeImage_Unload(dib);
			return false;
		}
	}

	uint BytePerPixel = BPP / m_nComponents / 8;
	int count = width * height * m_nComponents * BytePerPixel;

	m_pBytes = new BYTE[count];
	m_nWidth = width;
	m_nHeight = height;

	::memcpy(m_pBytes, bits, count);
	
	FreeImage_Unload(dib);
		
	return true;
}

int LXBitmap::GetPixelSize()
{
	return TextureFormatSize(m_eInternalFormat);
}

void LXBitmap::ToBlack()
{
	int size = TextureFormatSize(m_eInternalFormat) * m_nWidth * m_nHeight;
	ZeroMemory(m_pBytes, size);
}

int LXBitmap::GetComponentCount(ETextureFormat Format)
{
	switch (Format)
	{
	case ETextureFormat::LX_R16_USHORT: return 1;
	case ETextureFormat::LX_R16G16_USHORT: return 2;
	case ETextureFormat::LX_R16G16_FLOAT: return 2;
	case ETextureFormat::LX_R32G32_FLOAT: return 2;
	case ETextureFormat::LX_RGBA8:return 4;
	case ETextureFormat::LX_R32_FLOAT:return 1;
	default: CHK(0);  return -1;
	}
}
