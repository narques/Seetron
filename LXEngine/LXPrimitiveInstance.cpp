//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXPrimitiveInstance.h"
#include "LXPrimitive.h"
#include "LXMatrix.h"
#include "LXStatistic.h"
#include "LXActorMesh.h"
#include "LXMemory.h" // --- Must be the last included ---

LXPrimitiveInstance::LXPrimitiveInstance(const shared_ptr<LXPrimitive>& InPrimitive, LXMatrix* InMatrix, LXMaterial* InMaterial)
{
	LX_COUNTSCOPEINC(LXPrimitiveInstance)
	Primitive = InPrimitive;
	Matrix = InMatrix;
	if (Matrix)
	{
		MatrixRCS = new LXMatrix();
	}
	Material = InMaterial;
}

LXPrimitiveInstance::~LXPrimitiveInstance()
{
	LX_COUNTSCOPEDEC(LXPrimitiveInstance)
	LX_SAFE_DELETE(Matrix);
	LX_SAFE_DELETE(MatrixRCS);
}

void LXPrimitiveInstance::SetMaterial(LXMaterial* material)
{
	Primitive->SetMaterial(material);
	for (LXWorldPrimitive* worldPrimitive : Owners)
	{
		worldPrimitive->SetMaterial(material);
	}
}

