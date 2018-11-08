//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXMath.h"
#include "LXRenderPassDownsample.h"
#include "LXRenderer.h"
#include "LXRenderPassToneMapping.h"
#include "LXRenderPipeline.h"
#include "LXRenderTarget.h"
#include "LXRenderCommandList.h"
#include "LXShaderD3D11.h"
#include "LXTextureD3D11.h"
#include "LXInputElementDescD3D11Factory.h"
#include "LXSettings.h"
#include "LXRenderPipelineDeferred.h"
#include "LXRenderPassLighting.h"
#include "LXMemory.h" // --- Must be the last included ---

namespace
{
	const wchar_t szDownSampleShader[] = L"Downsample.hlsl";
}

LXRenderPassDownsample::LXRenderPassDownsample(LXRenderer* InRenderer, EDownsampleFunction DownSampleFunction):LXRenderPass(InRenderer)
{
	for (int i = 0; i < maxDownSample; i++)
	{
		_RenderTargerts[i] = nullptr;
	}

	CreateBuffers(Renderer->Width, Renderer->Height);

	_VertexShader = new LXShaderD3D11();
	_PixelShader = new LXShaderD3D11();
 
	const LXArrayInputElementDesc& Layout = GetInputElementDescD3D11Factory().GetInputElement_PT();
	
	_VertexShader->CreateVertexShader(GetSettings().GetShadersFolder() + szDownSampleShader, &Layout[0], (uint)Layout.size());
	_PixelShader->CreatePixelShader(GetSettings().GetShadersFolder() + szDownSampleShader);
}

LXRenderPassDownsample::~LXRenderPassDownsample()
{
	for (LXRenderTarget* RenderTarget : _RenderTargerts)
	{
		delete RenderTarget;
	}
	
	delete _VertexShader;
	delete _PixelShader;
}

bool LXRenderPassDownsample::IsValid() const
{
	return _VertexShader->IsValid() && _PixelShader->IsValid();
}

void LXRenderPassDownsample::CreateBuffers(uint Width, uint Height)
{
	DeleteBuffers();
	
	uint HalfWidth = Width;
	uint HalfHeight = Height;

	LXRenderTarget* smallestRenderTarget = nullptr;
		
	for (int i=0; i<maxDownSample; i++ )
	{
		HalfWidth /= 2;
		HalfHeight /= 2;

		if (HalfWidth < 1 && HalfHeight < 1)
		{
			_RenderTargerts[i] = nullptr;
		}
		else
		{
			HalfWidth = LXMax(1, HalfWidth);
			HalfHeight = LXMax(1, HalfHeight);
			_RenderTargerts[i] = new LXRenderTarget(HalfWidth, HalfHeight, DXGI_FORMAT_R16G16B16A16_FLOAT);
			smallestRenderTarget = _RenderTargerts[i];
		}

		LXRenderPipeline* RenderPipeline = Renderer->GetRenderPipeline();
		const LXString bufferName = L"View.DownSample_" + LXString::Number(i);
		const LXString commandName = bufferName + L"(" + LXString::Number((int)HalfWidth) + "x" + LXString::Number((int)HalfHeight) + L")";
		RenderPipeline->AddToViewDebugger(bufferName, commandName, _RenderTargerts[i]?_RenderTargerts[i]->TextureD3D11:nullptr, ETextureChannel::ChannelRGB);
	}

	CHK(smallestRenderTarget->Width == 1 && smallestRenderTarget->Height == 1)
}

void LXRenderPassDownsample::DeleteBuffers()
{
	for (int i = 0; i<maxDownSample; i++)
	{
		LX_SAFE_DELETE(_RenderTargerts[i]);
		_RenderTargerts[i] = nullptr;
	}
}

void LXRenderPassDownsample::Render(LXRenderCommandList* r)
{
	r->BeginEvent(L"Downsample");

	LXRenderPipelineDeferred* RenderPipelineDeferred = dynamic_cast<LXRenderPipelineDeferred*>(Renderer->GetRenderPipeline());
	CHK(RenderPipelineDeferred);
	
	for (int i=0; i<maxDownSample; i++)
	{
		LXString EventName = L"Downsample_" + LXString::Number(i);
		r->BeginEvent(EventName.GetBuffer());
		
		LXRenderTarget* RenderTarget = _RenderTargerts[i];

		if (RenderTarget == nullptr)
			continue;

		r->OMSetRenderTargets2(RenderTarget->RenderTargetViewD3D11, nullptr);
		r->RSSetViewports(RenderTarget->Width, RenderTarget->Height);

		const LXTextureD3D11* Texture;
		if (i == 0)
		{
			Texture = RenderPipelineDeferred->RenderPassLighting->GetOutputTexture();
		}
		else
		{
			Texture = _RenderTargerts[i - 1]->TextureD3D11;
		}

		r->IASetInputLayout(_VertexShader);
		r->VSSetShader(_VertexShader);
		r->PSSetShader(_PixelShader);
		r->PSSetShaderResources(0, 1, Texture);
		r->PSSetSamplers(0, 1, Texture);
		Renderer->DrawScreenSpacePrimitive(r);
		r->PSSetShaderResources(0, 1, nullptr);
		r->VSSetShader(nullptr);
		r->PSSetShader(nullptr);
		r->RSSetViewports(Renderer->Width, Renderer->Height);
		r->EndEvent();
	}

	r->EndEvent();
}

void LXRenderPassDownsample::Resize(uint Width, uint Height)
{
	CreateBuffers(Width, Height);
}

