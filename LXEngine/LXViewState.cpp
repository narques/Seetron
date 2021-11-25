//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXViewState.h"
#include "LXActorCamera.h"
#include "LXBackground.h"
#include "LXProject.h"
#include "LXPropertyManager.h"
#include "LXAnimationManager.h"
#include "LXProject.h"
#include "LXRenderer.h"
#include "LXActorLight.h"
#include "LXAnimation.h"
#include "LXMSXMLNode.h"
#include "LXRenderPipeline.h"
#include "LXEngine.h"

LXViewState::LXViewState(LXProject* pDocument):
_Project(pDocument),
m_bLighting(true),
m_bSSAO(false),
m_fSSAOPower(1.0f),
m_bShadows(true),
m_bDepthOfField(false),
m_bGround(true),
m_bMirror(true),
// m_fNear(0.1f),
// m_fFar(1000.0f),
m_bSilhouette(false),
m_fSSAORadius(-1.0f), // Undefined TODO
m_bSSAOSmooth(true),
m_fHour(14.f),
m_bFXAA(false),
m_bLayered(true)
{
//	_renderingLayer0.SetName(L"Rendering Layer 0"); 
//	_renderingLayer0.SetUID(LX_UID_RENDERINGLAYER_0);

//	_renderingLayer1.SetName(L"Rendering Layer 1");
//	_renderingLayer1.SetUID(LX_UID_RENDERINGLAYER_1);

	//AddSmartObject(&_renderingLayer0);
	//AddSmartObject(&_renderingLayer1);

	DefineProperties();
}

/*virtual*/
LXViewState::~LXViewState(void)
{
}

