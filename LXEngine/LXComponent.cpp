//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXComponent.h"

// Seetron
#include "LXActor.h"
#include "LXCore.h"
#include "LXMath.h"
#include "LXLogger.h"
#include "LXRenderData.h"
#include "LXRenderer.h"
#include "LXStatistic.h"

LXComponent::LXComponent(LXActor* actor):
	_actor(actor)
{
	LX_COUNTSCOPEINC(LXComponent);
	_nCID = LX_NODETYPE_ACTOR;

	_actor->GetTransformation().OnChange.AttachMemberLambda([this]()
	{
		InvalidateMatrixWCS();
	});

	DefineProperties();
}

LXComponent::~LXComponent(void)
{
	LX_COUNTSCOPEDEC(LXComponent);
	ReleaseRenderData(ERenderClusterRole::All);
}

void LXComponent::MarkForDelete()
{
	GetCore().AddObjectForDestruction(this);
}

void LXComponent::DefineProperties()
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
	LXProperty::SetCurrentGroup(L"Component");
	// --------------------------------------------------------------------------------------------------------------
	{
		auto p = DefinePropertyBool(L"Highlightable", LXPropertyID::Hilightable, &_bHighlightable);
		p = DefinePropertyBool(L"Pickable", LXPropertyID::Pickable, &_bPickable);
		p = DefinePropertyBool(L"DrawSelection", LXPropertyID::DrawSelection, &_bDrawSelection);
	}
}

void LXComponent::InvalidateRenderState(LXFlagsRenderClusterRole renderStates)
{
	_renderStates = renderStates;
	if (_renderData)
		_renderData->Invalidate();
		
}

void LXComponent::CreateRenderData(LXFlagsRenderClusterRole renderStates)
{
	ReleaseRenderData(renderStates);

	if (GetRenderer())
	{
		_renderData = new LXRenderData(_actor, this);
		
		if (_bVisible)
		{
			GetRenderer()->UpdateActor(_renderData, renderStates);
		}
	}
}

void LXComponent::ReleaseRenderData(LXFlagsRenderClusterRole renderStates)
{
	if (_renderData && GetRenderer())
	{
		GetRenderer()->ReleaseRenderData(_renderData, renderStates);
	}
}

const LXMatrix& LXComponent::GetMatrixWCS()
{
	return _actor->GetMatrixWCS();
}

void LXComponent::ComputeMatrixWCS()
{
	if (_renderData)
	{
		_renderData->ComputePrimitiveWorldMatrices();
	}
}

void LXComponent::InvalidateMatrixWCS()
{
	if (_isLoading)
		return;

	// Invalidate Matrices & Bounds
	_BBoxWorld.Invalidate();

	// Recompute
	ComputeMatrixWCS();
	ComputeBBoxWorld();
}

const LXBBox& LXComponent::GetBBoxLocal() const
{
	if (!_BBoxLocal.IsValid())
		(const_cast<LXComponent*>(this))->ComputeBBoxLocal();

	return _BBoxLocal;
}

LXBBox& LXComponent::GetBBoxWorld()
{
	if (!_BBoxWorld.IsValid())
		ComputeBBoxWorld();
 
	return _BBoxWorld;
 }

void LXComponent::InvalidateBounds()
{
	_BBoxLocal.Invalidate();
	_BBoxWorld.Invalidate();
	_actor->InvalidateBounds(true);
}

void LXComponent::ComputeBBoxLocal()
{
	if (_BBoxLocal.IsValid())
		return;

	_BBoxLocal.Reset();

	if (!_BBoxLocal.IsValid())
	{
		LogD(Component, L"Set the default BBox size for %s", GetName().GetBuffer());
		_BBoxLocal.Add(LX_VEC3F_XYZ_50);
		_BBoxLocal.Add(LX_VEC3F_NXYZ_50);
	}
}

void LXComponent::ComputeBBoxWorld()
{
	if (_BBoxWorld.IsValid())
		return;

	_BBoxWorld = GetBBoxLocal();

	_actor->GetMatrixWCS().LocalToParent(_BBoxWorld);

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

void LXComponent::SetVisible(bool bVisible)
{
	if (_bVisible != bVisible)
	{
		_bVisible = bVisible;

		if (!(GetCID() & LX_NODETYPE_CAMERA))
			InvalidateBounds();

		InvalidateRenderState();
	}
}

void LXComponent::SetBBoxVisible(bool visible)
{
	if (_bBBoxVisible != visible)
	{
		_bBBoxVisible = visible;
		InvalidateRenderState(LXFlagsRenderClusterRole(ERenderClusterRole::Default) | LXFlagsRenderClusterRole(ERenderClusterRole::ActorBBox));
	}
}

void LXComponent::SetPrimitiveBBoxVisible(bool visible)
{
	if (_bPrimitiveBBoxVisible != visible)
	{
		_bPrimitiveBBoxVisible = visible;
		_renderData->ShowPrimitiveBBox(_bPrimitiveBBoxVisible);
	}
}

void LXComponent::Run(double frameTime)
{
	if (!_BBoxLocal.IsValid())
		ComputeBBoxLocal();
	
	if (!_BBoxWorld.IsValid())
		ComputeBBoxWorld();
		
	if (_renderData)
	{
		if (!_renderData->IsValid())
		{
			CreateRenderData(_renderStates);
			_renderData->Validate();
		}
	}
	else
	{
		CreateRenderData(_renderStates);
		_renderData->Validate();
	}

	if (!_renderData->AreValideWorldMatrices())
		_renderData->ComputePrimitiveWorldMatrices();

	if (!_renderData->AreValideWorldBounds())
		_renderData->ComputePrimitiveWorldBounds();
}

