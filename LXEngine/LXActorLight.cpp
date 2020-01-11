//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXActorLight.h"
#include "LXProject.h"
#include "LXMath.h"
#include "LXMeshFactory.h"
#include "LXMemory.h" // --- Must be the last included ---

LXActorLight::LXActorLight(LXProject* pDocument):
LXActorMesh(pDocument)
{
	_nCID |= LX_NODETYPE_LIGHT;
	Mesh = GetMeshFactory()->CreateCone(50.f, 100.f);

	SetName("Light");

	// ActorMesh
	SetCastShadow(false);
	
	_bLight = true;
	_eLightType = ELightType::Spot;
		
	/*
	LXPrimitive* Primitive = GetPrimitiveFactory()->CreateQuadXY(100.f, 100.f, true);
	Primitive->SetMaterial(L"Materials/M_UILight.smat");
	LXMaterial* Material = GetAssetManager()->GetMaterial(L"Materials/M_UILight.smat");
	Primitive->SetPersistent(false);
	Primitive->ComputeTangents();
	AddPrimitive(Primitive, nullptr, Material);
	*/
	
	// Billboarded icon
	//  	LXMaterial* pMaterialLight = m_pDocument->GetAssetManager().GetMaterial(L"sysLight");
	//  	LXPrimitive* pPrim = LXPrimitive::CreatePoint();
	//  	pPrim->SetPersistent(false);
	//  	pPrim->SetMaterial(pMaterialLight);
	//  	AddPrimitive(pPrim);

	// Position anchor
	// 	LXAnchor* pAnchor = new LXAnchor(m_pDocument);
	// 	pAnchor->SetPersistent(false);
	// 	AddAnchor(pAnchor);
	// 	AddChild(pAnchor);

	// Target Anchor
	//  	pAnchor = new LXAnchor(m_pDocument);
	//  	pAnchor->SetPersistent(false);
	//  	AddAnchor(pAnchor);
	//  	AddChild(pAnchor);

	// Primitives
	// 	LXPrimitiveDynamic* primitive = new LXPrimitiveDynamic();
	// 	primitive->SetPersistent(false);
	// 	primitive->SetDrawCallBack(this, (LXPrimitiveDynamic::LXDrawCallBack)(&LXLight::OnDrawPrimitive));
	// 	AddPrimitive(primitive);

	DefineProperties();
	pDocument->GetLights().push_back(this);
}

LXActorLight::~LXActorLight(void)
{
}

void LXActorLight::OnTrashed()
{
	CHK(_Project);
	if (_Project)
	{
		auto It = find(_Project->GetLights().begin(), _Project->GetLights().end(), this);
		if (It != _Project->GetLights().end())
		{
			_Project->GetLights().erase(It);
		}
		else
			CHK(0);
	}
}

void LXActorLight::OnRecycled()
{
	CHK(_Project);
	if (_Project)
		_Project->GetLights().push_back(this);
}

/*virtual*/
void LXActorLight::DefineProperties() 
{
	//--------------------------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Light");
	//--------------------------------------------------------------------------------------------------------------------------------
	
	DefinePropertyBool(L"Light", LXPropertyID::LIGHT, &_bLight);

	{
		auto p = DefinePropertyEnum(L"Type", LXPropertyID::LIGHT_TYPE, (uint*)&_eLightType);
		p->AddChoice(L"Spot", (uint)ELightType::Spot); 
		p->AddChoice(L"Directional", (uint)ELightType::Directional);
		p->AddChoice(L"Omni directional", (uint)ELightType::Omnidirectional);
	}

	DefinePropertyBool(L"CastShadow", LXPropertyID::LIGHT_CASTSHADOW, &_bCastShadow);
	DefinePropertyColor4f(L"Color", LXPropertyID::LIGHT_COLOR, &_color);

	{
		auto p = DefinePropertyFloat("Intensity", LXPropertyID::LIGHT_INTENSITY, &_intensity);
		p->SetMinMax(0.f, LX_MAX_INTENSITY);
	}



	DefineProperty("TargetDistance", &_fTargetDistance);
	auto pPropSpotAngle = DefinePropertyFloat("SpotAngle", GetAutomaticPropertyID(), &_fSpotAngle);
	pPropSpotAngle->SetMinMax(1.f, 179.f);
}

