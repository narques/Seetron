//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

enum class EShader // DataModel
{
	UndefinedShader = 0,
	VertexShader = LX_BIT(0),
	HullShader = LX_BIT(1),
	DomainShader = LX_BIT(2),
	GeometryShader = LX_BIT(3),
	PixelShader = LX_BIT(4)
};
