//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXActor.h"
#include "LXActorFactory.h"
#include "LXComponent.h"
#include "LXCore.h"
#include "LXMSXMLNode.h"
#include "LXMath.h"
#include "LXRenderData.h"
#include "LXRenderer.h"
#include "LXScene.h"

LXActor::LXActor()
{
	LX_COUNTSCOPEINC(LXActor);
	_Project = GetCore().GetProject();
	_nCID = LX_NODETYPE_ACTOR;
	DefineProperties();
}

LXActor::LXActor(LXProject* pDocument):
_Project(pDocument)
{
	LX_COUNTSCOPEINC(LXActor);
	_nCID = LX_NODETYPE_ACTOR;
	DefineProperties();
}

LXActor::~LXActor(void)
{
	LX_COUNTSCOPEDEC(LXActor);

	for (LXActor* child : GetChildren())
		delete child;

	ReleaseRenderData(ERenderClusterRole::All);

	for (LXComponent* component : _components)
	{
		delete component;
	}
}

void LXActor::MarkForDelete()
{
	CHK(GetParent() == nullptr);
	CHK(GetChildren().size() == 0);
	GetCore().AddObjectForDestruction(this);
}

void LXActor::AddComponent(LXComponent* component)
{
	_components.push_back(component);
	component->InvalidateRenderState();
}

void LXActor::DefineProperties() 
{
	// --------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Misc");
	// --------------------------------------------------------------------------------------------------------------

	// Visibility
	DefinePropertyBool(L"Visible", LXPropertyID::VISIBILITY, &_bVisible);

	// --------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Bounds");
	// --------------------------------------------------------------------------------------------------------------

	LXPropertyVec3f* pPropSizeWorld = DefinePropertyVec3f(L"Size", GetAutomaticPropertyID(), nullptr);
	pPropSizeWorld->SetLambdaOnGet([this]()
	{
		return GetBBoxWorld().GetSize();
	});
	pPropSizeWorld->SetPersistent(false);
	pPropSizeWorld->SetReadOnly(true);

	LXPropertyVec3f* pPropSizeCenter = DefinePropertyVec3f(L"Center", GetAutomaticPropertyID(), nullptr);
	pPropSizeCenter->SetLambdaOnGet([this]()
	{
		return GetBBoxWorld().GetCenter();
	});
	pPropSizeCenter->SetPersistent(false);
	pPropSizeCenter->SetReadOnly(true);

	// --------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Transformation");
	// --------------------------------------------------------------------------------------------------------------
	
	_Transformation.DefineProperties(this);
	_Transformation.OnChange.AttachMemberLambda([this]()
	{
		InvalidateMatrixWCS();
	});

	// --------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Actor");
	// --------------------------------------------------------------------------------------------------------------
	{
		auto p = DefinePropertyBool(L"Highlightable", LXPropertyID::Hilightable, &_bHighlightable);
		p = DefinePropertyBool(L"Pickable", LXPropertyID::Pickable, &_bPickable);
		p = DefinePropertyBool(L"DrawSelection", LXPropertyID::DrawSelection, &_bDrawSelection);
	}
}

void LXActor::InvalidateRenderState(LXFlagsRenderClusterRole renderStates)
{
	if (_isLoading)
		return;

	CreateRenderData(renderStates);
}

void LXActor::CreateRenderData(LXFlagsRenderClusterRole renderStates)
{
	ReleaseRenderData(renderStates);

	if (GetRenderer() && GetParent())
	{
		_renderData = new LXRenderData(this);
		InvalidateMatrixWCS();
		InvalidateBounds();	
		if (_bVisible)
		{
			GetRenderer()->UpdateActor(_renderData, renderStates);
		}
	}
}

void LXActor::ReleaseRenderData(LXFlagsRenderClusterRole renderStates)
{
	if (_renderData && GetRenderer())
	{
		GetRenderer()->ReleaseRenderData(_renderData, renderStates);
	}
}

#if LX_ANCHOR

void LXActor::AddAnchor(LXAnchor* pAnchor)
{
	//CHK(pAnchor);
	if (pAnchor)
	{
		_ArrayAnchors.push_back(pAnchor);
		pAnchor->SetOwner(this);
	}
}

