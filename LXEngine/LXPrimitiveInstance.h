//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

class LXPrimitive;
class LXMatrix;
class LXMaterial;

// Represents an instanced Primitive in a Mesh 
// CPU/DataModel instantiation feature, different of the DrawInstanced GPU features
// It may have a matrix (Local, in the Mesh coordinate system).
// It may have a overridden material.

class LXPrimitiveInstance
{

public:

	LXPrimitiveInstance(const shared_ptr<LXPrimitive>& InPrimitive, LXMatrix* InMatrix, LXMaterial* InMaterial);
	~LXPrimitiveInstance();

public:

	// Ref
	shared_ptr<LXPrimitive> Primitive;

	// Local (Optional)
	LXMatrix* Matrix = nullptr;

	// Material (Optional)
	LXMaterial* Material = nullptr;

};

