//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXMaterialUtility.h"

//Seetron
#include "LXMaterial.h"
#include "LXMaterialBase.h"
#include "LXMaterialInstance.h"

const LXMaterial* LXMaterialUtility::GetMaterialFromBase(const LXMaterialBase* materialBase)
{
	if (const LXMaterial* material = dynamic_cast<const LXMaterial*>(materialBase))
	{
		return material;
	}
	else if (const LXMaterialInstance* materialInstance = dynamic_cast<const LXMaterialInstance*>(materialBase))
	{
		return materialInstance->Parent.get();
	}

	CHK(0);
	return nullptr;
}
