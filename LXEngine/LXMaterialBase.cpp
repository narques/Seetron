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
#include "LXCore.h"
#include "LXRenderer.h"

LXMaterialBase::LXMaterialBase()
{
}

LXMaterialBase::~LXMaterialBase()
{
	ReleaseDeviceMaterial();
}

void LXMaterialBase::CreateDeviceMaterial()
{
	if (GetRenderer())
	{
		GetRenderer()->CreateDeviceMaterial(shared_from_this());
	}
}

void LXMaterialBase::ReleaseDeviceMaterial()
{
	if (_materialD3D11 && GetRenderer())
	{
		GetRenderer()->ReleaseDeviceMaterial(this);
	}
}

