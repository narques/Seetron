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
class LXMesh;
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
	void AddPrimitive(const shared_ptr<LXPrimitive>& primitive, const LXMatrix* matrix = nullptr, const LXMaterialBase* material = nullptr);
	void ReleaseAllPrimitives();

	// Misc
	void SetMesh(shared_ptr<LXMesh>& mesh);
	shared_ptr<LXMesh>& GetMesh() { return _mesh; }
	const shared_ptr<LXMesh>& Get() const { return _mesh; }
	void SetAssetMesh(shared_ptr<LXAssetMesh>& assetMesh);
	void InvalidateWorldPrimitives();
	virtual bool GetCastShadows() const { return _castShadows; }

	// Delegates & Events
	LXDelegate<vec3f&, vec3f&, vec3f&> UpdateWorldPositionForPicking;

private:

	// Construction
	void DefineProperties();

	// Bounds
	virtual void ComputeBBoxLocal() override;
	void UpdateAssetMeshCallbacks();
	void UpdateMesh();

protected:

	shared_ptr<LXMesh> _mesh;
	shared_ptr<LXAssetMesh> _assetMesh;
	bool _castShadows = true;
};
