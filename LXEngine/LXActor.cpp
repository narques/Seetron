//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXActor.h"
#include "LXActorFactory.h"
#include "LXAnchor.h"
#include "LXController.h"
#include "LXCore.h"
#include "LXMSXMLNode.h"
#include "LXMath.h"
#include "LXProject.h"
#include "LXScene.h"
#include "LXMemory.h" // --- Must be the last included ---

LXActor::LXActor()
{
	_Project = GetCore().GetProject();
	_nCID = LX_NODETYPE_ACTOR;
	DefineProperties();
}

LXActor::LXActor(LXProject* pDocument):
_Project(pDocument)
{
	_nCID = LX_NODETYPE_ACTOR;
	DefineProperties();
}

LXActor::~LXActor(void)
{
	for (auto It = _Children.begin(); It != _Children.end(); It++)
		delete (*It);
}

void LXActor::MarkForDelete()
{
	CHK(GetParent() == nullptr);
	CHK(_Children.size() == 0);
	GetController()->AddActorToDeleteSet(this);
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
	_Transformation.OnChange([this]()
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

void LXActor::InvalidateRenderState()
{
	if (GetParent() == nullptr)
		return;

	_RenderStateValid = false;
	GetController()->AddActorToUpdateRenderStateSet(this);
}

void LXActor::ValidateRensterState()
{
	_RenderStateValid = true;
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

const LXMatrix& LXActor::GetMatrixWCS()
{
	if (!_bValidMatrixWCS)
	{
		if (GetParent())
		{
			LXMatrix m = GetParent()->GetMatrixWCS() * _Transformation.GetMatrix();
			SetMatrixWCS(m);
		}
		else
			SetMatrixWCS(_Transformation.GetMatrix());
	}
	return _MatrixWCS;
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
	// Invalidates the World Matrix recursively (this + children)
	for (LXActor* Actor : _Children)
		Actor->InvalidateMatrixWCS();

	// Currently used by ActorMesh to invalidate the primitive WorldMatrix.
	OnInvalidateMatrixWCS();

	// By the way, Invalidates the World Bounds recursively (this + parent)
	InvalidateWorldBounds(true);

	_bValidMatrixWCS = false;

	// No need to update an object without parent.
	// It cannot exist in the Renderer
	if (GetParent())
	{
		GetController()->ActorWorldMatrixChanged(this);
	}
}

void LXActor::SetPosition(const vec3f& Translation)
{
	if (!IsEqual(Translation, _Transformation.GetTranslation()))
	{
		_Transformation.SetTranslation(Translation);
		InvalidateMatrixWCS();
		OnPositionChanged();
	}
}

void LXActor::SetRotation(const vec3f& Rotation)
{
	if (!IsEqual(Rotation, _Transformation.GetRotation()))
	{
		_Transformation.SetRotation(Rotation);
		InvalidateMatrixWCS();
		OnPositionChanged();
	}
}

void LXActor::SetScale(const vec3f& Scale)
{
	if (!IsEqual(Scale, _Transformation.GetScale()))
	{
		_Transformation.SetScale(Scale);
		InvalidateMatrixWCS();
		OnPositionChanged();
	}
}

LXBBox& LXActor::GetBBoxLocal()
{
	if (!_BBoxLocal.IsValid())
		ComputeBBox();

	if (!_BBoxLocal.IsValid())
	{
		LogD(Actor, L"Set the default BBox size for %s", GetName());
		_BBoxLocal.Add(LX_VEC3F_XYZ_50);
		_BBoxLocal.Add(LX_VEC3F_NXYZ_50);
	}

	return _BBoxLocal;
}

LXBBox& LXActor::GetBBoxWorld()
{
	if (!_BBoxWorld.IsValid())
	{
		_BBoxWorld = GetBBoxLocal();

		for (LXActor* Child : _Children)
		{
			if (Child->ParticipateToSceneBBox())
			{
				_BBoxWorld.Add(Child->GetBBoxWorld());
			}
		}

		GetMatrixWCS().LocalToParent(_BBoxWorld);
	}

	return _BBoxWorld;
}

void LXActor::InvalidateBounds(bool bPropagateToParent)
{
	_BBoxLocal.Invalidate();
	InvalidateWorldBounds(true);
}

void LXActor::InvalidateWorldBounds(bool bPropagateToParent)
{
	_BBoxWorld.Invalidate();
	if (bPropagateToParent && _Parent)
		_Parent->InvalidateWorldBounds(true);
}

void LXActor::ComputeBBox()
{
	if (_BBoxLocal.IsValid())
		return;

	_BBoxLocal.Reset();

	if (!_BBoxLocal.IsValid())
	{
		LogD(Actor, L"Set the default BBox size for %s", GetName().GetBuffer());
		_BBoxLocal.Add(LX_VEC3F_XYZ_50);
		_BBoxLocal.Add(LX_VEC3F_NXYZ_50);
	}
}

void LXActor::AddChild(LXActor* Actor)
{
	CHK(Actor);
	
	CHK(!Actor->GetParent());
	Actor->SetParent(this);

	_Children.push_back(Actor);
	InvalidateWorldBounds(true);
	Actor->InvalidateRenderState();

	GetScene()->OnActorAdded(Actor);
}

void LXActor::RemoveChild(LXActor* Actor)
{
	CHK(Actor);
	if (!Actor)
		return;

	CHK(Actor->GetParent());

	ListActors::iterator It = find(_Children.begin(), _Children.end(), Actor);
	if (It != _Children.end())
	{
		// InvalidateRenderState before
		Actor->InvalidateRenderState(); 
		_Children.erase(It);
		Actor->_LastKnownParent = Actor->_Parent;
		Actor->SetParent(nullptr);
		InvalidateWorldBounds(true);

		GetScene()->OnActorRemoved(Actor);
	}
	else
		CHK(0);
}

bool LXActor::OnSaveChild(const TSaveContext& saveContext) const
{
	for (ListActors::const_iterator It = _Children.begin(); It != _Children.end(); It++)
	{
		(*It)->Save(saveContext);
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
		InvalidateRenderState();

		if (!(GetCID() & LX_NODETYPE_CAMERA))
			InvalidateBounds(true);
	}
}

void LXActor::RunRT(double frameTime)
{
	for (LXActor* Child : _Children)
	{
		Child->RunRT(frameTime);
	}
}
