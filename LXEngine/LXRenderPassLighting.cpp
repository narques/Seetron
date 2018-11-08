//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXActorSceneCapture.h"
#include "LXConstantBufferD3D11.h"
#include "LXDirectX11.h"
#include "LXPrimitiveD3D11.h"
#include "LXProject.h"
#include "LXRenderCluster.h"
#include "LXRenderCommandList.h"
#include "LXRenderPassGBuffer.h"
#include "LXRenderPassLighting.h"
#include "LXRenderPassShadow.h"
#include "LXRenderPassSSAO.h"
#include "LXRenderPipelineDeferred.h"
#include "LXRenderTarget.h"
#include "LXRenderTargetViewD3D11.h"
#include "LXRenderer.h"
#include "LXScene.h"
#include "LXSettings.h"
#include "LXShaderD3D11.h"
#include "LXShaderManager.h"
#include "LXTextureD3D11.h"
#include "LXViewport.h"
#include "LXWorldTransformation.h"
#include "LXMemory.h" // --- Must be the last included ---

namespace
{
	const wchar_t kIBLShaderFilename[] = L"LightingIBL.hlsl";
	const wchar_t kSpotShaderFilename[] = L"LightingSpot.hlsl";
	const wchar_t kComposeShaderFilename[] = L"LightingCompose.hlsl";
};

LXRenderPassLighting::LXRenderPassLighting(LXRenderer* InRenderer) :LXRenderPass(InRenderer)
{
	// Textures and Samplers
	CreateBuffers(Renderer->Width, Renderer->Height);

	_ShaderProgramIBLLight = new LXShaderProgramBasic();
	_ShaderProgramSpotLight = new LXShaderProgramBasic();
	_ShaderProgramComposeLight = new LXShaderProgramBasic();

	ConstantBufferDataIBL = new LXConstantBufferDataIBL();
	ConstantBufferIBL = new LXConstantBufferD3D11(ConstantBufferDataIBL, sizeof(LXConstantBufferDataIBL));
	
	RebuildShaders();
}

LXRenderPassLighting::~LXRenderPassLighting()
{
	DeleteBuffers();
	LX_SAFE_DELETE(TextureIBL);
	LX_SAFE_DELETE(_ShaderProgramIBLLight);
	LX_SAFE_DELETE(_ShaderProgramSpotLight);
	LX_SAFE_DELETE(ConstantBufferDataIBL);
	LX_SAFE_DELETE(ConstantBufferIBL);
}

void LXRenderPassLighting::CreateBuffers(uint Width, uint Height)
{
	DeleteBuffers();

	RenderTargetDiffuse = new LXRenderTarget(Width, Height, DXGI_FORMAT_R16G16B16A16_FLOAT);
	RenderTargetSpecular = new LXRenderTarget(Width, Height, DXGI_FORMAT_R16G16B16A16_FLOAT);
	RenderTargetCompose = new LXRenderTarget(Width, Height, DXGI_FORMAT_R16G16B16A16_FLOAT);

	LXRenderPipeline* RenderPipeline = Renderer->GetRenderPipeline();
	RenderPipeline->AddToViewDebugger(L"Lighting Diffuse", RenderTargetDiffuse->TextureD3D11, ETextureChannel::ChannelRGB);
	RenderPipeline->AddToViewDebugger(L"Lighting Specular", RenderTargetSpecular->TextureD3D11, ETextureChannel::ChannelRGB);
	RenderPipeline->AddToViewDebugger(L"Lighting Compose", RenderTargetCompose->TextureD3D11, ETextureChannel::ChannelRGB);
}

void LXRenderPassLighting::RebuildShaders()
{
	const LXArrayInputElementDesc& Layout = GetInputElementDescD3D11Factory().GetInputElement_PT();
	_ShaderProgramIBLLight->CreateShaders(GetSettings().GetShadersFolder() + kIBLShaderFilename, &Layout[0], (uint)Layout.size());
	_ShaderProgramSpotLight->CreateShaders(GetSettings().GetShadersFolder() + kSpotShaderFilename, &Layout[0], (uint)Layout.size());
	_ShaderProgramComposeLight->CreateShaders(GetSettings().GetShadersFolder() + kComposeShaderFilename, &Layout[0], (uint)Layout.size());
}

void LXRenderPassLighting::DeleteBuffers()
{
	LX_SAFE_DELETE(RenderTargetDiffuse);
	LX_SAFE_DELETE(RenderTargetSpecular);
	LX_SAFE_DELETE(RenderTargetCompose);
}

void LXRenderPassLighting::Resize(uint Width, uint Height)
{
	CreateBuffers(Width, Height);
}

