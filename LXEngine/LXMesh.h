//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXSmartObject.h"
#include "LXMatrix.h"
#include "LXTransformation.h"
#include "LXBBox.h"

class LXMesh;
class LXPrimitive;
class LXAssetMesh;

typedef list<LXMesh*> ListMeshes;

class LXPrimitiveInstance;
typedef vector <unique_ptr<LXPrimitiveInstance>> VectorPrimitiveInstances;

class LXCORE_API LXMesh : public LXSmartObject
{
	friend class LXActorMesh;

public:

	LXMesh();
	LXMesh(LXAssetMesh* InOwner);
	LXMesh(const LXMesh& Object) = delete;
	LXMesh& operator=(const LXMesh& Object) = delete;
	virtual ~LXMesh();
		
	// Overridden from LXSmartObject
	bool OnSaveChild(const TSaveContext& saveContext) const override;
	bool OnLoadChild(const TLoadContext& LoadContext) override;

	// Hierarchy
	void SetParent(LXMesh* InParent) { _Parent = InParent; }
	LXMesh* GetParent() const { return _Parent; }
	void AddChild(LXMesh* Mesh);
	const ListMeshes& GetChild() const { return _Children; } // GetChildren already exists in SmartObjects

	// Primitive management
	void AddPrimitive(const shared_ptr<LXPrimitive>& Primitive, LXMatrix* Matrix = nullptr, LXMaterial* Material = nullptr);
	void RemovePrimitive(LXPrimitive* Primitive);
	void RemoveAllPrimitives();
	const VectorPrimitiveInstances& GetPrimitives() { return _vectorPrimitives; }

	// Return the primitives, including child primitives
	void GetAllPrimitives(vector<LXPrimitiveInstance*>& primitives);

	//Bounds
	const LXBBox& GetBounds();
	void ComputeBounds();
	void InvalidateBounds();

	// Local Transformation
	LXTransformation& GetTransformation() { return _Transformation; }
	const LXMatrix& GetMatrix() { return _Transformation.GetMatrix(); }

	void ComputeMatrixRCS();

	// Misc
	void SetMaterial(const LXString& Key);
	void SetMaterial(LXMaterial* material);

	bool Visible() const { return _visible; }

private:

	void ComputeMatrixRCS(const LXMatrix* matrixParentRCS);

private:

	// Bounds
	LXBBox		_BBox;	// Local BBox

	// Local Transformation
	LXTransformation _Transformation;

	// Hierarchy
	LXMesh*		_Parent = nullptr;
	ListMeshes  _Children;

	// Renderer
	bool		_visible = true;

	// Owner
	LXAssetMesh* _Owner = nullptr;

	// Primitive
	VectorPrimitiveInstances _vectorPrimitives;

};









