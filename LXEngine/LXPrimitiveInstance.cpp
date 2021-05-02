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
#include "LXMaterial.h"
#include "LXMaterialBase.h"
#include "LXMatrix.h"
#include "LXStatistic.h"
#include "LXActorMesh.h"

LXPrimitiveInstance::LXPrimitiveInstance(const std::shared_ptr<LXPrimitive>& InPrimitive, const LXMatrix* InMatrix, const std::shared_ptr<LXMaterialBase>& InMaterial)
{
	LX_COUNTSCOPEINC(LXPrimitiveInstance)
	Primitive = InPrimitive;
	if (InMatrix)
	{
		Matrix = new LXMatrix(*InMatrix);
	}
	Material = InMaterial;
}

LXPrimitiveInstance::~LXPrimitiveInstance()
{
	LX_COUNTSCOPEDEC(LXPrimitiveInstance)
	LX_SAFE_DELETE(Matrix);
	LX_SAFE_DELETE(MatrixRCS);
}

void LXPrimitiveInstance::SetMaterial(std::shared_ptr<LXMaterialBase>& material)
{
	// If a PerInstance material exists, we should not set the primitive material (no effect).
	CHK(!Material);
	
	Primitive->SetMaterial(material);
	for (LXWorldPrimitive* worldPrimitive : Owners)
	{
		std::shared_ptr<LXMaterialBase> materialBase = std::dynamic_pointer_cast<LXMaterialBase>(material);
		worldPrimitive->SetMaterial(materialBase.get());
	}
}

