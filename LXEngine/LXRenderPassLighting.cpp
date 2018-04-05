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

LXRenderPassLighting::LXRenderPassLighting(LXRenderer* InRenderer) :LXRenderPass(InRenderer)
{
	// Textures and Samplers
	CreateBuffers(Renderer->Width, Renderer->Height);
}

LXRenderPassLighting::~LXRenderPassLighting()
{
	DeleteBuffers();
	delete TextureIBL;
}

void LXRenderPassLighting::CreateBuffers(uint Width, uint Height)
{
	DeleteBuffers();
	TextureColor = new LXTextureD3D11(Width, Height, DXGI_FORMAT_R16G16B16A16_FLOAT);

	// RenderTargetViews and ShaderResourceViews (in TextureD3D11 objects)
	RenderTargetColor = new LXRenderTargetViewD3D11(TextureColor);
}

void LXRenderPassLighting::DeleteBuffers()
{
	LX_SAFE_DELETE(RenderTargetColor);
	LX_SAFE_DELETE(TextureColor);
}

void LXRenderPassLighting::Resize(uint Width, uint Height)
{
	CreateBuffers(Width, Height);
}

bool LXRenderPassLighting::IsValid() const
{
	#pragma message("TODO LXRenderPassLighting::IsValid()")
	return true;
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
	
	RenderPassShadow->ConstantBufferDataSpotLight.AmbientIntensity = SceneCapture ? SceneCapture->GetIntensity() : 1.f ;
	r->UpdateSubresource4(RenderPassShadow->ConstantBufferSpotLight->D3D11Buffer, &RenderPassShadow->ConstantBufferDataSpotLight);
	
	r->OMSetRenderTargets2(RenderTargetColor, nullptr);
	r->RSSetViewports(Renderer->Width, Renderer->Height);

	// Render a triangle : GBuffer TextureColor
	r->IASetInputLayout(Renderer->GetShaderManager()->VSLighting);
	r->VSSetShader(Renderer->GetShaderManager()->VSLighting);
	r->PSSetShader(Renderer->GetShaderManager()->PSLighting);
	r->PSSetConstantBuffers(0, 1, RenderPipelineDeferred->_CBViewProjection);
	r->PSSetConstantBuffers(1, 1, RenderPassShadow->ConstantBufferSpotLight.get());

	LXTextureD3D11* Depth = RenderPassGBuffer->TextureDepth;
	LXTextureD3D11* Color = RenderPassGBuffer->TextureColor;
	LXTextureD3D11* Normal = RenderPassGBuffer->TextureNormal;
	LXTextureD3D11* Specular = RenderPassGBuffer->TextureSpecular;
	LXTextureD3D11* Emissive = RenderPassGBuffer->TextureEmissive;
	const LXTextureD3D11* TextureSSAO = RenderPassSSAO->GetOutputTexture();
	if (!TextureIBL && SceneCapture && SceneCapture->GetTexture())
	{
		const LXTexture* Texture = SceneCapture->GetTexture();
		TextureIBL = LXTextureD3D11::CreateFromTexture(const_cast<LXTexture*>(Texture));
	}
		
	r->PSSetShaderResources(0, 1, (LXTextureD3D11*)Depth);
	r->PSSetShaderResources(1, 1, (LXTextureD3D11*)Color);
	r->PSSetShaderResources(2, 1, (LXTextureD3D11*)Normal);
	r->PSSetShaderResources(3, 1, (LXTextureD3D11*)Specular);
	r->PSSetShaderResources(4, 1, (LXTextureD3D11*)Emissive);
	r->PSSetShaderResources(5, 1, (LXTextureD3D11*)TextureIBL);
	r->PSSetShaderResources(6, 1, (LXTextureD3D11*)TextureShadow);
	r->PSSetShaderResources(7, 1, (LXTextureD3D11*)TextureSSAO);
			
	r->PSSetSamplers(0, 1, (LXTextureD3D11*)Depth);
	r->PSSetSamplers(1, 1, (LXTextureD3D11*)Color);
	r->PSSetSamplers(2, 1, (LXTextureD3D11*)Normal);
	r->PSSetSamplers(3, 1, (LXTextureD3D11*)Specular);
	r->PSSetSamplers(4, 1, (LXTextureD3D11*)Emissive);
	if (TextureIBL) r->PSSetSamplers(5, 1, (LXTextureD3D11*)TextureIBL);
	if (TextureShadow) r->PSSetSamplers(6, 1, (LXTextureD3D11*)TextureShadow);
	r->PSSetSamplers(7, 1, (LXTextureD3D11*)Emissive);

	Renderer->GetSSTriangle()->Render(r);
	
	r->PSSetShaderResources(0, 1, nullptr); // Depth
	r->PSSetShaderResources(1, 1, nullptr); // Color
	r->PSSetShaderResources(2, 1, nullptr); // Normal
	r->PSSetShaderResources(3, 1, nullptr); // Specular
	r->PSSetShaderResources(4, 1, nullptr); // Emissive
	r->PSSetShaderResources(5, 1, nullptr); // Reflection
	r->PSSetShaderResources(6, 1, nullptr); // Shadows
	r->PSSetShaderResources(7, 1, nullptr); // SSAO
		
	r->EndEvent();
}
