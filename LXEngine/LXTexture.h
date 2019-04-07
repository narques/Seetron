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
class LXMaterial;
class LXTextureD3D11;

enum class ETextureTarget
{
	LXUndefined = -1,
	LXTexture1D,
	LXTexture2D,
	LXTexture3D,
	LXTextureCubeMap
};

enum class ETextureSource
{
	TextureSouceUndefined,
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
		
	ETextureFormat	GetInsternalFormat	( ) const { return _eInternalFormat; }
	void			SetInternalFormat	( ETextureFormat eInternalFormat) { _eInternalFormat = eInternalFormat; }
	
	void			SetTarget			( ETextureTarget eTarget) { _eTarget = eTarget; }
	ETextureTarget	GetTarget			( ) const { return _eTarget; }
		
	void			SetSize				( uint nWidth, uint nHeight ) { _nWidth = nWidth; _nHeight = nHeight; }
	void			SetDepth			( uint nDepth ){ _nDepth = nDepth; }

	LXBitmap*		GetBitmap			( int index ) const;
	void			SetBitmap			( LXBitmap* Bitmap );
	LXMaterial*		GetMaterial			( ) const { return _material; }
	void			SetMaterial			(LXMaterial* InGraph );

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

private:

	void			DefineProperties();
	void			CreateDeviceTexture();
	void			ReleaseDeviceTexture();

public:

	ETextureSource			TextureSource = ETextureSource::TextureSourceBitmap;
	
protected:

	ETextureTarget			_eTarget = ETextureTarget::LXUndefined;
	ETextureFormat			_eInternalFormat = ETextureFormat::LXUndefined;
	uint					_nWidth = 0;					
	uint					_nHeight = 0;	
	uint					_nDepth = 0;
		
private:
	
	// Texture sources
	LXFilepath				_SourceFilepath;
	LXBitmap*				_Bitmap = nullptr;				
	LXMaterial*				_material = nullptr;

	// Rendering
	LXTextureD3D11*			_textureD3D11 = nullptr;

};

