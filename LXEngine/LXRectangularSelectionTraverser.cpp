//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXRectangularSelectionTraverser.h"
#include "LXActor.h"
#include "LXActorMesh.h"
#include "LXFrustum.h"
#include "LXPrimitive.h"
#include "LXPrimitiveInstance.h"
#include "LXMemory.h" // --- Must be the last included ---

LXRectangularSelectionTraverser::LXRectangularSelectionTraverser()
{
}

LXRectangularSelectionTraverser::~LXRectangularSelectionTraverser()
{
}

void LXRectangularSelectionTraverser::Apply( )
{
	_setActors.clear();
	LXTraverserFrustumCulling::Apply();
}

void LXRectangularSelectionTraverser::OnActor(LXActor* pGroup)
{
	if (!pGroup->IsVisible())
		return;

	if (!pGroup->IsPickable())
		return;

	CHK(_Frustum);

	if (_Frustum && dynamic_cast<LXActorMesh*>(pGroup))
	{
		const LXMatrix& mat = pGroup->GetMatrixWCS();
		const LXBBox& b = pGroup->GetBBoxLocal();

		if (b.IsValid())
		{

			vec3f vCenter = b.GetCenter();
			mat.LocalToParentPoint(vCenter);

			EFrustumTestResult ftr;
			if (!_Frustum->IsSphereIn(vCenter, b.GetDiag() * 0.5f, ftr))
				return;

			if ((pGroup->GetCID() & LX_NODETYPE_MESH) && (ftr == EFrustumTestResult::Inside))
			{
				_setActors.insert((LXActorMesh*)pGroup);
				return;
			}
		}
		else
			return;
	}

	LXTraverser::OnActor(pGroup);
}

void LXRectangularSelectionTraverser::OnPrimitive(LXActorMesh* pMesh, LXComponentMesh* componentMesh, LXWorldPrimitive* WorldPrimitive)
{
	LXPrimitive* pPrimitive = WorldPrimitive->PrimitiveInstance->Primitive.get();

	ArrayVec3f& arrayPosition = pPrimitive->GetArrayPositions();

	for (uint i=0; i<arrayPosition.size(); i++)
	{
		vec3f v0 = arrayPosition[i];
		pMesh->GetMatrixWCS().LocalToParentPoint(v0);
		if (_Frustum->IsPointIn(v0.x, v0.y, v0.z))
		{
			_setActors.insert(pMesh);
			break;
		}
	}

	LXTraverserFrustumCulling::OnPrimitive(pMesh, componentMesh, WorldPrimitive);
}