/*virtual*/
void LXViewState::DefineProperties( )
{
	
	//--------------------------------------------------------------------------------------------------------------
	//LXProperty::SetCurrentGroup(L"Misc");
	//--------------------------------------------------------------------------------------------------------------
	
	// Near Far
	//DefinePropertyFloat(L"Near", LXPropertyID::Near, &m_fNear);
	//DefinePropertyFloat(L"Far", LXPropertyID::Far, &m_fFar);
	auto p = DefinePropertyFloat(L"Hour", LXPropertyID::HOUR, &m_fHour);
	p->SetMinMax(0.f, 23.f);
	{
		auto p = DefinePropertyBool(L"UseBlitFunction", (LXPropertyID)5454, &m_bUseBlitFunction);
		p->SetPersistent(false);
	}
	
	//--------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Ground");
	//--------------------------------------------------------------------------------------------------------------

	// Ground
	LXPropertyBool* pPropGround = DefinePropertyBool(L"Ground", LXPropertyID::GROUND, &m_bGround);

	// Mirror
	DefinePropertyBool( L"Mirror", LXPropertyID::MIRROR, &m_bMirror);
		
	//--------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Lighting");
	//--------------------------------------------------------------------------------------------------------------
		
	// Lighting
	DefinePropertyBool( L"Lighting", LXPropertyID::LIGHTING, &m_bLighting);

	// DeferredLighting
	DefinePropertyBool(L"DeferredLighting", LXPropertyID::DEFERREDLIGHTING, &_bDeferredLighting);

	// Dynamic light
	//DefinePropertyBool( L"DynamicLight", LXPropertyID::DYNAMICLIGHT, &m_bDynamicLight);

	// Shadows
	DefinePropertyBool( L"Shadows", LXPropertyID::SHADOWS, &m_bShadows);

	//--------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Ambient Occlusion");
	//--------------------------------------------------------------------------------------------------------------
	
	// SSAO
	DefinePropertyBool( L"AmbientOcclusion", LXPropertyID::SSOA, &m_bSSAO);

	// SSAO Radius
	LXPropertyFloat* pSSAORadius = DefinePropertyFloat( L"AmbientOcclusionRadius", LXPropertyID::SSAO_RADIUS, &m_fSSAORadius);
	pSSAORadius->SetMin(0.001f);
	
	// SSAO Power
	LXPropertyFloat* pSSAOPower = DefinePropertyFloat(L"AmbientOcclusionPower", LXPropertyID::SSAO_POWER, &m_fSSAOPower);
	pSSAOPower->SetMin(1.f);
	
	// SSAO Smooth
	DefinePropertyBool( L"AmbientOcclusionSmooth", LXPropertyID::SSOA_SMOOTH, &m_bSSAOSmooth);

	//--------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Glow/Bloom");
	//--------------------------------------------------------------------------------------------------------------

	// Enable/Disable Glow
	DefinePropertyBool(L"HighLightGlow", LXPropertyID::GLOW, &m_bGlow);

	// Strength
	{
		auto p = DefinePropertyFloat(L"Strength", GetAutomaticPropertyID(), &m_fGlowFactor);
		//p->SetMinMax(0.f, 8.f);
	}
	
	// DownSample factor
	{
		LXPropertyEnum* p = DefinePropertyEnum(L"DownSampleFactor", GetAutomaticPropertyID(), &_nDownSampleFactor);
		p->AddChoice(L"x2", 2);
		p->AddChoice(L"x4", 4);
		p->AddChoice(L"x8", 8);
		p->AddChoice(L"x16", 16);
	}

	// Blur pass count
	{
		LXPropertyEnum* p = DefinePropertyEnum(L"BlurPassCount", GetAutomaticPropertyID(), &_nBlurPassCount);
		p->AddChoice(L"1", 1);
		p->AddChoice(L"2", 2);
		p->AddChoice(L"3", 3);
		p->AddChoice(L"4", 4);
	}

	//--------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Additional Effects");
	//--------------------------------------------------------------------------------------------------------------

	// Silhouette
	DefinePropertyBool(L"Silhouette", LXPropertyID::VIEWSTATE_SILHOUETTE, &m_bSilhouette);

	// Depth Of Field
	DefinePropertyBool(L"DepthOfField", LXPropertyID::VIEWSTATE_DEPTHOFFIELD, &m_bDepthOfField);

	// FXAA
	DefinePropertyBool(L"FXAA", LXPropertyID::FXAA, &m_bFXAA);

	// Exposure
	auto pPropExposure = DefinePropertyFloat(L"Exposure", LXPropertyID::VIEWSTATE_EXPOSURE, &_fExposure);
	pPropExposure->SetMin(0.f);
	//pPropExposure->SetMinMax(0.f, 4.f);

	// Gamma
	//auto pPropGamma = DefinePropertyFloat(L"Gamma", LXPropertyID::VIEWSTATE_GAMMA, &_fGamma);
	//pPropGamma->SetMin(0.f);

	// Brightness 
	auto pPropBrightness = DefinePropertyFloat(L"Brightness", LXPropertyID::VIEWSTATE_BRIGHTNESS, &_fBrightness);
	pPropBrightness->SetMinMax(0.f, 1.f);
	pPropBrightness->SetAnimatable(true);

	// Texture
	LXPropertyAsset::Create(this, L"Texture", (LXPropertyID)554, (LXAssetPtr*)&_texture);
	
	
	//--------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Layers");
	//--------------------------------------------------------------------------------------------------------------

	// Layered
	DefinePropertyBool(L"Layered", LXPropertyID::VIEWSTATE_LAYERED, &m_bLayered);

	// Layer Transition
	LXPropertyEnum* pPropTransition = DefinePropertyEnum(L"Transition", L"Transition", LXPropertyID::LAYERTRANSITION, (uint*)&m_eLayerTransition);
	pPropTransition->AddChoice(L"Opacity", LX_VIEWSTATE_LAYERTRANSITION_OPACITY);
	pPropTransition->AddChoice(L"Deepness", LX_VIEWSTATE_LAYERTRANSITION_DEEPNESS);

	DefinePropertyFloat(L"Opacity", LXPropertyID::VIEWSTATE_LAYEROPACITY, &m_fLayerOpacity, 0.f, 1.f);
	DefinePropertyFloat(L"Deepness", LXPropertyID::VIEWSTATE_LAYERDEEPNESS, &m_fLayerDeepness);

	DefinePropertyBool(L"UseMask", LXPropertyID::VIEWSTATE_LAYERUSEMASK, &m_bLayerUseMask);

	//--------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Advanced");
	//--------------------------------------------------------------------------------------------------------------

	// Distribution function
	{
		LXPropertyEnum* prop = DefinePropertyEnum(L"Distributionfunction", GetAutomaticPropertyID(), (uint*)&_disributionFunc);
		prop->AddChoice(L"Beckmann", Beckmann);
		prop->AddChoice(L"Gaussian V1", GaussianV1); 
		prop->AddChoice(L"Gaussian V2", GaussianV2);
		prop->AddChoice(L"GGX", GGX);
		prop->ValueChanged.AttachMemberLambda([this](LXProperty* property)
		{
			
		});
	}

	//  Buffer to Visualize
	{
		LXPropertyEnum* PropBufferToVisualize = DefinePropertyEnum(L"ShowBuffer", GetAutomaticPropertyID(), &_BufferToVisualize);
		// Choices are dynamics
		PropBufferToVisualize->SetBuildChoicesFunc([](LXPropertyEnum* Property)
		{
			const TDebuggableTextures& textures = GetRenderer()->GetRenderPipeline()->GetDebugTextures();
			Property->AddChoice(L"Default", 0);
			uint i = 1;
			for (auto It : textures)
			{
				Property->AddChoice(It.Name.GetBuffer(), i++);
			}
		});
	}
}

