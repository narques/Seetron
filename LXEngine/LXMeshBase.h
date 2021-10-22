//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXSmartObject.h"

// Seetron
#include "LXBBox.h"

class LXMaterialBase;
class LXMeshData;
class LXPrimitive;
class LXPrimitiveInstance;

typedef std::vector <std::shared_ptr<LXPrimitiveInstance>> VectorPrimitiveInstances;

class LXENGINE_API LXMeshBase : public LXSmartObject
{
public:

	const LXBBox& GetBounds();

	virtual void ComputeBounds() = 0 ;

	virtual void AddPrimitive(const std::shared_ptr<LXPrimitive>& primitive, const LXMatrix* matrix, const std::shared_ptr<LXMaterialBase>& material, int LODIndex = 0) = 0;

	virtual void GetAllPrimitives(VectorPrimitiveInstances& primitiveInstances, int LODIndex = 0) = 0;

protected:

	LXBBox _LocalBounds;
};

class LXMesh2 : public LXMeshBase
{
public:

	LXMesh2();

	virtual void ComputeBounds()override;

	virtual void AddPrimitive(const std::shared_ptr<LXPrimitive>& primitive, const LXMatrix* matrix, const std::shared_ptr<LXMaterialBase>& material, int LODIndex = 0) override;
		
	virtual void GetAllPrimitives(VectorPrimitiveInstances& primitiveInstances, int LODIndex = 0) override;

private:

	VectorPrimitiveInstances _primitiveInstances[LX_MAX_LODS];

};
