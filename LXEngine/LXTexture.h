//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXAsset.h"
#include "LXFormats.h"

class LXBitmap;
class LXFilepath;
class LXTextureD3D11;

enum class ETextureTarget
{
	LXTexture1D,
	LXTexture2D,
	LXTexture3D,
	LXTextureCubeMap
};

enum class ETextureSource // !DataModel
{
	TextureSourceBitmap, 
	TextureSourceMaterial
};

class LXCORE_API LXTexture : public LXAsset
{

public:

	LXTexture(void);
	virtual ~LXTexture(void);

	int				GetWidth			( ) const { return _nWidth; }
	int				GetHeight			( ) const { return _nHeight; }
		
	bool			LoadSource			( );
	void			SetSource			( const LXFilepath& Filepath );
		
	ETextureFormat	GetInternalFormat	( ) const { return _eInternalFormat; }
	void			SetInternalFormat	( ETextureFormat eInternalFormat) { _eInternalFormat = eInternalFormat; }
	
	void			SetTarget			( ETextureTarget eTarget) { _eTarget = eTarget; }
	ETextureTarget	GetTarget			( ) const { return _eTarget; }
		
	void			SetSize				( uint nWidth, uint nHeight ) { _nWidth = nWidth; _nHeight = nHeight; }
	void			SetDepth			( uint nDepth ){ _nDepth = nDepth; }

	bool			GenerateMipMap() const { return _generateMipMap; }

	LXBitmap*		GetBitmap			( int index ) const;
	
	//
	// Overridden From LXResource
	//

	bool			Load() override;
	LXString		GetFileExtension() override { return LX_TEXTURE_EXT; }

	//
	// Rendering
	//

	const LXTextureD3D11* GetDeviceTexture() const { return _textureD3D11; }
	void SetDeviceTexture(LXTextureD3D11* textureD3D11) { _textureD3D11 = textureD3D11; }
	bool CopyDeviceToBitmap();

private:

	void			DefineProperties();
	void			CreateDeviceTexture();
	void			ReleaseDeviceTexture();

public:

	ETextureSource			TextureSource = ETextureSource::TextureSourceBitmap;

	// For RenderTarget only
	LXDelegate<>			DeviceCreated;
	LXDelegate<>			BitmapChanged;

	// Rendering Task Status
	atomic<bool>			DeviceCreationEnqueued = false;
	atomic<bool>			CopyDeviceToBitmapEnqueued = false;

protected:

	ETextureTarget			_eTarget = ETextureTarget::LXTexture2D;
	ETextureFormat			_eInternalFormat = ETextureFormat::LX_RGBA8 ;
	uint					_nWidth = 512;					
	uint					_nHeight = 512;	
	uint					_nDepth = 0;
	bool                    _generateMipMap = true;
		
private:
	
	// Texture sources
	LXFilepath				_SourceFilepath;
	LXBitmap*				_Bitmap = nullptr;				
	
	// Rendering
	LXTextureD3D11*			_textureD3D11 = nullptr;
};