bool LXRenderPassLighting::IsValid() const
{
	return _ShaderProgramIBLLight->IsValid() && _ShaderProgramSpotLight->IsValid() && _ShaderProgramComposeLight->IsValid();
}

const LXTextureD3D11* LXRenderPassLighting::GetOutputTexture() const 
{ 
	return RenderTargetCompose->TextureD3D11; 
}

void LXRenderPassLighting::Render(LXRenderCommandList* r)
{
	if (!Renderer->GetProject())
		return;

	LXRenderPipelineDeferred* RenderPipelineDeferred = dynamic_cast<LXRenderPipelineDeferred*>(Renderer->GetRenderPipeline());
	CHK(RenderPipelineDeferred);

	const LXActorSceneCapture* SceneCapture = GetCore().GetProject()->GetSceneCapture() ? GetCore().GetProject()->GetSceneCapture() : nullptr;
	LXRenderPassShadow* RenderPassShadow = RenderPipelineDeferred->RenderPassShadow;

	r->BeginEvent(L"Lighting");

	static ID3D11RenderTargetView* RenderTargetViews[2] = { 0 };
	RenderTargetViews[0] = RenderTargetDiffuse->RenderTargetViewD3D11->D3D11RenderTargetView;
	RenderTargetViews[1] = RenderTargetSpecular->RenderTargetViewD3D11->D3D11RenderTargetView;
	r->OMSetRenderTargets3(2, RenderTargetViews, nullptr);
	r->RSSetViewports(Renderer->Width, Renderer->Height);
	r->ClearRenderTargetView(RenderTargetDiffuse->RenderTargetViewD3D11);
	r->ClearRenderTargetView(RenderTargetSpecular->RenderTargetViewD3D11);
	
	// To accumulate
	r->OMSetBlendState(Renderer->GetBlendStateAdd());

	RenderIBL(r, RenderPipelineDeferred);
	RenderSpotLight(r, RenderPipelineDeferred);
	
	r->OMSetBlendState(Renderer->GetBlendStateOpaque());
	
	// Compose
	{
		r->BeginEvent(L"Compose");

		r->OMSetRenderTargets2(RenderTargetCompose->RenderTargetViewD3D11, nullptr);
		
		_ShaderProgramComposeLight->Render(r);

		LXTextureD3D11* Color = RenderPassGBuffer->TextureColor;
		LXTextureD3D11* MRUL = RenderPassGBuffer->TextureSpecular;
		LXTextureD3D11* Emissive = RenderPassGBuffer->TextureEmissive;
		const LXTextureD3D11* TextureSSAO = RenderPassSSAO->GetOutputTexture();

		LXTextureD3D11* Diffuse = RenderTargetDiffuse->TextureD3D11;
		LXTextureD3D11* Specular = RenderTargetSpecular->TextureD3D11;

		r->PSSetShaderResources(1, 1, (LXTextureD3D11*)Color);
		r->PSSetShaderResources(3, 1, (LXTextureD3D11*)MRUL);
		r->PSSetShaderResources(4, 1, (LXTextureD3D11*)Emissive);
		r->PSSetShaderResources(5, 1, (LXTextureD3D11*)Diffuse);
		r->PSSetShaderResources(6, 1, (LXTextureD3D11*)Specular);
		r->PSSetShaderResources(7, 1, (LXTextureD3D11*)TextureSSAO);

		r->PSSetSamplers(1, 1, (LXTextureD3D11*)Color);
		r->PSSetSamplers(3, 1, (LXTextureD3D11*)MRUL);
		r->PSSetSamplers(4, 1, (LXTextureD3D11*)Emissive);
		r->PSSetSamplers(5, 1, (LXTextureD3D11*)Diffuse);
		r->PSSetSamplers(6, 1, (LXTextureD3D11*)Specular);
		r->PSSetSamplers(7, 1, (LXTextureD3D11*)TextureSSAO);

		Renderer->GetSSTriangle()->Render(r);

		r->PSSetShaderResources(1, 1, nullptr); // Color
		r->PSSetShaderResources(3, 1, nullptr); // MRUL
		r->PSSetShaderResources(4, 1, nullptr); // Emissive
		r->PSSetShaderResources(5, 1, nullptr); // Diffuse
		r->PSSetShaderResources(6, 1, nullptr); // Specular
		r->PSSetShaderResources(7, 1, nullptr); // SSAO

		r->EndEvent();
	}
	
	r->EndEvent();
}