void LXActor::InsertAnchor(LXAnchor* pAnchor, int position)
{
	//CHK(pAnchor);
	if (pAnchor)
	{
		ArrayAnchors::iterator it = _ArrayAnchors.begin();
		_ArrayAnchors.insert(it + position, pAnchor);
	}
}

LXAnchor* LXActor::GetAnchor(uint position) const 
{
	//CHK(position < m_arrayAnchors.size());
	if (position < _ArrayAnchors.size())
		return _ArrayAnchors.at(position);
	else
		return NULL;
}

#endif

const LXMatrix& LXActor::GetMatrixWCS() const
{
	if (!_bValidMatrixWCS)
		const_cast<LXActor*>(this)->ComputeMatrixWCS();

	return _MatrixWCS;
}

void LXActor::ComputeMatrixWCS()
{
	if (_bValidMatrixWCS)
		return;

	_MatrixWCS = GetParent() ? GetParent()->GetMatrixWCS() * _Transformation.GetMatrix() : _Transformation.GetMatrix();
	_bValidMatrixWCS = true;

	if (_renderData)
	{
		_renderData->ComputePrimitiveWorldMatrices();
	}
}

void LXActor::SetMatrixWCS(const LXMatrix& matrix, bool ComputeLocalMatrix /*= false*/)
{
	_MatrixWCS = matrix; 
	_bValidMatrixWCS = true;
 
	if (ComputeLocalMatrix)
	{
		const LXMatrix& MatrixWCSParent = GetParent()->GetMatrixWCS();
		LXMatrix MatrixLocal = matrix;
		MatrixWCSParent.ParentToLocal(MatrixLocal);
		_Transformation.SetTranslation(MatrixLocal.GetOrigin());
		_Transformation.SetRotation(MatrixLocal.GetEulerAngles());
		_Transformation.SetScale(MatrixLocal.GetScale());
	}
}

void LXActor::InvalidateMatrixWCS()
{
	if (_isLoading)
		return;

	// Invalidate Matrices & Bounds
	InvalidateBranchMatrixWCS();

	// Recompute
	ComputeMatrixWCS();
	ComputeBBoxWorld();
}
	

void LXActor::InvalidateBranchMatrixWCS()
{
	// Invalidates the World Matrix recursively (this + children)
	for (LXActor* Actor : _children)
		Actor->InvalidateBranchMatrixWCS();

	// Currently used by ActorMesh to invalidate the primitive WorldMatrix.
	OnInvalidateMatrixWCS();

	// By the way, Invalidates the World Bounds recursively (this + parent)
	InvalidateBranchWorldBounds(true);

	_bValidMatrixWCS = false;
}

void LXActor::SetPosition(const vec3f& Translation)
{
	if (!IsNearlyEqual(Translation, _Transformation.GetTranslation()))
	{
		_Transformation.SetTranslation(Translation);
		InvalidateMatrixWCS();
		OnPositionChanged();
	}
}

void LXActor::SetRotation(const vec3f& Rotation)
{
	if (!IsNearlyEqual(Rotation, _Transformation.GetRotation()))
	{
		_Transformation.SetRotation(Rotation);
		InvalidateMatrixWCS();
		OnRotationChanged();
	}
}

void LXActor::SetScale(const vec3f& Scale)
{
	if (!IsNearlyEqual(Scale, _Transformation.GetScale()))
	{
		_Transformation.SetScale(Scale);
		InvalidateMatrixWCS();
		OnScaleChanged();
	}
}

LXBBox& LXActor::GetBBoxLocal()
{
	if (!_BBoxLocal.IsValid())
		ComputeBBoxLocal();

	return _BBoxLocal;
}

LXBBox& LXActor::GetBBoxWorld()
{
	if (!_BBoxWorld.IsValid())
		ComputeBBoxWorld();

	return _BBoxWorld;
}

void LXActor::InvalidateBounds(bool bPropagateToParent)
{
	if (_isLoading)
		return;

	_BBoxLocal.Invalidate();
	InvalidateWorldBounds(true);
}

void LXActor::InvalidateWorldBounds(bool bPropagateToParent)
{
	if (_isLoading)
		return;

	// Invalidate Bounds
	InvalidateBranchWorldBounds(bPropagateToParent);

	// Recompute
	ComputeBBoxWorld();
}