/*virtual*/ 
bool LXViewState::OnSaveChild(const TSaveContext& saveContext) const
{
// 	for (LXSmartObject* pSmartObject : m_arraySmartObject)
// 	{
// 		pSmartObject->Save(saveContext);
// 	}

	return true;
}

/*virtual*/
bool LXViewState::OnLoadChild ( const TLoadContext& loadContext )
{
// 	if (loadContext.node.name() == _renderingLayer0.GetObjectName())
// 	{
// 		if (loadContext.node.attr(L"UID") == LX_UID_RENDERINGLAYER_0)
// 		{
// 			_renderingLayer0.Load(loadContext);
// 		}
// 		else if (loadContext.node.attr(L"UID") == LX_UID_RENDERINGLAYER_1)
// 		{
// 			_renderingLayer1.Load(loadContext);
// 		}
// 		else
// 		{
// 			CHK(0);
// 			return false;
// 		}
// 
// 		return true;
// 	}
//	else
	{
		return false;
	}
}

void LXViewState::ZoomOnBBoxAnimated(const LXBBox& box, bool bAnimated)
{
	CHK(box.IsValid());
	if (!box.IsValid())
		return;

	CHK(_Project);
	if (!_Project)
		return;

	LXActorCamera* ActorCamera = GetProject()->GetCamera();
	
	vec3f vDir = ActorCamera->GetViewVector();
	
	vec3f vNewTarget;
	vNewTarget.Set(box.GetCenter().x, box.GetCenter().y, box.GetCenter().z);
	vec3f vNewPosition = vNewTarget - vDir * box.GetDiag() * (90.0f / ActorCamera->GetFov()) * 0.5f;
	float fNewHeight = box.GetDiag() * 1.5f;

	if (bAnimated)
	{
		LXProperty* pPropPosition = ActorCamera->GetProperty(LXPropertyID::POSITION);
		LXProperty* pPropTarget = ActorCamera->GetProperty(LXPropertyID::CAMERA_TARGET);
		LXProperty* pPropHeight = ActorCamera->GetProperty(LXPropertyID::CAMERA_HEIGHT);

		CHK(pPropPosition && pPropTarget && pPropHeight);

		LXAnimation* animation = new LXAnimation;
		animation->SetDuration(LX_ZOOMONBBOX_TIME);
		animation->AddKey(pPropPosition, vNewPosition, LX_ZOOMONBBOX_TIME);
		animation->AddKey(pPropTarget, vNewTarget, LX_ZOOMONBBOX_TIME);
		//animation->AddKey((LXPropertyFloat)pPropHeight, fNewHeight, LX_ZOOMONBBOX_TIME);
		_Project->GetAnimationManager().PlayVolatileAnimation(animation);
	}
	else
	{
		CHK(IsValid(vNewPosition));
		CHK(IsValid(vNewTarget));
		ActorCamera->SetPosition(vNewPosition);
		ActorCamera->SetTarget(vNewTarget);
		ActorCamera->SetHeight(fNewHeight);
	}
}

LXRenderingLayer::LXRenderingLayer()
{
	//m_bSaveUIDInObjectTag = true;
	DefineProperties();
}

