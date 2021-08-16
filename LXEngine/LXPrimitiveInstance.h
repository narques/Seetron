//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

class LXPrimitive;
class LXMaterialBase;
class LXWorldPrimitive;

// Represents an instanced Primitive in a Mesh 
// CPU/DataModel instantiation feature, different of the DrawInstanced GPU features
// It may have a matrix (Local, in the Mesh coordinate system).
// It may have a overridden material.

class LXPrimitiveInstance
{

public:

	LXPrimitiveInstance(const std::shared_ptr<LXPrimitive>& InPrimitive, const LXMatrix* InMatrix, const std::shared_ptr<LXMaterialBase>& InMaterial);
	~LXPrimitiveInstance();

	// Set the material to the Primitive itself (not the instance)
	void SetMaterial(std::shared_ptr<LXMaterialBase>& material);

public:

	// Ref
	std::shared_ptr<LXPrimitive> Primitive;

	// Owners
	std::list<LXWorldPrimitive*> Owners;

	// Local matrix (Optional)
	const LXMatrix* Matrix = nullptr;

	// Root matrix (when Matrix is used)
	const LXMatrix* MatrixRCS = nullptr;

	// Material (Optional)
	std::shared_ptr<LXMaterialBase> Material;
};