void LXActor::InvalidateBranchWorldBounds(bool bPropagateToParent)
{
	_BBoxWorld.Invalidate();
	if (bPropagateToParent && _parent)
		_parent->InvalidateBranchWorldBounds(true);
}

void LXActor::ComputeBBoxLocal()
{
	if (_BBoxLocal.IsValid())
		return;

	_BBoxLocal.Reset();

	for (const LXComponent* component : _components)
	{
		_BBoxLocal.Add(component->GetBBoxLocal());
	}

	if (!_BBoxLocal.IsValid())
	{
		LogD(Actor, L"Set the default BBox size for %s", GetName().GetBuffer());
		_BBoxLocal.Add(LX_VEC3F_XYZ_50);
		_BBoxLocal.Add(LX_VEC3F_NXYZ_50);
	}
}

void LXActor::ComputeBBoxWorld()
{
	if (_BBoxWorld.IsValid())
		return;
	
	_BBoxWorld = GetBBoxLocal();

	for (LXActor* Child : _children)
	{
		if (Child->ParticipateToSceneBBox())
		{
			_BBoxWorld.Add(Child->GetBBoxWorld());
		}
	}

	for (LXComponent* component : _components)
	{
		_BBoxWorld.Add(component->GetBBoxWorld());
	}
	
	GetMatrixWCS().LocalToParent(_BBoxWorld);

	if (_renderData)
	{
		_renderData->ComputePrimitiveWorldBounds();

		LXBBox box;
		for (const LXWorldPrimitive* worldPrimitive : _renderData->GetPrimitives())
		{
			CHK(worldPrimitive->BBoxWorld.IsValid());
			box.Add(worldPrimitive->BBoxWorld);
		}
		_BBoxWorld.Add(box);
	}
}

void LXActor::ComputePrimitiveWorldMatrices()
{
	if (_renderData)
	{
		_renderData->ComputePrimitiveWorldMatrices();
	}
}

void LXActor::AddChild(LXActor* Actor)
{
	__super::AddChild(Actor);
	InvalidateWorldBounds(true);
	Actor->InvalidateRenderState();
	GetScene()->OnActorAdded(Actor);
}

void LXActor::RemoveChild(LXActor* Actor)
{
	__super::RemoveChild(Actor);
	Actor->InvalidateRenderState();
	InvalidateWorldBounds(true);
	GetScene()->OnActorRemoved(Actor);
}

bool LXActor::OnSaveChild(const TSaveContext& saveContext) const
{
	for (LXActor* child : _children)
	{
		child->Save(saveContext);
	}
	return true;
}

bool LXActor::OnLoadChild(const TLoadContext& loadContext)
{
	const LXString& name = loadContext.node.name();
	bool bRet = false;
	
	if (LXActor* Actor = GetActorFactory()->CreateActor(name))
	{
		Actor->Load(loadContext);
		AddChild(Actor);
		bRet = true;
	}
	
	return bRet;
}

void LXActor::SetVisible(bool bVisible)
{
	if (_bVisible != bVisible)
	{
		_bVisible = bVisible;

		if (!(GetCID() & LX_NODETYPE_CAMERA))
			InvalidateBounds(true);

		InvalidateRenderState();
	}
}

void LXActor::SetBBoxVisible(bool visible)
{
	if (_bBBoxVisible != visible)
	{
		_bBBoxVisible = visible;
		InvalidateRenderState(LXFlagsRenderClusterRole(ERenderClusterRole::Default) | LXFlagsRenderClusterRole(ERenderClusterRole::ActorBBox));
	}
}

void LXActor::SetPrimitiveBBoxVisible(bool visible)
{
	if (_bPrimitiveBBoxVisible != visible)
	{
		_bPrimitiveBBoxVisible = visible;
		InvalidateRenderState(LXFlagsRenderClusterRole(ERenderClusterRole::Default) | LXFlagsRenderClusterRole(ERenderClusterRole::PrimitiveBBox));
	}

	for (LXComponent* component : _components)
	{
		component->SetPrimitiveBBoxVisible(visible);
	}
}

void LXActor::Run(double frameTime)
{
	for (LXActor* Child : _children)
	{
		Child->Run(frameTime);
	}

	for (LXComponent* component : _components)
	{
		component->Run(frameTime);
	}
}

