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
class LXMaterialBase;
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

		
	// Overridden from LXSmartObject
	void							OnPropertyChanged( LXProperty* Property) override;
	
	//
	// Primitive
	//

	// Retrieve all the primitives with their corresponding world matrix
	const TWorldPrimitives&			GetAllPrimitives();

	// Add a primitive to the root mesh
	void							AddPrimitive(const shared_ptr<LXPrimitive>& Primitive, const LXMatrix* Matrix = nullptr, const LXMaterialBase* Material = nullptr);
	void							ReleaseAllPrimitives();
		
	// Misc
	void							SetMesh(shared_ptr<LXMesh>& Mesh);
	shared_ptr<LXMesh>&				GetMesh() { return Mesh; }
	void							SetAssetMesh(shared_ptr<LXAssetMesh>& AssetMesh);
	void							InvalidateWorldPrimitives();
	virtual bool					GetCastShadows() const override { return _bCastShadows; }

	
private:

	// Bounds
	virtual void					ComputeBBoxLocal() override;
	
	//virtual void					OnLoaded() override;
	void							UpdateAssetMeshCallbacks();
	void							UpdateMesh();
			
protected:

	shared_ptr<LXMesh> Mesh;
	shared_ptr<LXAssetMesh> _AssetMesh;
	bool _bCastShadows = true;

private:
		

	// Instances
	uint _InstanceCount = 0;
	ArrayVec3f _ArrayInstancePosition;
	
};

typedef list<LXActorMesh*> ListMeshs;
typedef set<LXActorMesh*> SetMeshs;
typedef vector<LXActorMesh*> ArrayMeshs;