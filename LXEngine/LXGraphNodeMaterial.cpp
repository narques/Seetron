//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXGraphNodeMaterial.h"

LXGraphNodeTextureSampler::LXGraphNodeTextureSampler(LXGraph* graph) : LXNode(graph)
{
	DefineProperties();
}

LXGraphNodeTextureSampler::LXGraphNodeTextureSampler(LXGraph* graph, const LXNodeTemplate* nodeTemplate) : LXNode(graph, nodeTemplate)
{
	DefineProperties();
}

LXGraphNodeTextureSampler::~LXGraphNodeTextureSampler()
{
}

void LXGraphNodeTextureSampler::DefineProperties()
{
	LXPropertySmartObject* properySmartObject = DefineProperty(L"TextureSampler", (LXSmartObject*)&_textureSampler);
}