void LXActorLight::OnPropertyChanged(LXProperty* Property)
{
	__super::OnPropertyChanged(Property);

	if (Property && Property->GetName() == "TargetDistance")
	{
//		CHK(0);
// 		LXAnchor* pAnchorTarget = GetAnchor(1);
// 		if (pAnchorTarget)
// 		{
// 			LXMatrix mat;
// 			mat.SetOrigin(0.f, 0.f, m_fTargetDistance);
// 			pAnchorTarget->SetMatrix(mat);
// 		}
	}
}

#if LX_ANCHOR

void LXActorLight::OnAnchorMove(LXAnchor* pAnchor, const LXMatrix& lcs) 
{
	LXAnchor* pAnchorPosition = GetAnchor(0);
	LXAnchor* pAnchorTarget = GetAnchor(1);
 
	if (pAnchor == pAnchorPosition)
	{
		// Light matrix
// 		LXMatrix mat = GetMatrix() * lcs;
// 		SetMatrix(mat);
		CHK(0);
	}
	else if (pAnchor == pAnchorTarget)
	{
		// Light matrix
		vec3f direction = lcs.GetOrigin() - pAnchorPosition->GetPosition();
		float distance = direction.Length();
		direction.Normalize();
		LXMatrix matRt;
		matRt.SetVz(direction);
		LXMatrix mat = GetMatrix() * matRt;
		SetMatrix(mat);
		// Target distance
		LXMatrix target;
		target.SetOrigin(0.f, 0.f, distance);
		_fTargetDistance = distance;
		pAnchorTarget->SetMatrix(target);
	}
	else
		CHK(0);

	InvalidateBounds(true);
}

void LXActorLight::OnAnchorReleased(LXAnchor* pAnchors)
{
	LXPropertyMatrix* pProperty = dynamic_cast<LXPropertyMatrix*>(GetProperty(LXPropertyID::TRANSFORMATION));
	GetCore().GetCommandManager().ChangeProperty(pProperty);
}

#endif

void LXActorLight::UpdateTargetDistance(float fDistance)
{
#if LX_ANCHOR
	GetAnchor(1)->SetPosition(vec3f(0.f, 0.f, fDistance));
#endif
}

vec3f DirToEuler(const vec3f& x, const vec3f& y, const vec3f& z)
{
	vec3f Euler;
	float z_xy = sqrtf(z.x * z.x + z.y * z.y);
	if (z_xy > 0.005)
	{
		Euler.x = atan2(y.x * z.y - y.y * z.x, x.x * z.y - x.y * z.x);
		Euler.y = atan2(z_xy, z.z);
		Euler.z = -atan2(-z.x, z.y);
	}
	else 
	{
		Euler.x = 0.f;
		Euler.y = (z.z > 0.f) ? 0.f : LX_PI;
		Euler.z = -atan2(x.y, x.x);
	}
	return Euler;
}

void LXActorLight::SetDirection(const vec3f& vDirection)
{
	vec3f direction = vDirection;
	direction.Normalize();
	LXMatrix matRt;
	matRt.SetVz(direction);
// 	LXMatrix mat = GetMatrix() * matRt;
// 	SetMatrix(mat);
	vec3f Euler = DirToEuler(matRt.GetVx(), matRt.GetVy(), matRt.GetVz());
	SetPosition(Euler);
	
	// https://stackoverflow.com/questions/21622956/how-to-convert-direction-vector-to-euler-angles
	// https://stackoverflow.com/questions/1568568/how-to-convert-euler-angles-to-directional-vector
}

vec3f LXActorLight::GetDirectionWCS()
{
	return GetMatrixWCS().GetVz();
}