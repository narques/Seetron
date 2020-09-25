//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXComponent.h"
#include "LXRenderData.h"

class LXAssetMesh;
class LXMaterialBase;
class LXMeshBase;
class LXRenderCluster;
class LXPrimitive;

class LXCORE_API LXComponentMesh : public LXComponent
{

public:

	LXComponentMesh(LXActor* actor);
	virtual ~LXComponentMesh();

	static int GetCIDBit() { return LX_NODETYPE_MESH; }

	// Overridden from LXSmartObject
	void OnPropertyChanged(LXProperty* property) override;

	// Retrieve all the primitives with their corresponding world matrix
	const TWorldPrimitives& GetAllPrimitives();

	// Add a primitive to mesh
	void AddPrimitive(const shared_ptr<LXPrimitive>& primitive, const LXMatrix* matrix = nullptr, const shared_ptr<LXMaterialBase>& material = nullptr, int LODIndex = 0);
	void ReleaseAllPrimitives();

	// LOD Support
	void SetLODData(int LODIndex, float maxDistance);
	const float* GetLODData() const { return &_LODMaxDistance[0]; }

	// Misc
	void SetMesh(shared_ptr<LXMeshBase>& mesh);
	shared_ptr<LXMeshBase>& GetMesh() { return _mesh; }
	const shared_ptr<LXMeshBase>& Get() const { return _mesh; }
	void SetAssetMesh(shared_ptr<LXAssetMesh>& assetMesh);
	void InvalidateWorldPrimitives();
	virtual bool GetCastShadows() const { return _castShadows; }

	// Delegates & Events
	LXDelegate<LXComponentMesh*, vec3f&, vec3f&, vec3f&> UpdateWorldPositionForPicking;

private:

	// Construction
	void DefineProperties();

	// Bounds
	virtual void ComputeBBoxLocal() override;
	void UpdateAssetMeshCallbacks();
	void UpdateMesh();

protected:

	shared_ptr<LXMeshBase> _mesh;
	shared_ptr<LXAssetMesh> _assetMesh;
	bool _castShadows = true;
	float _LODMaxDistance[LX_MAX_LODS] = {};
};
