//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXBitmap.h"
#include "FreeImage3170/FreeImage.h"
#include "LXMemory.h" // --- Must be the last included ---

#pragma comment(lib, "freeimage.lib")

LXBitmap::LXBitmap(void)
{
}

LXBitmap::LXBitmap(uint width, uint height, ETextureFormat format) :
	_width(width),
	_height(height),
	_format(format)
{
	_components = GetComponentCount();
	uint bytePerPixel = GetPixelSize(format);
	int size = width * height * bytePerPixel;
	_bytes = new BYTE[size];
	ZeroMemory(_bytes, size);
}

LXBitmap::~LXBitmap(void)
{
	LX_SAFE_DELETE(_bytes);
}

bool LXBitmap::Load( const LXFilepath& strFilename )
{
	if (!strFilename.IsFileExist())
	{
		LogW(Bitmap, L"File not found: %s", strFilename.GetBuffer()); 
		return false;
	}

	_filename = strFilename;
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
	//if still unknown, return failure
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
		uint newWidth = GetUpperPowerOfTwo(width);
		uint newHeight = GetUpperPowerOfTwo(height);

		if (FIBITMAP* newdib = FreeImage_Rescale(dib, newWidth, newHeight))
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
			_components = 1;
			_format = ETextureFormat::LX_R16_USHORT;
		}
	break;

	case FIC_RGB: // 16 24 or 32 Bit
	{
		switch (BPP)
		{
		case 24:
		{
			_components = 3;
			_format = ETextureFormat::LX_RGB8;
		}
		break;

		case 32:
		{
			_components = 4;
			_format = ETextureFormat::LX_RGBA8;
		}
		break;

		case 96:
		{
			_components = 3;
			_format = ETextureFormat::LX_RGB32F;
		}
		break;

		case 128:
		{
			_components = 4;
			_format = ETextureFormat::LX_RGBA32F;
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
			_components = 4;
			_format = ETextureFormat::LX_RGBA8;
		}
		else if (BPP == 128)
		{
			_components = 4;
			_format = ETextureFormat::LX_RGBA32F;
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

	uint BytePerPixel = BPP / _components / 8;
	int count = width * height * _components * BytePerPixel;

	_bytes = new BYTE[count];
	_width = width;
	_height = height;

	::memcpy(_bytes, bits, count);
	
	FreeImage_Unload(dib);
		
	return true;
}

int LXBitmap::GetBitmapSize()
{
	return GetPixelSize(_format) * _width * _height;
}

int LXBitmap::GetLineSize()
{
	return GetPixelSize(_format) * _width;
}

int LXBitmap::GetPixelSize()
{
	return GetPixelSize(_format);
}

void LXBitmap::ToBlack()
{
	int size = GetPixelSize(_format) * _width * _height;
	ZeroMemory(_bytes, size);
}

int LXBitmap::GetPixelSize(ETextureFormat Format)
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

uint LXBitmap::GetUpperPowerOfTwo(uint v)
{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v;
}

uint LXBitmap::GetNumMipLevels(uint width, uint height)
{
	UINT numLevels = 1;
	while (width > 1 && height > 1)
	{
		width = max<unsigned int>(width / 2, 1);
		height = max<unsigned int>(height / 2, 1);
		++numLevels;
	}
	return numLevels;
}

int LXBitmap::GetComponentCount()
{
	switch (_format)
	{
	case ETextureFormat::LX_R16_USHORT: return 1;
	case ETextureFormat::LX_R16G16_USHORT: return 2;
	case ETextureFormat::LX_R16G16_FLOAT: return 2;
	case ETextureFormat::LX_R32G32_FLOAT: return 2;
	case ETextureFormat::LX_RGB8: return 3;
	case ETextureFormat::LX_RGBA8:return 4;
	case ETextureFormat::LX_R32_FLOAT:return 1;
	case ETextureFormat::LX_RGB32F: return 3;
	case ETextureFormat::LX_RGBA32F: return 4;
	default: CHK(0);  return -1;
	}
}

int LXBitmap::GetComponentSize(ETextureFormat Format)
{
	switch (Format)
	{
	case ETextureFormat::LX_R16_USHORT: return 2;
	case ETextureFormat::LX_R16G16_USHORT: return 2;
	case ETextureFormat::LX_R16G16_FLOAT: return 2;
	case ETextureFormat::LX_R32G32_FLOAT: return 4;
	case ETextureFormat::LX_RGB8: return 1;
	case ETextureFormat::LX_RGBA8: return 1;
	case ETextureFormat::LX_R32_FLOAT: return 4;
	case ETextureFormat::LX_RGB32F: return 4;
	case ETextureFormat::LX_RGBA32F: return 4;
	default: CHK(0); return -1;
	}
}

ETextureType LXBitmap::GetComponentType(ETextureFormat Format)
{
	switch (Format)
	{
	case ETextureFormat::LX_R16_USHORT: return ETextureType::LX_Uint16;
	case ETextureFormat::LX_R16G16_USHORT: return ETextureType::LX_Uint16;
	case ETextureFormat::LX_R16G16_FLOAT: return ETextureType::LX_Float;
	case ETextureFormat::LX_R32G32_FLOAT: return ETextureType::LX_Float;
	case ETextureFormat::LX_RGB8: return ETextureType::LX_Uint8;
	case ETextureFormat::LX_RGBA8: return ETextureType::LX_Uint8;
	case ETextureFormat::LX_R32_FLOAT: return ETextureType::LX_Float;
	case ETextureFormat::LX_RGB32F: return ETextureType::LX_Float;
	case ETextureFormat::LX_RGBA32F: return ETextureType::LX_Float;
	default: CHK(0); return ETextureType::LX_Uint8;
	}
}
