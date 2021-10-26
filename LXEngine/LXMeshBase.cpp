//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXMeshBase.h"

//Seetron
#include "LXMatrix.h"
#include "LXPrimitive.h"
#include "LXPrimitiveInstance.h"

const LXBBox& LXMeshBase::GetBounds()
{
	if (!_LocalBounds.IsValid())
		ComputeBounds();

	return _LocalBounds;
}

LXMesh2::LXMesh2()
{
}

void LXMesh2::ComputeBounds()
{
	if (_LocalBounds.IsValid())
		return;

	// Reset the BBox
	_LocalBounds.Reset();

	// Extend with the LOD0 PrimitiveInstances
	const VectorPrimitiveInstances& primitiveInstances = _primitiveInstances[0];

	for (const std::shared_ptr<LXPrimitiveInstance>& PrimitiveInstance : primitiveInstances)
	{
		LXPrimitive* pPrimitive = PrimitiveInstance->Primitive.get();

		if (PrimitiveInstance->Matrix)
		{
			LXBBox BBoxPrimitiveInstance = PrimitiveInstance->Primitive->GetBBoxLocal();
			PrimitiveInstance->Matrix->LocalToParent(BBoxPrimitiveInstance);
			_LocalBounds.Add(BBoxPrimitiveInstance);
		}
		else
		{
			_LocalBounds.Add(pPrimitive->GetBBoxLocal());
		}
	}

	if (!_LocalBounds.IsValid())
	{
		CHK(0);
		// We cannot set arbitrary values (to create a default box of 1m)
		// Because the scale matrix could modify it too much.
		_LocalBounds.Add(LX_VEC3F_NULL);
	}
}

void LXMesh2::AddPrimitive(const std::shared_ptr<LXPrimitive>& primitive, const LXMatrix* matrix /*= nullptr*/, const std::shared_ptr<LXMaterialBase>& material /*= nullptr*/, int LODIndex /*= 0*/)
{
	_primitiveInstances[LODIndex].push_back(std::make_unique<LXPrimitiveInstance>(primitive, matrix, material));
	_LocalBounds.Invalidate();
}

void LXMesh2::GetAllPrimitives(VectorPrimitiveInstances& outPrimitiveInstances, int LODIndex /*= 0*/)
{
	const VectorPrimitiveInstances& primitiveInstances = _primitiveInstances[LODIndex];

	for (const std::shared_ptr<LXPrimitiveInstance>& it : primitiveInstances)
	{
		// LXMesh2 is "flat" (no child or parent), so no need to compote the MatrixRCS.
		it->MatrixRCS = new LXMatrix(*it->Matrix);
		outPrimitiveInstances.push_back(it);
	}
}
