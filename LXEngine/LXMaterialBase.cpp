//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXMaterialBase.h"

// Seetron
#include "LXEngine.h"
#include "LXRenderer.h"

LXMaterialBase::LXMaterialBase()
{
}

LXMaterialBase::~LXMaterialBase()
{
}

void LXMaterialBase::ReleaseDeviceMaterials()
{
	if (GetRenderer())
	{
		GetRenderer()->ReleaseDeviceMaterials(this);
	}
}

