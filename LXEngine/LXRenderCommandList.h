//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include "LXDelegate.h"
#include "LXRenderCommandDX11.h"
#include "LXMatrix.h"

// Check if the needed objects for the current operation are correctly binded.
// Ex: check the PS and VS status before calling Draw...
#define LX_CHECK_BINDED_OBJECT 1

class LXRenderer;
class LXShaderManager;
class LXRenderCommand;
class LXTextureD3D11;
class LXDepthStencilViewD3D11;
class LXShaderD3D11;
class LXRenderTargetViewD3D11;
class LXPrimitiveD3D11;
class LXConstantBufferData;
class LXConstantBufferD3D11;

struct D3D11_MAPPED_SUBRESOURCE;
struct ID3D11BlendState;
struct ID3D11Buffer;
struct ID3D11DepthStencilView;
struct ID3D11RasterizerState;
struct ID3D11RenderTargetView;
struct ID3D11Resource;
struct ID3D11SamplerState;
struct ID3D11ShaderResourceView;

#define CLASSRENDERCOMMAND0(name) class LXRenderCommand_##name : public LXRenderCommandDX11 { public : LXRenderCommand_##name(){} void Execute(LXRenderCommandList*) override; };
#define CLASSRENDERCOMMAND1(name, type0, var0) class LXRenderCommand_##name : public LXRenderCommandDX11 { public : LXRenderCommand_##name(type0 In0):var0(In0){} void Execute(LXRenderCommandList*) override; type0 var0; };
#define CLASSRENDERCOMMAND2(name, type0, var0, type1, var1)	class LXRenderCommand_##name : public LXRenderCommandDX11 {	public : LXRenderCommand_##name(type0 In0, type1 In1):var0(In0), var1(In1){} void Execute(LXRenderCommandList*) override; type0 var0; type1 var1; };
#define CLASSRENDERCOMMAND3(name, type0, var0, type1, var1, type2, var2) class LXRenderCommand_##name : public LXRenderCommandDX11 { public : LXRenderCommand_##name(type0 In0, type1 In1, type2 In2):var0(In0), var1(In1), var2(In2){} void Execute(LXRenderCommandList*) override; type0 var0; type1 var1; type2 var2; };
#define CLASSRENDERCOMMAND4(name, type0, var0, type1, var1, type2, var2, type3, var3) class LXRenderCommand_##name : public LXRenderCommandDX11 { public : LXRenderCommand_##name(type0 In0, type1 In1, type2 In2, type3 In3):var0(In0), var1(In1), var2(In2), var3(In3){} void Execute(LXRenderCommandList*) override; type0 var0; type1 var1; type2 var2; type3 var3; };

