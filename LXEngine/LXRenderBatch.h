//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include "LXShaderProgramD3D11.h"

class LXConstantBufferD3D11;
class LXMaterialD3D11;
class LXPrimitiveD3D11;
class LXRenderCommandList;

enum class ERenderPass;

class LXRenderBatch : public LXObject
{

public:


	//LXRenderBatch

	void Render(ERenderPass RenderPass, LXRenderCommandList* RCL);

	LXConstantBufferD3D11* CBWorld;
	LXShaderProgramD3D11 ShaderProgram;
	std::shared_ptr<LXMaterialD3D11> Material;
	std::shared_ptr<LXPrimitiveD3D11> Primitive;
};

class LXRenderBatchLOD
{
public:

};