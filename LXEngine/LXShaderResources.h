//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"

//Seetron
#include "LXRenderPassType.h"

class LXRenderCommandList;

class LXShaderResources : public LXObject
{
public:
};

class LXTextureD3D11;

class LXShaderResourcesD3D11 : public LXShaderResources
{
public:
};

class LXShaderTexturesD3D11 : public LXShaderResources
{
public:

	void Render(ERenderPass renderPass, LXRenderCommandList* RCL) const;

	std::list<LXTextureD3D11*> TexturesVS;
	std::list<LXTextureD3D11*> TexturesPS;
};

class LXShaderConstantBuffers : public LXShaderResources
{

};