//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXRenderBatch.h"

// Seetron
#include "LXMaterialD3D11.h"
#include "LXPrimitiveD3D11.h"
#include "LXRenderCommandList.h"

void LXRenderBatch::Render(ERenderPass RenderPass, LXRenderCommandList* RCL)
{
	bool bHasShader = false;

	// Vertex Shader
	if (ShaderProgram.VertexShader)
	{
		RCL->IASetInputLayout(ShaderProgram.VertexShader.get());
		RCL->VSSetShader(ShaderProgram.VertexShader.get());
		bHasShader = true;

		if (RCL->CBViewProjection)
		{
			RCL->VSSetConstantBuffers(0, 1, RCL->CBViewProjection);
		}

		if (CBWorld)
		{
			RCL->VSSetConstantBuffers(1, 1, CBWorld);
		}
	}

	// Hull Shader
	if (ShaderProgram.HullShader)
	{
		RCL->HSSetShader(ShaderProgram.HullShader.get());
		bHasShader = true;
	}

	// Domain Shader
	if (ShaderProgram.DomainShader)
	{
		RCL->DSSetShader(ShaderProgram.DomainShader.get());
		bHasShader = true;
	}

	// Geometry Shader
	if (ShaderProgram.GeometryShader)
	{
		RCL->GSSetShader(ShaderProgram.GeometryShader.get());
		bHasShader = true;
	}

	// Pixel Shader	TODO why tested ?
	if (ShaderProgram.PixelShader)
	{
		RCL->PSSetShader(ShaderProgram.PixelShader.get());
		bHasShader = true;

		if (CBWorld)
		{
			RCL->PSSetConstantBuffers(1, 1, CBWorld);
		}
	}


	Material->Render(RenderPass, RCL);
	Primitive->Render(RCL);

}
