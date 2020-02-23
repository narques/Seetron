//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXActor.h"
#include "LXRenderData.h"

class LXAssetMesh;
class LXMesh;
class LXRenderCluster;

class LXCORE_API LXActorMesh : public LXActor
{

public:

	// Constructors - Destructor
	LXActorMesh();
	LXActorMesh(LXProject* pDocument);
	virtual ~LXActorMesh(void);
	void							DefineProperties();

	// Multi Instance support
	void							SetInstanceCount(uint Count);
	uint							GetInsanceCount() const { return _InstanceCount; }
	void							SetInstancePosition(uint i, const vec3f& Position);
	const ArrayVec3f&				GetArrayInstancePosition() const { return _ArrayInstancePosition; }
			
	// Misc
	virtual void					MarkForDelete() override;
	static int						GetCIDBit() { return LX_NODETYPE_MESH; }

	virtual void					ComputeBBoxLocal() override;
		
	// Overridden from LXSmartObject
	void							OnPropertyChanged( LXProperty* Property) override;
	
	//
	// Primitive
	//

	// Retrieve all the primitives with their corresponding world matrix
	const TWorldPrimitives&			GetAllPrimitives();

	// Add a primitive to the root mesh
	void							AddPrimitive(const shared_ptr<LXPrimitive>& Primitive, LXMatrix* Matrix = nullptr, LXMaterial* Material = nullptr);
	void							ReleaseAllPrimitives();
		
	// Misc
	void							SetMesh(shared_ptr<LXMesh>& Mesh);
	shared_ptr<LXMesh>&				GetMesh() { return Mesh; }
	void							SetAssetMesh(LXAssetMesh* AssetMesh);
	void							InvalidateWorldPrimitives();
	virtual bool					GetCastShadows() const override { return _bCastShadows; }

protected:

	// Bounds
	virtual	void					ComputeBBoxWorld() override;
	
private:

	//virtual void					OnLoaded() override;
	void							UpdateAssetMeshCallbacks();
	void							UpdateMesh();
	void							OnInvalidateMatrixWCS() override;
		
protected:

	shared_ptr<LXMesh> Mesh;
	LXAssetMesh* _AssetMesh = nullptr;
	bool _bCastShadows = true;

private:
		

	// Instances
	uint _InstanceCount = 0;
	ArrayVec3f _ArrayInstancePosition;
	
};

typedef list<LXActorMesh*> ListMeshs;
typedef set<LXActorMesh*> SetMeshs;
typedef vector<LXActorMesh*> ArrayMeshs;