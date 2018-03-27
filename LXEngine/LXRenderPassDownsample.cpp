//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXRenderPassDownsample.h"
#include "LXRenderer.h"
#include "LXRenderPassToneMapping.h"
#include "LXRenderPipeline.h"
#include "LXRenderTarget.h"
#include "LXRenderCommandList.h"
#include "LXShaderD3D11.h"
#include "LXInputElementDescD3D11Factory.h"
#include "LXSettings.h"
#include "LXMemory.h" // --- Must be the last included ---

const wchar_t szDownSampleShader[] = L"Downsample.hlsl";

LXRenderPassDownsample::LXRenderPassDownsample(LXRenderer* InRenderer, EDownsampleFunction DownSampleFunction):LXRenderPass(InRenderer)
{
	uint HalfWidth = Renderer->Width;
	uint HalfHeight = Renderer->Height;

	if (DownSampleFunction == EDownsampleFunction::Downsample_HalfRes)
	{ 
		HalfWidth /= 2;
		HalfHeight /= 2;
		_RenderTargerts.push_back(new LXRenderTarget(HalfWidth, HalfHeight, DXGI_FORMAT_R16G16B16A16_FLOAT));
	}
	else
	{
		while (1)
		{
			HalfWidth /= 2;
			HalfHeight /= 2;

			if (HalfWidth < 1 || HalfHeight < 1)
			{
				break;
			}

			_RenderTargerts.push_back(new LXRenderTarget(HalfWidth, HalfHeight, DXGI_FORMAT_R16G16B16A16_FLOAT));
		}
	}

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
	for (LXRenderTarget* RenderTarget : _RenderTargerts)
	{
		RenderTarget->CreateBuffers(Width, Height, DXGI_FORMAT_R16G16B16A16_FLOAT);
	}
}

void LXRenderPassDownsample::DeleteBuffers()
{
	for (LXRenderTarget* RenderTarget : _RenderTargerts)
	{
		RenderTarget->DeleteBuffers();
	}
}

void LXRenderPassDownsample::Render(LXRenderCommandList* r)
{
	r->BeginEvent(L"Downsample");
	
	for (int i=0; i<_RenderTargerts.size(); i++)
	{
		LXString EventName = L"Downsample" + LXString::Number(i);
		r->BeginEvent(EventName.GetBuffer());
		
		LXRenderTarget* RenderTarget = _RenderTargerts[i];
		r->OMSetRenderTargets2(RenderTarget->_RenderTargetViewD3D11, nullptr);
		const LXTextureD3D11* Texture;
		if (i == 0)
		{
			const LXRenderPass* RenderPass = Renderer->GetRenderPipeline()->GetPreviousRenderPass();
			Texture = RenderPass->GetOutputTexture();
			CHK(Texture);
		}
		else
		{
			Texture = _RenderTargerts[i - 1]->_TextureD3D11;
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
		r->EndEvent();
	}
	r->EndEvent();
}

void LXRenderPassDownsample::Resize(uint Width, uint Height)
{
	CreateBuffers(Width, Height);
}
