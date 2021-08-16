//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXRenderPassDepth.h"
#include "LXRenderCluster.h"
#include "LXRenderCommandList.h"
#include "LXRenderer.h"
#include "LXRenderPipelineDeferred.h"
#include "LXStatistic.h"
#include "LXThreadManager.h"

LXConsoleCommandT<bool> CSet_UseDepthPass(L"Engine.ini", L"Renderer", L"DepthPass", L"false");

LXRenderPassDepth::LXRenderPassDepth(LXRenderer* renderer):LXRenderPass(renderer)
{
}

void LXRenderPassDepth::Render(LXRenderCommandList* rcl)
{
	if (!RenderClusters || !RenderClusters->size() || !CSet_UseDepthPass.GetValue())
		return;

	LX_PERFOSCOPE(RenderThread_RenderPassDepth);

	LXRenderPipelineDeferred* renderPipelineDeferred = dynamic_cast<LXRenderPipelineDeferred*>(Renderer->GetRenderPipeline());
	CHK(renderPipelineDeferred);

	const LXDepthStencilViewD3D11* depthStencilView = renderPipelineDeferred->GetDepthStencilView();

	rcl->BeginEvent(L"DepthPass");
	rcl->OMSetRenderTargets2(nullptr, depthStencilView);
	rcl->RSSetViewports(Renderer->Width, Renderer->Height);
	rcl->ClearDepthStencilView(depthStencilView);

	// ConstantBuffers per Frame (View, Projection)
	rcl->CBViewProjection = renderPipelineDeferred->GetCBViewProjection();
	rcl->UpdateSubresource4(renderPipelineDeferred->GetCBViewProjection()->D3D11Buffer, &renderPipelineDeferred->GetCBViewProjectionData());
	for (LXRenderCluster* renderCluster : *RenderClusters)
	{
		renderCluster->Render(ERenderPass::Depth, rcl);
	}

	// Reset...
	rcl->VSSetShaderResources(1, 1, nullptr);
	rcl->VSSetShader(nullptr);
	rcl->HSSetShader(nullptr);
	rcl->DSSetShader(nullptr);
	rcl->GSSetShader(nullptr);
	rcl->PSSetShader(nullptr);
	rcl->RSSetState(Renderer->GetDefaultRasterizerState());

	rcl->EndEvent();

}