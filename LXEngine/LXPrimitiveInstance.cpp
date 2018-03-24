//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXPrimitiveInstance.h"
#include "LXPrimitive.h"
#include "LXMatrix.h"
#include "LXStatistic.h"
#include "LXMemory.h" // --- Must be the last included ---

LXPrimitiveInstance::LXPrimitiveInstance(const shared_ptr<LXPrimitive>& InPrimitive, LXMatrix* InMatrix, LXMaterial* InMaterial)
{
	LX_COUNTSCOPEINC(LXPrimitiveInstance)
	Primitive = InPrimitive;
	Matrix = InMatrix;
	Material = InMaterial;
}

LXPrimitiveInstance::~LXPrimitiveInstance()
{
	LX_COUNTSCOPEDEC(LXPrimitiveInstance)
	LX_SAFE_DELETE(Matrix);
}

