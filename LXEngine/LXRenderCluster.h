//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXBBox.h"
#include "LXMatrix.h"
#include "LXConstantBufferD3D11.h"
#include "LXShaderProgramD3D11.h"
#include "LXRenderClusterType.h"
#include "LXRenderPassType.h"
#include "LXFlags.h"

class LXActor;
class LXActorMesh;
class LXConstantBufferD3D11;
class LXMaterial;
class LXMatrix;
class LXPrimitiveD3D11;
class LXRenderClusterManager;
class LXRenderCommandList;
class LXShaderD3D11;
class LXWorldPrimitive;
enum class ELightType;

enum class ERenderClusterType
{
	Surface = LX_BIT(0),
	Light = LX_BIT(1),
	Auxiliary = LX_BIT(2)
};

typedef LXFlags<ERenderClusterType> LXFlagsRenderCluster;

struct LXConstantBufferDataSpotLight;

class LXRenderCluster
{

public:

	LXRenderCluster(LXRenderClusterManager* RenderClusterManager, LXActor* InActor, const LXMatrix& MatrixWCS);
	~LXRenderCluster();

	void ReleaseShaders();

	void SetMaterial(LXMaterial* material);
	void SetPrimitive(shared_ptr<LXPrimitiveD3D11>& InPrimitiveD3D11);
			
	void SetMatrix(const LXMatrix& InMatrix);
	void SetBBoxWorld(const LXBBox& Box);
	
	bool IsTransparent() const;
	
	void Render(ERenderPass RenderPass, LXRenderCommandList* RCL);

	// Cluster Specialization according the type
	void SetLightParameters(LXActor* Actor);
	void UpdateLightParameters(LXActor* Actor);
	void UpdateLightParameters();
	ELightType GetLightType() const;

private:

	bool UpdateDeviceMaterialAndShaders();

public:

	LXActor* Actor = nullptr;
	LXWorldPrimitive* PrimitiveInstance = nullptr;

	LXConstantBufferD3D11* CBWorld = nullptr;
	LXConstantBufferData1 cb1;

	LXConstantBufferDataSpotLight* ConstantBufferDataSpotLight = nullptr;
	LXConstantBufferData0* LightView = nullptr;
		
	LXMaterial* Material = nullptr;
	shared_ptr<LXPrimitiveD3D11> Primitive;
		
	LXBBox BBoxWorld;
	
	LXShaderProgramD3D11 ShaderPrograms[(int)ERenderPass::Last];
		
	// Cache
	LXMatrix Matrix;
	bool ValidConstantBufferMatrix = false;
	
	bool CastShadow = false;

	LXFlagsRenderCluster Flags = ERenderClusterType::Surface;
	LXFlagsRenderClusterRole Role = ERenderClusterRole::Default;

private:

	LXRenderClusterManager* _renderClusterManager;
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