#define CMD0_CLASS(name) \
CLASSRENDERCOMMAND0(name) \
void name() { Commands.push_back(new LXRenderCommand_##name()); if(DirectMode) Commands.back()->Execute(this); } 

#define CMD1_CLASS(name, type0, var0) \
CLASSRENDERCOMMAND1(name, type0, var0) \
void name(type0 var0) { Commands.push_back(new LXRenderCommand_##name(var0)); if(DirectMode) Commands.back()->Execute(this);} 

#define CMD1_CLASS_CR(name, type0, var0) \
CLASSRENDERCOMMAND1(name, type0, var0) \
void name(type0 var0) { Commands.push_back(new LXRenderCommand_##name(var0)); Current##var0 = var0; if(DirectMode) Commands.back()->Execute(this); } 

#define CMD2_CLASS(name, type0, var0, type1, var1) \
CLASSRENDERCOMMAND2(name, type0, var0, type1, var1) \
void name(type0 var0, type1 var1) { Commands.push_back(new LXRenderCommand_##name(var0, var1)); if(DirectMode) Commands.back()->Execute(this); } 

#define CMD3_CLASS(name, type0, var0, type1, var1, type2, var2) \
CLASSRENDERCOMMAND3(name, type0, var0, type1, var1, type2, var2) \
void name(type0 var0, type1 var1, type2 var2) { Commands.push_back(new LXRenderCommand_##name(var0, var1, var2)); if(DirectMode) Commands.back()->Execute(this);}

#define CMD4_CLASS(name, type0, var0, type1, var1, type2, var2, type3, var3) \
CLASSRENDERCOMMAND4(name, type0, var0, type1, var1, type2, var2, type3, var3) \
void name(type0 var0, type1 var1, type2 var2, type3 var3) { Commands.push_back(new LXRenderCommand_##name(var0, var1, var2, var3)); if(DirectMode) Commands.back()->Execute(this);}																									

class LXRenderCommandList :	public LXObject
{

public:

	LXRenderCommandList();
	virtual ~LXRenderCommandList();

	void Empty();
	void Execute();

	//
	// ID3DUserDefinedAnnotation Interface
	//

	CMD1_CLASS(BeginEvent, const wchar_t*, Name)
	CMD0_CLASS(EndEvent)
	
	//
	// Standard commands (Direct3D API functions)
	//

	CMD1_CLASS_CR(VSSetShader, LXShaderD3D11*, VertexShader)
	CMD1_CLASS(HSSetShader, LXShaderD3D11*, HullShader)
	CMD1_CLASS(DSSetShader, LXShaderD3D11*, DomainShader)
	CMD1_CLASS(GSSetShader, LXShaderD3D11*, GeometryShader)
	CMD1_CLASS(PSSetShader, LXShaderD3D11*, PixelShader)
	CMD2_CLASS(Draw, UINT, VertexCount, UINT, StartVertexLocation)
	CMD4_CLASS(DrawInstanced, UINT, VertexCount, UINT, InstanceCount, UINT, StartVertexLocation, UINT, StartInstanceLocation)
	CMD1_CLASS(DrawIndexed, UINT, IndexCount)
	CMD4_CLASS(DrawIndexedInstanced, UINT, IndexCountPerInstance, UINT, InstanceCount, UINT, StartIndexLocation, INT,  BaseVertexLocation)
	CMD2_CLASS(RSSetViewports, UINT, Width, UINT, Height)
	CMD4_CLASS(RSSetViewports2, float, TopLeftX, float, TopLeftY, float, Width, float, Height)
	CMD3_CLASS(VSSetShaderResources, UINT, StartSlot, UINT, NumViews, const LXTextureD3D11*, Texture)
	CMD3_CLASS(PSSetShaderResources, UINT, StartSlot, UINT, NumViews, const LXTextureD3D11*, Texture)
	CMD3_CLASS(PSSetShaderResources2, UINT, StartSlot, UINT, NumViews, ID3D11ShaderResourceView*, Texture)
	CMD3_CLASS(PSSetSamplers, UINT, StartSlot, UINT, NumSamplers, ID3D11SamplerState*, Sampler)
	CMD3_CLASS(VSSetSamplers, UINT, StartSlot, UINT, NumSamplers, ID3D11SamplerState*, Sampler)
	CMD1_CLASS(IASetPrimitiveTopology, UINT, PrimitiveTopology)
	CMD1_CLASS(IASetVertexBuffer, LXPrimitiveD3D11*, Primitive)
	CMD1_CLASS(IASetIndexBuffer, LXPrimitiveD3D11*, Primitive)
	CMD2_CLASS(UpdateSubresource, ID3D11Buffer*, D3D11Buffer, LXPrimitiveD3D11*, Primitive)
	CMD2_CLASS(UpdateSubresource2, ID3D11Buffer*, D3D11Buffer, LXConstantBufferData*, ConstantBufferData)
	CMD2_CLASS(UpdateSubresource3, LXConstantBufferD3D11*, ConstantBuffer, LXConstantBufferData*, ConstantBufferData)
	CMD2_CLASS(UpdateSubresource4, ID3D11Buffer*, D3D11Buffer, const void*, ConstantBufferData)
	CMD3_CLASS(VSSetConstantBuffers, UINT, StartSlot, UINT, NumBuffers, const LXConstantBufferD3D11*, ConstantBuffer)
	CMD3_CLASS(PSSetConstantBuffers, UINT, StartSlot, UINT, NumBuffers, const LXConstantBufferD3D11*, ConstantBuffer)
	CMD1_CLASS(ClearDepthStencilView, const LXDepthStencilViewD3D11*, DepthStencilView)
	CMD1_CLASS(ClearRenderTargetView, LXRenderTargetViewD3D11*, RenderTargetView)
	CMD2_CLASS(ClearRenderTargetView2, LXRenderTargetViewD3D11*, RenderTargetView, vec4f, Color)
	CMD1_CLASS(ClearRenderTargetView3, ID3D11RenderTargetView*, RenderTargetView)
	CMD1_CLASS(IASetInputLayout, LXShaderD3D11*, VertexShader)
	CMD1_CLASS(OMSetRenderTargets, ID3D11RenderTargetView*, RenderTarget)
	CMD2_CLASS(OMSetRenderTargets2, LXRenderTargetViewD3D11*, RenderTargetView, const LXDepthStencilViewD3D11*, DepthStencilView)
	CMD3_CLASS(OMSetRenderTargets3, UINT, NumViews, ID3D11RenderTargetView**, RenderTargetViews, ID3D11DepthStencilView*, DepthStencilView)
	CMD1_CLASS(OMSetBlendState, ID3D11BlendState*, D3D11BlendState);
	CMD1_CLASS(RSSetState, ID3D11RasterizerState*, RasterizerState)
	CMD2_CLASS(Map, ID3D11Resource*, Resource, D3D11_MAPPED_SUBRESOURCE*, MappedResource)
	CMD1_CLASS(Unmap, ID3D11Resource*, Resource)
	CMD2_CLASS(CopyResource, ID3D11Resource*, DstResource, ID3D11Resource*, SrcResource)
	CMD1_CLASS(GenerateMips, ID3D11ShaderResourceView*, pShaderResourceView)

	//
	// Standard commands (IDXGISwapChain Interface)
	//

	CMD0_CLASS(Present)

	//
	// Engine commands 
	//

	// Enqueue the delegate invocation on MainThread
	CMD1_CLASS(EnqueueInvokeDelegate, LXDelegate<>*, delegate)

	// Invoke the delegate 'immediately' on RenderThread
	CMD1_CLASS(InvokeDelegate, LXDelegate<>*, delegate)

public:

	// Current objects
	LXShaderD3D11* CurrentVertexShader = nullptr;

	// Frame ConstantBuffers
	const LXConstantBufferD3D11* CBViewProjection = nullptr;
	std::vector<LXRenderCommand*> Commands;

	//Stats
	UINT DrawCallCount;
	UINT TriangleCount;

	//Refs
	LXRenderer* Renderer;
	LXShaderManager* ShaderManager;

	//Debug purpose. Default value is false. 
	//Set true to immediately call the command.
	bool DirectMode = false;

#if LX_CHECK_BINDED_OBJECT

	LXShaderD3D11* _VertexShader = nullptr;
	LXShaderD3D11* _PixelShader = nullptr;

#endif
	
};

