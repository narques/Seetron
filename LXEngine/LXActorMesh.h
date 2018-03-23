//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXActor.h"

class LXAssetMesh;
class LXMesh;
class LXPrimitiveInstance;

struct LXWorldPrimitive
{
	LXWorldPrimitive(LXPrimitiveInstance* InPrimitiveInstance, const LXMatrix& Matrix, LXBBox& BBox):
		PrimitiveInstance(InPrimitiveInstance), 
		MatrixWorld(Matrix), 
		BBoxWorld(BBox)
	{
	}

	LXPrimitiveInstance* PrimitiveInstance;
	LXMatrix MatrixWorld;
	LXBBox BBoxWorld;
};

typedef vector<LXWorldPrimitive> TWorldPrimitives;

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

	// Retrieve a primitive
	const LXWorldPrimitive*			GetWorldPrimitive(const LXPrimitiveInstance* PrimitiveInstance);

	// Add a primitive to the root mesh
	void							AddPrimitive(const shared_ptr<LXPrimitive>& Primitive, LXMatrix* Matrix = nullptr, LXMaterial* Material = nullptr);
		
	// Misc
	void							SetMesh(LXMesh* Mesh);
	LXMesh*							GetMesh() { return Mesh; }
	void							SetAssetMesh(LXAssetMesh* AssetMesh);

private:

	void							GetAllPrimitives(LXMesh* InMesh, TWorldPrimitives& OutWorldPrimitives, const LXMatrix& MatrixWCSParent);
	void							UpdateAssetMeshCallbacks();
	void							UpdateMesh();
	void							OnInvalidateMatrixWCS() override;
		
protected:

	LXMesh* Mesh = nullptr;
	LXAssetMesh* _AssetMesh = nullptr;

	// Additional size applied to the primitive BBoxWorlds ( in the LXWorldPrimitive structure )
	// Useful when displacement is applied to the primitive vertex, like a terrain patch.
	float _ExtendZ = 0.f;

	GetSetDef(int, _nLayer, Layer, 0);
	GetSetDef(bool, _showOutlines, ShowOutlines, false);
	GetSetDef(bool, _bCastShadows, CastShadows, true);

private:

	bool _bValidWorldPrimitives = false;
	TWorldPrimitives _WorldPrimitives;

	// Instances
	uint _InstanceCount = 0;
	ArrayVec3f _ArrayInstancePosition;
	
};

typedef list<LXActorMesh*> ListMeshs;
typedef set<LXActorMesh*> SetMeshs;
typedef vector<LXActorMesh*> ArrayMeshs;