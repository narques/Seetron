//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"

class LXShaderD3D11;

class LXShaderProgramD3D11 : public LXObject
{

public:

	LXShaderProgramD3D11();
	virtual ~LXShaderProgramD3D11();
	
public:

	shared_ptr<LXShaderD3D11> HullShader;
	shared_ptr<LXShaderD3D11> DomainShader;
	shared_ptr<LXShaderD3D11> GeometryShader;
	shared_ptr<LXShaderD3D11> VertexShader;
	shared_ptr<LXShaderD3D11> PixelShader;
};

