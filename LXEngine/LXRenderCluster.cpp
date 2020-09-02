//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXRenderCluster.h"

// Seetron
#include "LXActorLight.h"
#include "LXCamera.h"
#include "LXConstantBufferD3D11.h"
#include "LXCore.h"
#include "LXDeviceResourceManager.h"
#include "LXDirectX11.h"
#include "LXLogger.h"
#include "LXMaterial.h"
#include "LXMaterialD3D11.h"
#include "LXPrimitiveD3D11.h"
#include "LXPrimitiveFactory.h"
#include "LXPrimitiveInstance.h"
#include "LXRenderClusterManager.h"
#include "LXRenderCommandList.h"
#include "LXRenderer.h"
#include "LXRenderPassShadow.h"
#include "LXShaderManager.h"
#include "LXShaderProgramD3D11.h"
#include "LXStatistic.h"
#include "LXWorldTransformation.h"

LXRenderCluster::LXRenderCluster(LXRenderClusterManager* renderClusterManager, LXRenderData* renderData, const LXMatrix& matrixWCS, const LXMatrix& matrix):
	_renderClusterManager(renderClusterManager)
{
	LX_COUNTSCOPEINC(LXRenderCluster)
	renderData->RenderClusters.push_back(this);
	
	//
	// Misc Ref.
	// 

	RenderData = renderData;
	MatrixWCS = matrixWCS;
	MatrixWCS.LocalToParent(BBoxWorld);
	LocalPosition = matrix.GetOrigin();

	//
	//
	//

	LXConstantBufferData1 cb1;

	cb1.World = Transpose(MatrixWCS);
	cb1.Normal = Inverse(MatrixWCS/*cb1.World*/);
	cb1.LocalPosition = LocalPosition;
	
	CBWorld = new LXConstantBufferD3D11();
	CBWorld->CreateConstantBuffer(&cb1, sizeof(LXConstantBufferData1));

	// TODO : Au lieu d'un UpdateSubresource, passer la valeur dans CreateConstantBuffer. Type void*. Défaut à null.
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	D3D11DeviceContext->UpdateSubresource(CBWorld->D3D11Buffer, 0, nullptr, &cb1, 0, 0);
}

LXRenderCluster::~LXRenderCluster()
{
	LX_COUNTSCOPEDEC(LXRenderCluster)
	RenderData->RenderClusters.remove(this);
	LX_SAFE_DELETE(CBWorld);
	LX_SAFE_DELETE(ConstantBufferDataSpotLight);
	LX_SAFE_DELETE(LightView);
}

void LXRenderCluster::SetMaterial(const LXMaterialBase* material)
{
	if (Material != material)
	{
		Material = material;
	}
}

bool LXRenderCluster::UpdateDeviceMaterialAndShaders(ERenderPass renderPass)
{
	CHK(IsRenderThread());
	
	LXRenderer* renderer = GetCore().GetRenderer();
	   	   
#if LX_BUILD_SHADERS_FOR_ALL_PASSES 
	for (auto i = 0; i < (int)ERenderPass::Last; i++)
	{
		renderPass = (ERenderPass)i;
#endif

		// Resources
		const shared_ptr<LXMaterialD3D11> materialD3D11 = renderer->GetDeviceResourceManager()->GetShaderResources(renderPass, Material);
		ShaderResources[(int)renderPass] = materialD3D11;

		// Shaders
		LXShaderProgramD3D11 shaderProgram;
		if (renderer->GetShaderManager()->GetShaders(renderPass, Primitive.get(), materialD3D11.get(), &shaderProgram))
		{
			LXShaderProgramD3D11* RenderClusterShaderProgram = &ShaderPrograms[(int)renderPass];
			RenderClusterShaderProgram->VertexShader = shaderProgram.VertexShader;
			RenderClusterShaderProgram->HullShader = shaderProgram.HullShader;
			RenderClusterShaderProgram->DomainShader = shaderProgram.DomainShader;
			RenderClusterShaderProgram->GeometryShader = shaderProgram.GeometryShader;
			RenderClusterShaderProgram->PixelShader = shaderProgram.PixelShader;
		}
		else
		{
			return false;
		}
#if LX_BUILD_SHADERS_FOR_ALL_PASSES 
	}
#endif

	return true;
}

