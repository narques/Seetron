//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXActor.h"

class LXAssetMesh;
class LXMesh;
class LXPrimitiveInstance;
class LXRenderCluster;

class LXWorldPrimitive
{

public:

	LXWorldPrimitive(LXPrimitiveInstance* InPrimitiveInstance, const LXMatrix& Matrix, LXBBox& BBox);
	~LXWorldPrimitive();
	void SetMaterial(LXMaterial* material);

	LXPrimitiveInstance* PrimitiveInstance;
	LXMatrix MatrixWorld;
	LXBBox BBoxWorld;

	// Rendering
	LXRenderCluster* RenderCluster = nullptr;
};

typedef vector<LXWorldPrimitive*> TWorldPrimitives;

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

	virtual void					ComputeBBox() override;
		
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

protected:

	void							GatherPrimitives();

private:

	virtual void					OnLoaded() override;
	void							UpdateAssetMeshCallbacks();
	void							UpdateMesh();
	void							OnInvalidateMatrixWCS() override;
	void							ComputePrimitiveWorldMatrices();
		
protected:

	shared_ptr<LXMesh> Mesh;
	LXAssetMesh* _AssetMesh = nullptr;

	// Additional size applied to the primitive BBoxWorlds ( in the LXWorldPrimitive structure )
	// Useful when displacement is applied to the primitive vertex, like a terrain patch.
	float _ExtendZ = 0.f;

	GetSetDef(int, _nLayer, Layer, 0);
	GetSetDef(bool, _showOutlines, ShowOutlines, false);
	GetSetDef(bool, _bCastShadows, CastShadows, true);

private:

	bool _bValidWorldPrimitiveMatrices = false;
	TWorldPrimitives _worldPrimitives;

	// Instances
	uint _InstanceCount = 0;
	ArrayVec3f _ArrayInstancePosition;
	
};

typedef list<LXActorMesh*> ListMeshs;
typedef set<LXActorMesh*> SetMeshs;
typedef vector<LXActorMesh*> ArrayMeshs;