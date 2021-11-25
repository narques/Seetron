//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXBackground.h"
#include "LXSettings.h"
#include "LXViewport.h"
#include "LXProject.h"
#include "LXProperty.h"
#include "LXTexture.h"
#include "LXPropertyManager.h"
#include "LXActorLight.h"

LXBackground::LXBackground(LXProject* pDocument):
m_eType(LX_BGTYPE_GRADIENT),
m_color(0.0f, 0.0f, 0.0f, 0.0f),
m_colorBottom(0.4745f, 0.5765f, 0.7098f, 0.0f),
_Project(pDocument)
{
	SetName(L"Background");
	DefineProperties();
}

LXBackground::~LXBackground(void)
{
}

void LXBackground::DefineProperties( ) 
{
	// --------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Background");
	// --------------------------------------------------------------------------------------------------------------
	
	LXPropertyEnum* pPropType = DefinePropertyEnum(L"Type", LXPropertyID::BACKGROUND_TYPE, (uint*)&m_eType);
	pPropType->AddChoice(L"None", LX_BGTYPE_NONE);
	pPropType->AddChoice(L"Color", LX_BGTYPE_COLOR);
	pPropType->AddChoice(L"Gradient", LX_BGTYPE_GRADIENT);
	//pPropType->AddChoice(L"Sun", LX_BGTYPE_SUN);
	pPropType->AddChoice(L"Environment", LX_BGTYPE_ENVCUBE);

	// --------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Colors");
	// --------------------------------------------------------------------------------------------------------------
		
	// Top Color
	DefinePropertyColor4f( L"TopColor", LXPropertyID::BACKGROUND_TOPCOLOR, &m_color );

	// Bottom Color
	DefinePropertyColor4f( L"BottomColor", LXPropertyID::BACKGROUND_BOTTOMCOLOR, &m_colorBottom );

	// --------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Environment Texture ");
	// --------------------------------------------------------------------------------------------------------------

	// Environment 
	LXPropertyAsset::Create(this, L"Environment", LXPropertyID::BACKGROUND_TEXTURE, (LXAssetPtr*)&_pTexture);

	// Environment 
	LXPropertyAsset::Create(this, L"Irradiance", LXPropertyID::BACKGROUND_TEXTURE_IRRADIANCE, (LXAssetPtr*)&_pTextureIrradiance);

	// Mapping
	{
// 		auto p = DefinePropertyEnum(L"Mapping", GetAutomaticPropertyID(), (uint*)&_eMapping);
// 		p->AddChoice(L"UV", LXMaterial::LX_TEXTURECOORD_UVMAP);
// 		p->AddChoice(L"Sphere Env", LXMaterial::LX_TEXTURECOORD_ENV_SPHERE);
// 		p->AddChoice(L"Cube Env", LXMaterial::LX_TEXTURECOORD_ENV_CUBE);
// 		p->AddChoice(L"Screen", LXMaterial::LX_TEXTURECOORD_ENV_SCREEN);
// 		p->AddChoice("Equirectangular Env", LXMaterial::LX_TEXTURECOORD_ENV_Equirectangular);
	}
	
	// Brightness
	{
		auto p = DefinePropertyFloat(L"Intensity", LXPropertyID::BackgroundIntensity, &_fBrightness);
		p->SetMinMax(0.f, LX_MAX_INTENSITY);
	}
}
