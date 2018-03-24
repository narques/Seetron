//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include "LXTexture.h"
#include "LXFilepath.h"

class LXCORE_API LXBitmap : public LXObject
{

public:

	LXBitmap(void);
	LXBitmap(uint Width, uint Height, ETextureFormat Format);
	virtual ~LXBitmap(void);

	bool					Load(const LXFilepath& strFilename);
	LXFilepath				GetFilename() { return m_strFilename; }
	uint					GetWidth() const { CHK(m_nWidth); return m_nWidth; }
	uint					GetHeight() const { CHK(m_nHeight); return m_nHeight; }
	int						GetPixelSize();
	template<typename T>
	T*						GetPixel(int x, int y) { return &((T*)m_pBytes)[(x + y * m_nHeight) * m_nComponents]; }
	void*					GetPixels() const { CHK(m_pBytes); return m_pBytes; }
	template<typename T>
	T*						GetPixels() const { CHK(m_pBytes); return (T*)m_pBytes; }
	
	ETextureFormat	GetInternalFormat() const { return m_eInternalFormat; }

	void OnBitmapChanged(std::function<void()> Func) { _func = Func; }
	std::function<void()> _func;
	void InvokeOnBitmapChanged() { if (_func) _func(); }
	

	// Misc

	void ToBlack();

	static int GetComponentCount(ETextureFormat Format);

	static bool  IsPowerOfTwo(uint v)
	{
		return !((v - 1) & v);
	}

	static uint GetUpperPowerOfTwo(uint v)
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

	static uint GetNumMipLevels(uint width, uint height)
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

public:

	void*					m_pBytes = nullptr;
	uint					m_nWidth = 0;
	uint					m_nHeight = 0;
	ETextureFormat			m_eInternalFormat = ETextureFormat::LXUndefined;
	uint					m_nComponents = 0;
	LXFilepath				m_strFilename;
};
