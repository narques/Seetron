//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXRenderCluster.h"
#include "LXActorCamera.h"
#include "LXActorLight.h"
#include "LXActorMesh.h"
#include "LXConstantBufferD3D11.h"
#include "LXCore.h"
#include "LXDirectX11.h"
#include "LXLogger.h"
#include "LXMaterial.h"
#include "LXMaterialD3D11.h"
#include "LXPrimitiveD3D11.h"
#include "LXPrimitiveInstance.h"
#include "LXRenderClusterManager.h"
#include "LXRenderCommandList.h"
#include "LXRenderer.h"
#include "LXRenderPassShadow.h"
#include "LXShaderProgramD3D11.h"
#include "LXStatistic.h"
#include "LXWorldTransformation.h"
#include "LXMemory.h" // --- Must be the last included ---

LXRenderCluster::LXRenderCluster(LXRenderClusterManager* RenderClusterManager, LXActor* InActor, const LXMatrix& MatrixWCS)
{
	LX_COUNTSCOPEINC(LXRenderCluster)

	//
	// Misc Ref.
	// 

	Actor = InActor;
	Matrix = MatrixWCS;
	MatrixWCS.LocalToParent(BBoxWorld);

	//
	//
	//

	LXConstantBufferData1 cb1;

	cb1.World = Transpose(MatrixWCS);
	cb1.Normal = Inverse(MatrixWCS/*cb1.World*/);

	CBWorld = new LXConstantBufferD3D11();
	CBWorld->CreateConstantBuffer(&cb1, sizeof(LXConstantBufferData1));

	// TODO : Au lieu d'un UpdateSubresource, passer la valeur dans CreateConstantBuffer. Type void*. Défaut à null.
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	D3D11DeviceContext->UpdateSubresource(CBWorld->D3D11Buffer, 0, nullptr, &cb1, 0, 0);
}

LXRenderCluster::~LXRenderCluster()
{
	LX_COUNTSCOPEDEC(LXRenderCluster)
	LX_SAFE_DELETE(CBWorld);
	LX_SAFE_DELETE(ConstantBufferDataSpotLight);
	LX_SAFE_DELETE(LightView);
}

bool LXRenderCluster::SetMaterial(shared_ptr<LXMaterialD3D11>& InMaterial)
{
	Material = InMaterial;
	return true;
}

void LXRenderCluster::SetPrimitive(shared_ptr<LXPrimitiveD3D11>& InPrimitiveD3D11)
{
	CHK(Primitive == nullptr)
	Primitive = InPrimitiveD3D11;
}

void LXRenderCluster::SetMatrix(const LXMatrix& InMatrix)
{
	 Matrix = InMatrix;
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
	LXShaderProgramD3D11* ShaderProgram = &ShaderPrograms[(int)RenderPass];

	if (1/*!ValidConstantBufferMatrix*/)
	{
		//LXConstantBufferData1 cb1;
		cb1.World = Transpose(Matrix);
		cb1.Normal = Inverse(Matrix);
		RCL->UpdateSubresource4(CBWorld->D3D11Buffer, &cb1);
		ValidConstantBufferMatrix = true;
	}

	if (ConstantBufferDataSpotLight)
		UpdateLightParameters(Actor);
	

	// Vertex Shader
	if (ShaderProgram->VertexShader)
	{
		RCL->IASetInputLayout(ShaderProgram->VertexShader.get());
		RCL->VSSetShader(ShaderProgram->VertexShader.get());
	}

	if (RCL->CBViewProjection) 
		RCL->VSSetConstantBuffers(0, 1, RCL->CBViewProjection);
	
	if (CBWorld)
		RCL->VSSetConstantBuffers(1, 1, CBWorld);

	// Hull Shader
	RCL->HSSetShader(ShaderProgram->HullShader.get());

	// Domain Shader
	RCL->DSSetShader(ShaderProgram->DomainShader.get());

	// Geometry Shader
	RCL->GSSetShader(ShaderProgram->GeometryShader.get());
	
	// Pixel Shader	TODO why tested ?
	if (ShaderProgram->PixelShader)
		RCL->PSSetShader(ShaderProgram->PixelShader.get());

	// Material (Shader resources)
	if (Material)
		Material->Render(RenderPass, RCL);

	Primitive->Render(RCL);
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
	LXActorCamera Camera(nullptr);
	Camera.SetPosition(LightPosition);
	Camera.SetDirection(LightDirection);
	Camera.SetFov(ActorLight->GetSpotAngle());
	Camera.LookAt(1.0);

	// Camera to WorlTransformation
	LXWorldTransformation WorldTransformation;
	WorldTransformation.FromCamera(&Camera, -1, -1);

	// Also updated in RenderPassShadow
	LXMatrix MatrixLightView = WorldTransformation.GetMatrixView();
	LXMatrix MatrixLightProjection = WorldTransformation.GetMatrixProjection();
	
	ConstantBufferDataSpotLight->MatrixLightView = Transpose(MatrixLightView);
	ConstantBufferDataSpotLight->MatrixLightProjection = Transpose(MatrixLightProjection);
	ConstantBufferDataSpotLight->LightPosition = LightPosition;
	ConstantBufferDataSpotLight->LightIntensity = ActorLight->GetIntensity();
	ConstantBufferDataSpotLight->Angle = LX_DEGTORAD(ActorLight->GetSpotAngle()) * 0.5f;
	ConstantBufferDataSpotLight->CastShadow = ActorLight->GetCastShadow();
}

