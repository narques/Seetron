//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include "LXFilepath.h"
#include "LXFormats.h"

class LXCORE_API LXBitmap : public LXObject
{

public:

	LXBitmap(void);
	LXBitmap(uint Width, uint Height, ETextureFormat Format);
	virtual ~LXBitmap(void);

	bool Load(const LXFilepath& strFilename);
	LXFilepath GetFilename() { return _filename; }

	uint GetWidth() const { CHK(_width); return _width; }
	uint GetHeight() const { CHK(_height); return _height; }
	ETextureFormat	GetInternalFormat() const { return _format; }
	
	// Accessors
	template<typename T>
	T* GetPixel(int x, int y) { return &((T*)_bytes)[(x + y * _width) * _components]; }
	void* GetPixels() const { CHK(_bytes); return _bytes; }

	// Return the Pixel (first Component) normalized between [0.f, 1.f]
	float GetNormalized(int x, int y)
	{
		switch (GetComponentType(_format))
		{
		case ETextureType::LX_Uint8:
		{
			uint8 p = GetPixel<uint8>(x, y)[0];
			return (float)(p) / UCHAR_MAX;
		}
		break;
		case ETextureType::LX_Uint16:
		{
			uint16 p = GetPixel<uint16>(x, y)[0];
			return (float)(p) / USHRT_MAX;
		}
		break;
		case ETextureType::LX_Float:
		{
			return GetPixel<float>(x, y)[0];
		}
		break;

		default:
		{
			CHK(0);
			return 0.f;
		}
		break;
		}

	}

	// Returns the Bitmap size in Bytes
	int GetBitmapSize();
	
	// Returns the Width in Bytes
	int GetLineSize();

	// Returns the Pixel size in Bytes
	int GetPixelSize();

	// Returns the Component count
	int GetComponentCount();
		
	// Returns the Component size in Bytes
	static int GetComponentSize(ETextureFormat Format);

	// Returns the Component Type (uint8, uint16,...)
	static ETextureType GetComponentType(ETextureFormat Format);

	// Returns the Pixel size in Bytes
	static int GetPixelSize(ETextureFormat Format);

	// Fills bitmap with 0
	void ToBlack();

	// Misc
	bool IsPowerOfTwo() { return IsPowerOfTwo(_width) && IsPowerOfTwo(_height); }
	static bool  IsPowerOfTwo(uint v) { return !((v - 1) & v); }
	static uint GetUpperPowerOfTwo(uint v);
	static uint GetNumMipLevels(uint width, uint height);

public:

	atomic<bool>			Busy = false;

private:
		
	void*					_bytes = nullptr;
	uint					_width = 0;
	uint					_height = 0;
	ETextureFormat			_format = ETextureFormat::LXUndefined;
	uint					_components = 0;
	LXFilepath				_filename;

};