void LXRenderPassLighting::RenderIBL(LXRenderCommandList* r, const LXRenderPipelineDeferred* RenderPipelineDeferred)
{
	r->BeginEvent(L"IBL");

	const LXActorSceneCapture* SceneCapture = GetCore().GetProject()->GetSceneCapture() ? GetCore().GetProject()->GetSceneCapture() : nullptr;
	ConstantBufferDataIBL->AmbientIntensity = SceneCapture ? SceneCapture->GetIntensity() : 1.f;
	
	_ShaderProgramIBLLight->Render(r);
	r->PSSetConstantBuffers(0, 1, RenderPipelineDeferred->_CBViewProjection);
	r->PSSetConstantBuffers(1, 1, ConstantBufferIBL);

	LXTextureD3D11* Depth = RenderPassGBuffer->TextureDepth;
	LXTextureD3D11* Normal = RenderPassGBuffer->TextureNormal;
	LXTextureD3D11* Specular = RenderPassGBuffer->TextureSpecular;
	if (!TextureIBL && SceneCapture && SceneCapture->GetTexture())
	{
		const LXTexture* Texture = SceneCapture->GetTexture();
		TextureIBL = LXTextureD3D11::CreateFromTexture(const_cast<LXTexture*>(Texture));
	}

	r->PSSetShaderResources(0, 1, (LXTextureD3D11*)Depth);
	r->PSSetShaderResources(2, 1, (LXTextureD3D11*)Normal);
	r->PSSetShaderResources(3, 1, (LXTextureD3D11*)Specular);
	r->PSSetShaderResources(5, 1, (LXTextureD3D11*)TextureIBL);
	
	r->PSSetSamplers(0, 1, (LXTextureD3D11*)Depth);
	r->PSSetSamplers(2, 1, (LXTextureD3D11*)Normal);
	r->PSSetSamplers(3, 1, (LXTextureD3D11*)Specular);
	if (TextureIBL) r->PSSetSamplers(5, 1, (LXTextureD3D11*)TextureIBL);
	
	r->UpdateSubresource4(ConstantBufferIBL->D3D11Buffer, ConstantBufferDataIBL);
	Renderer->GetSSTriangle()->Render(r);
	
	r->PSSetShaderResources(0, 1, nullptr); // Depth
	r->PSSetShaderResources(2, 1, nullptr); // Normal
	r->PSSetShaderResources(3, 1, nullptr); // MRUL
	r->PSSetShaderResources(5, 1, nullptr); // IBL
	
	r->EndEvent();

}
void LXRenderPassLighting::RenderSpotLight(LXRenderCommandList* r, const LXRenderPipelineDeferred* RenderPipelineDeferred)
{
	r->BeginEvent(L"Spots");

	_ShaderProgramSpotLight->Render(r);
	r->PSSetConstantBuffers(0, 1, RenderPipelineDeferred->_CBViewProjection);
	r->PSSetConstantBuffers(1, 1, RenderPassShadow->ConstantBufferSpotLight.get());

	LXTextureD3D11* Depth = RenderPassGBuffer->TextureDepth;
	LXTextureD3D11* Normal = RenderPassGBuffer->TextureNormal;
	LXTextureD3D11* Specular = RenderPassGBuffer->TextureSpecular;

	r->PSSetShaderResources(0, 1, (LXTextureD3D11*)Depth);
	r->PSSetShaderResources(2, 1, (LXTextureD3D11*)Normal);
	r->PSSetShaderResources(3, 1, (LXTextureD3D11*)Specular);
	r->PSSetShaderResources(6, 1, (LXTextureD3D11*)TextureShadow);

	r->PSSetSamplers(0, 1, (LXTextureD3D11*)Depth);
	r->PSSetSamplers(2, 1, (LXTextureD3D11*)Normal);
	r->PSSetSamplers(3, 1, (LXTextureD3D11*)Specular);
	if (TextureShadow) r->PSSetSamplers(6, 1, (LXTextureD3D11*)TextureShadow);
	
	for (const LXRenderCluster* SpotLight : *_ListRenderClusterLights)
	{
		CHK(SpotLight->ConstantBufferDataSpotLight);

		SpotLight->ConstantBufferDataSpotLight->MatrixLightView = SpotLight->LightView->View;
		SpotLight->ConstantBufferDataSpotLight->MatrixLightProjection = SpotLight->LightView->Projection;
		SpotLight->ConstantBufferDataSpotLight->LightPosition = SpotLight->LightView->CameraPosition;
		
		r->UpdateSubresource4(RenderPassShadow->ConstantBufferSpotLight->D3D11Buffer, SpotLight->ConstantBufferDataSpotLight);
		Renderer->GetSSTriangle()->Render(r);
	}

	r->PSSetShaderResources(0, 1, nullptr); // Depth
	r->PSSetShaderResources(2, 1, nullptr); // Normal
	r->PSSetShaderResources(3, 1, nullptr); // Specular
	r->PSSetShaderResources(6, 1, nullptr); // Shadows
	
	r->EndEvent();
}
