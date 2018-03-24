//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXBBox.h"
#include "LXMatrix.h"
#include "LXConstantBufferD3D11.h"
#include "LXShaderProgramD3D11.h"
#include "LXRenderPass.h"

class LXActor;
class LXActorMesh;
class LXConstantBufferD3D11;
class LXMaterialD3D11;
class LXMatrix;
class LXPrimitiveD3D11;
class LXPrimitiveInstance;
class LXRenderClusterManager;
class LXRenderCommandList;
class LXShaderD3D11;

class LXRenderCluster
{

public:

	LXRenderCluster(LXRenderClusterManager* RenderClusterManager, LXActor* InActor, const LXMatrix& MatrixWCS);
	~LXRenderCluster();

	bool SetMaterial(shared_ptr<LXMaterialD3D11>& InMaterial);
	void SetPrimitive(shared_ptr<LXPrimitiveD3D11>& InPrimitiveD3D11);
			
	void SetMatrix(const LXMatrix& InMatrix);
	void SetBBoxWorld(const LXBBox& Box);
	
	bool IsTransparent() const;
	
	void Render(ERenderPass RenderPass, LXRenderCommandList* RCL);

public:

	LXActor* Actor = nullptr;
	LXPrimitiveInstance* PrimitiveInstance = nullptr;

	LXConstantBufferD3D11* CBWorld = nullptr;
	LXConstantBufferData1 cb1;
	
	shared_ptr<LXMaterialD3D11> Material;
	shared_ptr<LXPrimitiveD3D11> Primitive;
		
	LXBBox BBoxWorld;
	
	LXShaderProgramD3D11 ShaderPrograms[(int)ERenderPass::Last];
		
	// Cache
	LXMatrix Matrix;
	bool ValidConstantBufferMatrix = false;
	

	bool CastShadow = false;

};

static bool SortRenderCluster(const LXRenderCluster& RC0, const LXRenderCluster& RC1)
{
// 	VertexShader
// 	PixelShader
// 	HullShader
// 	DomainShader
// 	MaterialRenderProxy
// 	VertexFactory
// 	SceneTextureMode
// 	bAllowGlobalFog
// 	bEnableSkyLight
// 	bEnableReceiveDecalOutput

	CHK(0);

// 	if (RC0.ShaderProgram->VertexShader != RC1.ShaderProgram->VertexShader) return false;
// 	if (RC0.ShaderProgram->PixelShader != RC1.ShaderProgram->PixelShader) return false;
// 	if (RC0.Material != RC1.Material) return false;
// 	if (RC0.Primitive != RC1.Primitive) return false;

	return true;
};