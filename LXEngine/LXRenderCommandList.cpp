//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXRenderCommandList.h"
#include "LXBitmap.h"
#include "LXConsoleManager.h"
#include "LXConstantBufferD3D11.h"
#include "LXDirectX11.h"
#include "LXLogger.h"
#include "LXPrimitiveD3D11.h"
#include "LXRenderTargetViewD3D11.h"
#include "LXShaderD3D11.h"
#include "LXStatistic.h"
#include "LXTextureD3D11.h"
//#include <pix.h>
#include "LXMemory.h" // --- Must be the last included ---

namespace
{
	LXConsoleCommandT<bool> CSet_DirectMode(L"Engine.ini", L"Renderer", L"DirectMode", L"false");
};

LXRenderCommandList::LXRenderCommandList()
{
	DirectMode = CSet_DirectMode.GetValue();
}

LXRenderCommandList::~LXRenderCommandList()
{
	Empty();
}

void LXRenderCommandList::Empty()
{
	for (LXRenderCommand* Command : Commands)
	{
		delete Command;
	}

	Commands.clear();

	DrawCallCount = 0;
	TriangleCount = 0;
}

void LXRenderCommandList::Execute()
{
	LX_PERFOSCOPE(LXRenderCommandList_Execute);

	if (DirectMode)
		return;

	for (LXRenderCommand* Command : Commands)
	{
		Command->Execute(this);
	}
}

void DispatchError(HRESULT Result)
{
	if (Result == S_OK)
		return;
	
	switch (Result)
	{
		case D3D11_ERROR_FILE_NOT_FOUND: LogE(RenderCommandList, L"The file was not found.");  break;
		case D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS: LogE(RenderCommandList, L"There are too many unique instances of a particular type of state object."); break;
		case D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS: LogE(RenderCommandList, L"There are too many unique instances of a particular type of view object."); break;
		case D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD: LogE(RenderCommandList, L"The first call to ID3D11DeviceContext::Map after either ID3D11Device::CreateDeferredContext or ID3D11DeviceContext::FinishCommandList per Resource was not D3D11_MAP_WRITE_DISCARD."); break;
		case DXGI_ERROR_INVALID_CALL: LogE(RenderCommandList, L"The method call is invalid.For example, a method's parameter may not be a valid pointer."); break;
		case DXGI_ERROR_WAS_STILL_DRAWING: LogE(RenderCommandList, L"The previous blit operation that is transferring information to or from this surface is incomplete."); break;
		case E_FAIL: LogE(RenderCommandList, L"Attempted to create a device with the debug layer enabled and the layer is not installed."); break;
		case E_INVALIDARG: LogE(RenderCommandList, L"An invalid parameter was passed to the returning function."); break;
		case E_OUTOFMEMORY: LogE(RenderCommandList, L"Direct3D could not allocate sufficient memory to complete the call."); break;
		case E_NOTIMPL: LogE(RenderCommandList, L"The method call isn't implemented with the passed parameter combination."); break;
		case S_FALSE: LogE(RenderCommandList, L"Alternate success value, indicating a successful but nonstandard completion(the precise meaning depends on context)."); break;
		default: CHK(0);
	}
}

//------------------------------------------------------------------------------------------------------------------
//
// LXRenderCommandList::LXRenderCommand_XXX::Execute() definitions
//
//------------------------------------------------------------------------------------------------------------------

#define EXECUTE(name) void LXRenderCommandList::LXRenderCommand_##name::Execute(LXRenderCommandList* RCL)

//
// ID3DUserDefinedAnnotation Interface
//

EXECUTE(BeginEvent)
{
	LXDirectX11* Direct11 = LXDirectX11::GetCurrentDirectX11();
	if (Direct11->_D3DUserDefinedAnnotation)
	{
		Direct11->_D3DUserDefinedAnnotation->BeginEvent(Name);
		//PIXBeginEvent(Direct11->D3DUserDefinedAnnotation, PIX_COLOR(255, 0, 0), Name);
	}
}