bool LXRenderCluster::GetDeviceMaterialAndShaders(ERenderPass renderPass, const LXPrimitiveD3D11* primitive, const LXMaterialBase* material, LXShaderProgramD3D11& shaderProgram, shared_ptr<LXMaterialD3D11>& shaderResources)
{
	CHK(IsRenderThread());

	LXRenderer* renderer = GetCore().GetRenderer();

	// Resources
	const shared_ptr<LXMaterialD3D11> materialD3D11 = renderer->GetDeviceResourceManager()->GetShaderResources(renderPass, material);
	shaderResources = materialD3D11;

	// Shaders
	if (renderer->GetShaderManager()->GetShaders(renderPass, primitive, materialD3D11.get(), &shaderProgram))
	{
		shaderProgram.VertexShader = shaderProgram.VertexShader;
		shaderProgram.HullShader = shaderProgram.HullShader;
		shaderProgram.DomainShader = shaderProgram.DomainShader;
		shaderProgram.GeometryShader = shaderProgram.GeometryShader;
		shaderProgram.PixelShader = shaderProgram.PixelShader;

		return true;
	}
	else
	{
		return false;
	}

}

void LXRenderCluster::SetPrimitive(shared_ptr<LXPrimitiveD3D11>& InPrimitiveD3D11)
{
	CHK(Primitive == nullptr);
	Primitive = InPrimitiveD3D11;
}

void LXRenderCluster::SetMatrix(const LXMatrix& InMatrix)
{
	 MatrixWCS = InMatrix;
	 ValidConstantBufferMatrix = false;
}

void LXRenderCluster::SetBBoxWorld(const LXBBox& Box)
{
	BBoxWorld = Box;
}

bool LXRenderCluster::IsTransparent() const
{
	return Material->IsTransparent();
}

