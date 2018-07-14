//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXSmartObject.h"

class LXProject;
class LXBBox;
class LXTexture;

enum LXNearFarMode
{
	LX_NEARFAR_AUTO,
	LX_NEARFAR_USER
};

enum LXLayerTransition
{
	LX_VIEWSTATE_LAYERTRANSITION_OPACITY,
	LX_VIEWSTATE_LAYERTRANSITION_DEEPNESS
};

enum LXDistributionFunction
{
	Beckmann,
	GaussianV1,
	GaussianV2,
	GGX
};

class LXCORE_API LXRenderingLayer : public LXSmartObject
{

public:

	LXRenderingLayer();

	//
	// Overridden from LXSmartObject
	//

	void DefineProperties() 
	{
		DefinePropertyBool(L"Outlines", LXPropertyID::RENDERING_OUTLINES, &_outlines);
		DefinePropertyAsset(L"Material", LXPropertyID::RENDERING_MATERIAL, (LXAsset**)&_material);
	}

	GetSetDefPtr(LXMaterial, _material, Material, NULL);
	GetSetDef(bool, _outlines, Outlines, false);

	//LX_DAE(bool, _outlines, false, Outlines, LXPropertyID::RENDERING_OUTLINES);
	//LX_DAE_PTR(LXMaterial, _material, NULL, Material, LXPropertyID::RENDERING_MATERIAL);
};

class LXCORE_API LXViewState : public LXSmartObject
{

public:

	LXViewState(LXProject* pDocument);
	virtual ~LXViewState(void);

	//
	// Overridden from LXSmartObject
	//
	void					DefineProperties( ) ;
	virtual bool			OnSaveChild( const TSaveContext& saveContext  ) const override;
	virtual bool			OnLoadChild( const TLoadContext& loadContext ) override;

	void					ZoomOnBBoxAnimated( const LXBBox& box, bool bAnimated = true );

	uint					GetBufferToVisualize() const { return _BufferToVisualize; }
		
private:

	// External objects

	LXProject*				_Project;

	//
	// Rendering options
	//
	
	// Misc
// 	DeclGet		(float, m_fNear, Near);
// 	DeclGet		(float, m_fFar, Far);
	GetSetDef	(bool, m_bUseBlitFunction, UseBlitFunction, false);
	GetSet		(float, m_fHour, Hour);

	// Ground
	GetSet		(bool, m_bGround, Ground);
	GetSet		(bool, m_bMirror, Mirror);

	// Lighting
	GetSet		(bool, m_bLighting, Lighting);
	GetSetDef	(bool, _bDeferredLighting, DeferredLighting, false);
	GetSet		(bool, m_bShadows, Shadows);

	// Ambient Occlusion
	GetSet		(bool, m_bSSAO, SSAO);
	GetSet		(float, m_fSSAOPower, SSAOPower);
	GetSet		(float, m_fSSAORadius, SSAORadius);
	GetSet		(bool, m_bSSAOSmooth, SSAOSmooth);
	
	// Glow/Bloom
	GetSetDef	(bool, m_bGlow, Glow, true);
	GetSetDef	(float, m_fGlowFactor, GlowFactor, 0.f);
	GetSetDef	(uint, _nDownSampleFactor, DownSampleFactor, 4);
	GetSetDef	(uint, _nBlurPassCount, BlurPassCount, 1);

	// Additional Effects
	GetSet		(bool, m_bSilhouette, Silhouette);
	GetSet		(bool, m_bDepthOfField, DepthOfField);
	GetSet		(bool, m_bFXAA, FXAA);
	GetSetDef	(float, _fExposure, Exposure, 1.f);
	//GetSetDef	(float, _fGamma, Gamma, 1.f);
	GetSetDef	(float, _fBrightness, Brightness, 1.f);
	GetSetDefPtr(LXTexture, _texture, Texture, NULL);

	// Layers
	GetSet		(bool, m_bLayered, Layered);
	GetSetDef	(LXLayerTransition, m_eLayerTransition, LayerTransition, LX_VIEWSTATE_LAYERTRANSITION_OPACITY);
	GetSetDef	(float, m_fLayerOpacity, LayerOpacity, 0.5f);
	GetSetDef	(float, m_fLayerDeepness, LayerDeepness, 0.5f);
	GetSetDef	(bool, m_bLayerUseMask, LayerUseMask, true);
	//GetSet		(LXRenderingLayer, _renderingLayer0, RenderingLayer0);
	//GetSet		(LXRenderingLayer, _renderingLayer1, RenderingLayer1);

	// Advanced 
	GetSetDef(LXDistributionFunction, _disributionFunc, DistributionFunc, Beckmann);

	// Debug purpose.
	uint _BufferToVisualize = 0;
	
};

typedef list<LXViewState*> ListViewStates;

