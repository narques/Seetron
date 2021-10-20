//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXSmartObject.h"
#include "LXColor.h"
#include "LXMaterial.h"

class LXProject;
class LXRenderer;
class LXTexture;

enum LXBackgroundType /* DataModel : The new items must be added to the end */
{
	LX_BGTYPE_NONE,
	LX_BGTYPE_COLOR,
	LX_BGTYPE_GRADIENT,
	LX_BGTYPE_TEXTURE,
	LX_BGTYPE_ENVCUBE,
	LX_BGTYPE_SUN
};

class LXENGINE_API LXBackground : public LXSmartObject
{

public:

	LXBackground(LXProject* pDocument);
	virtual ~LXBackground(void);

	// Misc
	bool				NeedDraw			( ) const { return (m_eType != LX_BGTYPE_NONE) && (m_eType != LX_BGTYPE_COLOR); }
	
private:

	void				DefineProperties	( );
		
private:

	// Properties
	GetSet				( LXBackgroundType, m_eType, Type );
	GetSet				( LXColor4f, m_color, Color );
	GetSet				( LXColor4f, m_colorBottom, ColorBottom );
	GetSetDefPtr		( LXTexture, _pTexture, Texture, NULL);						 // RadianceMap	  ( Specular )
	GetSetDefPtr		( LXTexture, _pTextureIrradiance, TextureIrradiance, NULL);	 // IrradianceMap ( Diffuse  )
	GetSetDef			( float, _fBrightness, Brightness, 1.f );
	
	// Misc
	LXProject*			_Project;
};