void LXRenderCluster::Render(ERenderPass RenderPass, LXRenderCommandList* RCL)
{
	bool result = UpdateDeviceMaterialAndShaders(RenderPass);
		
	if (!result)
	{
		return;
	}
	
	const LXShaderProgramD3D11* ShaderProgram = &ShaderPrograms[(int)RenderPass];
	const shared_ptr<LXMaterialD3D11>& materialD3D11 = ShaderResources[(int)RenderPass];

	if (1/*!ValidConstantBufferMatrix*/ && CBWorld)
	{
		//LXConstantBufferData1 cb1;
		cb1.World = Transpose(MatrixWCS);
		cb1.Normal = Inverse(MatrixWCS);
		cb1.LocalPosition = LocalPosition;
		RCL->UpdateSubresource4(CBWorld->D3D11Buffer, &cb1);
		ValidConstantBufferMatrix = true;
	}

	bool bHasShader = false;

	// Vertex Shader
	if (ShaderProgram->VertexShader)
	{
		RCL->IASetInputLayout(ShaderProgram->VertexShader.get());
		RCL->VSSetShader(ShaderProgram->VertexShader.get());
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
	if (ShaderProgram->HullShader)
	{
		RCL->HSSetShader(ShaderProgram->HullShader.get());
		bHasShader = true;
	}

	// Domain Shader
	if (ShaderProgram->DomainShader)
	{
		RCL->DSSetShader(ShaderProgram->DomainShader.get());
		bHasShader = true;
	}

	// Geometry Shader
	if (ShaderProgram->GeometryShader)
	{
		RCL->GSSetShader(ShaderProgram->GeometryShader.get());
		bHasShader = true;
	}
	
	// Pixel Shader	TODO why tested ?
	if (ShaderProgram->PixelShader)
	{
		RCL->PSSetShader(ShaderProgram->PixelShader.get());
		bHasShader = true;

		if (CBWorld)
		{
			RCL->PSSetConstantBuffers(1, 1, CBWorld);
		}
	}

	if (bHasShader)
	{
		materialD3D11->Render(RenderPass, RCL);

		Primitive->Render(RCL);
	}
}

void LXRenderCluster::RenderBounds(ERenderPass RenderPass, LXRenderCommandList* RCL)
{
	// GetRenderBBoxResources is unique 
	CHK(RenderPass == ERenderPass::GBuffer);
	LXRenderBBoxResources* rbr = GetRenderer()->GetRenderBBoxResources();
	LXShaderProgramD3D11& shaderProgram = rbr->ShaderProgramD3D11;
	bool result = GetDeviceMaterialAndShaders(RenderPass, rbr->WireframeCubeD3D11, rbr->WireframeCube->GetMaterial().get(), rbr->ShaderProgramD3D11, rbr->MaterialD3D11);

	if (!result)
	{
		return;
	}

	LXMatrix MatrixScale, MatrixTranslation;
	// Max/1.f to avoid a 0 scale value ( possible with the "flat" geometries )
	MatrixScale.SetScale(max(BBoxWorld.GetSizeX(), 1.f), max(BBoxWorld.GetSizeY(), 1.f), max(BBoxWorld.GetSizeZ(), 1.f));
	MatrixTranslation.SetTranslation(BBoxWorld.GetCenter());
	LXMatrix matrixWCS = MatrixTranslation * MatrixScale;

	if (1/*!ValidConstantBufferMatrix*/ && CBWorld)
	{
		//LXConstantBufferData1 cb1;
		cb2.World = Transpose(matrixWCS);
		cb2.Normal = Inverse(matrixWCS);
		cb2.LocalPosition = LocalPosition;
		RCL->UpdateSubresource4(CBWorld->D3D11Buffer, &cb2);
		ValidConstantBufferMatrix = true;
	}

	bool bHasShader = false;

	// Vertex Shader
	if (shaderProgram.VertexShader)
	{
		RCL->IASetInputLayout(shaderProgram.VertexShader.get());
		RCL->VSSetShader(shaderProgram.VertexShader.get());
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
	if (shaderProgram.HullShader)
	{
		RCL->HSSetShader(shaderProgram.HullShader.get());
		bHasShader = true;
	}

	// Domain Shader
	if (shaderProgram.DomainShader)
	{
		RCL->DSSetShader(shaderProgram.DomainShader.get());
		bHasShader = true;
	}

	// Geometry Shader
	if (shaderProgram.GeometryShader)
	{
		RCL->GSSetShader(shaderProgram.GeometryShader.get());
		bHasShader = true;
	}

	// Pixel Shader	TODO why tested ?
	if (shaderProgram.PixelShader)
	{
		RCL->PSSetShader(shaderProgram.PixelShader.get());
		bHasShader = true;

		if (CBWorld)
		{
			RCL->PSSetConstantBuffers(1, 1, CBWorld);
		}
	}

	if (bHasShader)
	{
		rbr->MaterialD3D11->Render(RenderPass, RCL);
		rbr->WireframeCubeD3D11->Render(RCL);
	}

}

void LXRenderCluster::SetLightParameters(LXActor* Actor)
{
	CHK(!ConstantBufferDataSpotLight);
	CHK(Flags & ERenderClusterType::Light);
	LXActorLight* ActorLight = dynamic_cast<LXActorLight*>(Actor);
	CHK(ActorLight);
	ConstantBufferDataSpotLight = new LXConstantBufferDataSpotLight();
	LightView = new LXConstantBufferData0;

	UpdateLightParameters(Actor);
}

void LXRenderCluster::UpdateLightParameters(LXActor* Actor)
{
	LXActorLight* ActorLight = dynamic_cast<LXActorLight*>(Actor);
	CHK(ActorLight);

	vec3f LightPosition = ActorLight->GetMatrixWCS().GetOrigin();
	vec3f LightDirection = ActorLight->GetMatrixWCS().GetVz() * -1.f;

	#pragma message("move actor volatile: move depuis le thread RT")
	LXCamera Camera;
	Camera.SetPosition(LightPosition);
	Camera.SetDirection(LightDirection);
	Camera.SetFov(ActorLight->GetSpotAngle());
	Camera.SetAspectRatio(1.0);

	// Camera to WorlTransformation
	LXWorldTransformation WorldTransformation;
	WorldTransformation.FromCamera(&Camera, -1, -1);

	// Also updated in RenderPassShadow
	LXMatrix MatrixLightView = WorldTransformation.GetMatrixView();
	LXMatrix MatrixLightProjection = WorldTransformation.GetMatrixProjection();
	
	ConstantBufferDataSpotLight->MatrixLightView = Transpose(MatrixLightView);
	ConstantBufferDataSpotLight->MatrixLightProjection = Transpose(MatrixLightProjection);
	ConstantBufferDataSpotLight->LightPosition = vec4f(LightPosition, 0.f);
	ConstantBufferDataSpotLight->LightIntensity = ActorLight->GetIntensity();
	ConstantBufferDataSpotLight->Angle = LX_DEGTORAD(ActorLight->GetSpotAngle()) * 0.5f;
	ConstantBufferDataSpotLight->CastShadow = ActorLight->GetCastShadow();
}

void LXRenderCluster::UpdateLightParameters()
{
	LXActor* actor = const_cast<LXActor*>(RenderData->GetActor());
	LXActorLight* ActorLight = dynamic_cast<LXActorLight*>(actor);
	CHK(ActorLight);
	CHK(ConstantBufferDataSpotLight);

	// Matrices are already computed during RenderPassShadow
	if (ActorLight->GetCastShadow())
	{
		ConstantBufferDataSpotLight->MatrixLightView = LightView->View;
		ConstantBufferDataSpotLight->MatrixLightProjection = LightView->Projection;
		ConstantBufferDataSpotLight->LightPosition = LightView->CameraPosition;
	}
	else
	{
		ConstantBufferDataSpotLight->MatrixLightView = Transpose(MatrixWCS);
		ConstantBufferDataSpotLight->LightPosition = vec4f(ActorLight->GetPosition(), 0.0);
	}

	ConstantBufferDataSpotLight->LightDirection = vec4f(MatrixWCS.GetVz(), 0.0) * -1.f;
	
	// ShadowMapCoords updated during RenderPassShadow
	//ConstantBufferDataSpotLight->ShadowMapCoords;

	if (ActorLight->GetType() != ELightType::Spot)
	{
		ConstantBufferDataSpotLight->CastShadow = false;
	}
	
	// Misc.
	ConstantBufferDataSpotLight->Color = ActorLight->GetColor();
	ConstantBufferDataSpotLight->LightIntensity = ActorLight->GetIntensity();
	ConstantBufferDataSpotLight->Angle = LX_DEGTORAD(ActorLight->GetSpotAngle()) * 0.5f;
	ConstantBufferDataSpotLight->CastShadow = ActorLight->GetCastShadow();
}

ELightType LXRenderCluster::GetLightType() const
{
	LXActor* actor = const_cast<LXActor*>(RenderData->GetActor());
	LXActorLight* actorLight = dynamic_cast<LXActorLight*>(actor);
	CHK(actorLight);
	return actorLight->GetType();
}

void LXRenderCluster::ReleaseShaders()
{
	for(int i=0; i <(int)ERenderPass::Last; i++)
	{
		LXShaderProgramD3D11& shaderProgram = ShaderPrograms[i];
		shaderProgram.Release();
	}
}