EXECUTE(EndEvent)
{
	LXDirectX11* Direct11 = LXDirectX11::GetCurrentDirectX11();
	if (Direct11->_D3DUserDefinedAnnotation)
	{
		Direct11->_D3DUserDefinedAnnotation->EndEvent();
		//PIXEndEvent();
	}
}

//
// ID3D11DeviceContext Interface
// 


EXECUTE(ClearDepthStencilView)
{
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	D3D11DeviceContext->ClearDepthStencilView(DepthStencilView->D3D11DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

EXECUTE(ClearRenderTargetView)
{
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	D3D11DeviceContext->ClearRenderTargetView(RenderTargetView->D3D11RenderTargetView, ClearColor);
}

EXECUTE(ClearRenderTargetView2)
{
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	D3D11DeviceContext->ClearRenderTargetView(RenderTargetView->D3D11RenderTargetView, Color);
}

EXECUTE(IASetInputLayout)
{
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	D3D11DeviceContext->IASetInputLayout(VertexShader->D3D11VertexLayout);
}

EXECUTE(OMSetRenderTargets)
{
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	D3D11DeviceContext->OMSetRenderTargets(1, &RenderTarget, NULL);
}

EXECUTE(OMSetRenderTargets2)
{
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	D3D11DeviceContext->OMSetRenderTargets(RenderTargetView?1:0, RenderTargetView?&RenderTargetView->D3D11RenderTargetView:NULL, DepthStencilView?DepthStencilView->D3D11DepthStencilView:NULL);
}

EXECUTE(OMSetRenderTargets3)
{
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	D3D11DeviceContext->OMSetRenderTargets(NumViews, RenderTargetViews, DepthStencilView);
}

EXECUTE(OMSetBlendState)
{
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	const float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	const UINT sampleMask = 0xffffffff;
	D3D11DeviceContext->OMSetBlendState(D3D11BlendState, blendFactor, sampleMask);
}

EXECUTE(RSSetViewports)
{
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	D3D11_VIEWPORT vp;
	vp.Width = (float)Width;
	vp.Height = (float)Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	D3D11DeviceContext->RSSetViewports(1, &vp);
}

EXECUTE(RSSetViewports2)
{
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	D3D11_VIEWPORT vp;
	vp.Width = Width;
	vp.Height = Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = TopLeftX;
	vp.TopLeftY = TopLeftY;
	D3D11DeviceContext->RSSetViewports(1, &vp);
}

EXECUTE(HSSetShader)
{
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	D3D11DeviceContext->HSSetShader(HullShader?HullShader->D3D11HullShader:nullptr, nullptr, 0);
}

EXECUTE(DSSetShader)
{
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	D3D11DeviceContext->DSSetShader(DomainShader?DomainShader->D3D11DomainShader:nullptr, nullptr, 0);
}

EXECUTE(GSSetShader)
{
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	D3D11DeviceContext->GSSetShader(GeometryShader?GeometryShader->D3D11GeometryShader:nullptr, nullptr, 0);
}

EXECUTE(PSSetShader)
{
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	D3D11DeviceContext->PSSetShader(PixelShader?PixelShader->D3D11PixelShader:nullptr, nullptr, 0);
#if LX_CHECK_BINDED_OBJECT
	RCL->_PixelShader = PixelShader;
#endif
}

EXECUTE(Draw)
{
#if LX_CHECK_BINDED_OBJECT
	CHK(RCL->_VertexShader && RCL->_VertexShader->D3D11VertexShader);
	CHK(RCL->_PixelShader && RCL->_PixelShader->D3D11PixelShader && RCL->_PixelShader->GetState() == EShaderD3D11State::Ok);
#endif
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	D3D11DeviceContext->Draw(VertexCount, StartVertexLocation);
}

EXECUTE(DrawInstanced)
{
#if LX_CHECK_BINDED_OBJECT
	CHK(RCL->_VertexShader && RCL->_VertexShader->D3D11VertexShader);
	CHK(RCL->_PixelShader && RCL->_PixelShader->D3D11PixelShader);
#endif
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	D3D11DeviceContext->DrawInstanced(VertexCount, InstanceCount, StartVertexLocation, StartInstanceLocation);
}

EXECUTE(DrawIndexed)
{
#if LX_CHECK_BINDED_OBJECT
	CHK(RCL->_VertexShader && RCL->_VertexShader->D3D11VertexShader);
	CHK(RCL->_PixelShader && RCL->_PixelShader->D3D11PixelShader);
#endif
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	D3D11DeviceContext->DrawIndexed(IndexCount, 0, 0);
}

EXECUTE(DrawIndexedInstanced)
{
#if LX_CHECK_BINDED_OBJECT
	CHK(RCL->_VertexShader && RCL->_VertexShader->D3D11VertexShader);
	CHK(RCL->_PixelShader && RCL->_PixelShader->D3D11PixelShader);
#endif
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	D3D11DeviceContext->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, 0/*UINT StartInstanceLocation*/);
}

EXECUTE(VSSetShaderResources)
{
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();

	ID3D11ShaderResourceView* NullArray[1];
	NullArray[0] = NULL;

	if (Texture)
		CHK(Texture->D3D11ShaderResouceView);
		
	D3D11DeviceContext->VSSetShaderResources(StartSlot, NumViews, Texture ? &Texture->D3D11ShaderResouceView : NullArray);
}

EXECUTE(PSSetShaderResources)
{
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();

	ID3D11ShaderResourceView* NullArray[1];
	NullArray[0] = NULL;

	if (Texture)
		CHK(Texture->D3D11ShaderResouceView);

	D3D11DeviceContext->PSSetShaderResources(StartSlot, NumViews, Texture ? &Texture->D3D11ShaderResouceView : NullArray);
}

EXECUTE(PSSetShaderResources2)
{
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();

	ID3D11ShaderResourceView* NullArray[1];
	NullArray[0] = NULL;

	D3D11DeviceContext->PSSetShaderResources(StartSlot, NumViews, &Texture);
}

EXECUTE(VSSetSamplers)
{
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	D3D11DeviceContext->VSSetSamplers(StartSlot, NumSamplers, &Texture->D3D11SamplerState);
}

EXECUTE(PSSetSamplers)
{
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	D3D11DeviceContext->PSSetSamplers(StartSlot, NumSamplers, &Texture->D3D11SamplerState);
}

EXECUTE(IASetVertexBuffer)
{
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
// 	const UINT* stride = &Primitive->Stride;
// 	const UINT* offset = &Primitive->VertexBufferOffset;
// 	D3D11DeviceContext->IASetVertexBuffers(0, 1, &Primitive->VertexBuffer, stride, offset);

	UINT NumBuffers = 1;

	vector<ID3D11Buffer*> Buffers;
	vector<UINT> Strides;
	vector<UINT> Offsets;

	Buffers.push_back(Primitive->VertexBuffer);
	Strides.push_back(Primitive->VertexStride);
	Offsets.push_back(Primitive->VertexBufferOffset);

	if (Primitive->InstanceBuffer != nullptr)
	{
		NumBuffers++;
		Buffers.push_back(Primitive->InstanceBuffer);
		Strides.push_back(Primitive->InstanceBufferStride);
		Offsets.push_back(Primitive->InstanceBufferOffset);
	}

	D3D11DeviceContext->IASetVertexBuffers(0, NumBuffers, &Buffers[0], &Strides[0], &Offsets[0]);


}

EXECUTE(IASetIndexBuffer)
{
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
#ifdef INDEXTYPE_USHORT
	D3D11DeviceContext->IASetIndexBuffer(Primitive->IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
#else
	D3D11DeviceContext->IASetIndexBuffer(Primitive->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
#endif
}

EXECUTE(IASetPrimitiveTopology)
{
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	D3D11DeviceContext->IASetPrimitiveTopology((D3D_PRIMITIVE_TOPOLOGY)PrimitiveTopology);
}

EXECUTE(UpdateSubresource)
{
#if LX_USE_D3D11_1
	if (NoOverwriteConstantBuffer)
	{
		ID3D11DeviceContext1* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext1();

//		D3D11DeviceContext->UpdateSubresource1(D3D11Buffer, 0, nullptr, Primitive->ConstantBufferData, 0, 0, D3D11_COPY_DISCARD/*D3D11_COPY_NO_OVERWRITE*/);
	
		/*
		D3D11_MAPPED_SUBRESOURCE MappedResource;
		ZeroMemory(&MappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

		D3D11_MAP Mapping;

		if (Offset == 0)
			Mapping = D3D11_MAP_WRITE_DISCARD;
		else
			Mapping = D3D11_MAP_WRITE_NO_OVERWRITE;

		if (S_OK == D3D11DeviceContext->Map(D3D11Buffer, 0, Mapping, 0, &MappedResource))
		{
			memcpy(MappedResource.pData, Primitive->ConstantBufferData + Offset, sizeof(LXConstantBufferData));
			D3D11DeviceContext->Unmap(D3D11Buffer, 0);
			Offset += sizeof(LXConstantBufferData);
		}
		*/
	}
	else
#endif
	{
		ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
		//D3D11DeviceContext->UpdateSubresource(D3D11Buffer, 0, nullptr, Primitive->ConstantBufferData, 0, 0);
	}
}

EXECUTE(UpdateSubresource2)
{
#if LX_USE_D3D11_1
	if (NoOverwriteConstantBuffer)
	{
// 		ID3D11DeviceContext1* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext1();
// 		//	D3D11DeviceContext->UpdateSubresource1(D3D11Buffer, 0, nullptr, ConstantBufferData, 0, 0, /*D3D11_COPY_NO_OVERWRITE*/);
// 
// 		D3D11_MAPPED_SUBRESOURCE MappedResource;
// 		ZeroMemory(&MappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
// 
// 		D3D11_MAP Mapping;
// 
// 		if (Offset == 0)
// 			Mapping = D3D11_MAP_WRITE_DISCARD;
// 		else
// 			Mapping = D3D11_MAP_WRITE_NO_OVERWRITE;
// 
// 		if (S_OK == D3D11DeviceContext->Map(D3D11Buffer, 0, Mapping, 0, &MappedResource))
// 		{
// 			memcpy(MappedResource.pData, ConstantBufferData + Offset, sizeof(LXConstantBufferData));
// 			D3D11DeviceContext->Unmap(D3D11Buffer, 0);
// 			Offset += sizeof(LXConstantBufferData);
// 		}
	}
	else
#endif
	{
		ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
		D3D11DeviceContext->UpdateSubresource(D3D11Buffer, 0, nullptr, ConstantBufferData, 0, 0);
	}
}

EXECUTE(UpdateSubresource3)
{
#if LX_USE_D3D11_1
	if (NoOverwriteConstantBuffer)
	{
// 		ID3D11DeviceContext1* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext1();
// 		//	D3D11DeviceContext->UpdateSubresource1(D3D11Buffer, 0, nullptr, ConstantBufferData, 0, 0, /*D3D11_COPY_NO_OVERWRITE*/);
// 
// 		D3D11_MAPPED_SUBRESOURCE MappedResource;
// 		ZeroMemory(&MappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
// 
// 		D3D11_MAP Mapping;
// 
// 		if (Offset == 0)
// 			Mapping = D3D11_MAP_WRITE_DISCARD;
// 		else
// 			Mapping = D3D11_MAP_WRITE_NO_OVERWRITE;
// 
// 		if (S_OK == D3D11DeviceContext->Map(ConstantBuffer->D3D11Buffer, 0, Mapping, 0, &MappedResource))
// 		{
// 			memcpy(MappedResource.pData, ConstantBufferData + Offset, sizeof(LXConstantBufferData));
// 			D3D11DeviceContext->Unmap(ConstantBuffer->D3D11Buffer, 0);
// 			Offset += sizeof(LXConstantBufferData);
// 		}
	}
	else
#endif
	{
		ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
		D3D11DeviceContext->UpdateSubresource(ConstantBuffer->D3D11Buffer, 0, nullptr, ConstantBufferData, 0, 0);
	}
}

EXECUTE(UpdateSubresource4)
{
#if LX_USE_D3D11_1
	if (NoOverwriteConstantBuffer)
	{
		// 		ID3D11DeviceContext1* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext1();
		// 		//	D3D11DeviceContext->UpdateSubresource1(D3D11Buffer, 0, nullptr, ConstantBufferData, 0, 0, /*D3D11_COPY_NO_OVERWRITE*/);
		// 
		// 		D3D11_MAPPED_SUBRESOURCE MappedResource;
		// 		ZeroMemory(&MappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		// 
		// 		D3D11_MAP Mapping;
		// 
		// 		if (Offset == 0)
		// 			Mapping = D3D11_MAP_WRITE_DISCARD;
		// 		else
		// 			Mapping = D3D11_MAP_WRITE_NO_OVERWRITE;
		// 
		// 		if (S_OK == D3D11DeviceContext->Map(ConstantBuffer->D3D11Buffer, 0, Mapping, 0, &MappedResource))
		// 		{
		// 			memcpy(MappedResource.pData, ConstantBufferData + Offset, sizeof(LXConstantBufferData));
		// 			D3D11DeviceContext->Unmap(ConstantBuffer->D3D11Buffer, 0);
		// 			Offset += sizeof(LXConstantBufferData);
		// 		}
	}
	else
#endif
	{
		ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
		D3D11DeviceContext->UpdateSubresource(D3D11Buffer, 0, nullptr, ConstantBufferData, 0, 0);
	}
}

EXECUTE(VSSetConstantBuffers)
{
//#if LX_USE_D3D11_1
//	ID3D11DeviceContext1* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext1();
//	D3D11DeviceContext->VSSetConstantBuffers1(StartSlot, NumBuffers, &VertexShader->ConstantBuffer,0,0);
//#else
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	D3D11DeviceContext->VSSetConstantBuffers(StartSlot, NumBuffers, &ConstantBuffer->D3D11Buffer);
//#endif

}

EXECUTE(PSSetConstantBuffers)
{
// #if LX_USE_D3D11_1
// 	ID3D11DeviceContext1* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext1();
// 	D3D11DeviceContext->PSSetConstantBuffers1(StartSlot, NumBuffers, &PixelShader->ConstantBuffer,0,0);
// #else
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	D3D11DeviceContext->PSSetConstantBuffers(StartSlot, NumBuffers, &ConstantBuffer->D3D11Buffer);
//#endif
}

EXECUTE(Present)
{
	LXDirectX11::GetCurrentDirectX11()->Present();
}

EXECUTE(VSSetShader)
{
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	D3D11DeviceContext->VSSetShader(VertexShader?VertexShader->D3D11VertexShader:nullptr, nullptr, 0);
#if LX_CHECK_BINDED_OBJECT
	RCL->_VertexShader = VertexShader;
#endif
}

EXECUTE(RSSetState)
{
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	D3D11DeviceContext->RSSetState(RasterizerState);
}

EXECUTE(Map)
{
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	auto Subresource = D3D11CalcSubresource(0, 0, 1);
	HRESULT Result = D3D11DeviceContext->Map(Resource, Subresource, D3D11_MAP_READ, 0, MappedResource);
	DispatchError(Result);
}

EXECUTE(Unmap)
{
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	auto Subresource = D3D11CalcSubresource(0, 0, 1);
	D3D11DeviceContext->Unmap(Resource, Subresource);
}

EXECUTE(CopyResource)
{
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	D3D11DeviceContext->CopyResource(DstResource, SrcResource);
}

EXECUTE(GenerateMips)
{
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	D3D11DeviceContext->GenerateMips(pShaderResourceView);
}

//
// Advanced commands (multiple Direct3D commands)
//

EXECUTE(CopyResourceToBitmap)
{
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	auto Subresource = D3D11CalcSubresource(0, 0, 1);

	D3D11_MAPPED_SUBRESOURCE MappedResource;

	HRESULT Result = D3D11DeviceContext->Map(SrcResource, Subresource, D3D11_MAP_READ, 0, &MappedResource);
	DispatchError(Result);

	const unsigned short* Source = static_cast<const unsigned short*>(MappedResource.pData);
	void *Dest = DstBitmap->GetPixels();
	int	size = 4096 * 4096 * 2;
	memcpy(Dest, Source, size);

	DstBitmap->InvokeOnBitmapChanged();

	D3D11DeviceContext->Unmap(SrcResource, Subresource);
}