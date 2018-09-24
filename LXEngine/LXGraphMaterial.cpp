//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXGraphMaterial.h"
#include "LXConnector.h"
#include "LXNode.h"

LXGraphMaterial::LXGraphMaterial(LXMaterial* material):Material(material)
{
	SetName(L"GraphMaterial");
}

LXGraphMaterial::~LXGraphMaterial()
{
}

void LXGraphMaterial::GetChildProperties(ListProperties& listProperties) const
{
	for (const LXNode* node : Nodes)
	{
		node->GetUserProperties(listProperties);
	}
}