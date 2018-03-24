//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXTraverserFrustumCulling.h"
#include "LXFrustum.h"
#include "LXActorMesh.h"
#include "LXMemory.h" // --- Must be the last included ---

LXTraverserFrustumCulling::LXTraverserFrustumCulling()
{
}

LXTraverserFrustumCulling::~LXTraverserFrustumCulling()
{
}

void LXTraverserFrustumCulling::OnActor(LXActor* pGroup)
{
	CHK(_Frustum);

	if (_Frustum && dynamic_cast<LXActorMesh*>(pGroup))
	{
		const LXMatrix& mat = pGroup->GetMatrixWCS();
		const LXBBox& b = pGroup->GetBBoxLocal();
		vec3f vCenter = b.GetCenter();
		mat.LocalToParentPoint(vCenter);
		if (!_Frustum->IsSphereIn(vCenter, b.GetDiag() / 2.0f))
		{
			return;
		}
	}

	LXTraverser::OnActor(pGroup);
}