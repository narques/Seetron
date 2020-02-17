//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

enum class ETextureChannel
{
	ChannelR,
	ChannelG,
	ChannelB,
	ChannelA,
	ChannelRGB,
};

//------------------------------------------------------------------------------------------------------
//
// Texture and Bitmap Formats
//
//------------------------------------------------------------------------------------------------------

enum class ETextureFormat
{
	LXUndefined = -1,
	LX_R16_USHORT,				// 16 Bits
	LX_R32_FLOAT,				// 32 Bits
	LX_R16G16_USHORT,			// 32 Bits
	LX_R16G16_FLOAT,			// 32 Bits
	LX_R32G32_FLOAT,			// 64 Bits
	LX_RGB8,					// 24 bITS
	LX_RGB32F,					// 32 Bits
	LX_RGBA8,					// 32 Bits
	LX_RGBA32F,					// 128 Bits
};

enum class ETextureType
{
	LX_Uint8,
	LX_Uint16,
	LX_Float,
};

//------------------------------------------------------------------------------------------------------
//
// Texture sampler Parameters
//
//------------------------------------------------------------------------------------------------------

enum class ETextureMinFilter
{
	LXMinFilterNearest,
	LXMinFilterLinear,
	LXMinFilterNearestMipMapNearest,
	LXMinFilterLinearMipMapNearest,
	LXMinFilterNearestMipMapLinear,
	LXMinFilterLinearMipMapLinear,
};

enum class ETextureMagFilter
{
	LXMagFilterNearest,
	LXMagFilterLinear,
};

enum class ETextureWrap
{
	LXClampToBorder,
	LXClampToEdge,
	LXMirroredRepeat,
	LXRepeat
};
