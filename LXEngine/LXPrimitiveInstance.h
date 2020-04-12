//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

class LXPrimitive;
class LXMatrix;
class LXMaterial;
class LXWorldPrimitive;

// Represents an instanced Primitive in a Mesh 
// CPU/DataModel instantiation feature, different of the DrawInstanced GPU features
// It may have a matrix (Local, in the Mesh coordinate system).
// It may have a overridden material.

class LXPrimitiveInstance
{

public:

	LXPrimitiveInstance(const shared_ptr<LXPrimitive>& InPrimitive, LXMatrix* InMatrix, LXMaterial* InMaterial);
	~LXPrimitiveInstance();

	void SetMaterial(shared_ptr<LXMaterial>& material);

public:

	// Ref
	shared_ptr<LXPrimitive> Primitive;

	// Owners
	list<LXWorldPrimitive*> Owners;

	// Local matrix (Optional)
	LXMatrix* Matrix = nullptr;

	// Root matrix (when Matrix is used)
	const LXMatrix* MatrixRCS = nullptr;

	// Material (Optional)
	const LXMaterial* Material = nullptr;
};


