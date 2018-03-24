//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2016 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXRenderPass.h"

class LXTexture;
class LXTextureD3D11;
class LXMaterialD3D11;
class LXGraphNode;
class LXConstantBufferD3D11;
class LXRenderClusterJob;

class LXRenderPassDynamicTexture : public LXRenderPass
{

public:

	LXRenderPassDynamicTexture(LXRenderer* InRenderer);
	virtual ~LXRenderPassDynamicTexture();
	void Render(LXRenderCommandList* RenderCommandList) override;
	void Resize(uint Width, uint Height) override;

	LXTextureD3D11* AddGraph(LXTexture* Texture);

	static LXMaterialD3D11* GetMaterialD3D11(LXRenderer* Renderer, LXMaterial* InMaterial);
	//LXTextureD3D11* GetTexture() const;
		
private:

	LXRenderClusterJob* TraverseNode(LXGraphNode* LinkedNode, LXRenderClusterJob* LinkedJob, LXGraphNode* Node);

public:

	// Basic Objects
	LXConstantBufferD3D11* CBTime;

	LXTexture* Texture;
	LXTextureD3D11* TextureD3D11;

private:

	list<LXRenderClusterJob*> ListRenderClusters;
	map<LXGraphNode*, LXRenderClusterJob*> MapRenderClusters;

	// Run
	list<LXRenderClusterJob*>::iterator ItStart;

};

class LXRenderClusterJob
{
public:
	LXRenderClusterJob() {}
	virtual ~LXRenderClusterJob() {};
	virtual void Render(LXRenderCommandList* RCL, LXConstantBufferD3D11* ConstantBuffer) = 0;
	int Iteration = 1;
	int Index = 0;
};

// class LXRenderClusterJobLoop : public LXRenderClusterJob
// {
// public:
// 	LXRenderClusterJobLoop() {}
// 	virtual ~LXRenderClusterJobLoop() {};
// 	virtual void Render(LXRenderCommandList* RCL, LXConstantBufferD3D11* ConstantBuffer) override;
// 	int Start = 0;
// 	int End = 1000;
// 	int Index = 0;
// 	virtual LXRenderClusterJob* GetNextJob() override;
// };

class LXRenderClusterJobTexture : public LXRenderClusterJob
{
public:
	LXRenderClusterJobTexture(LXRenderer* Renderer, LXMaterial* InMaterial);
	~LXRenderClusterJobTexture();
	void Render(LXRenderCommandList* RCL, LXConstantBufferD3D11* ConstantBuffer) override;
	LXShaderD3D11* VertexShader = nullptr;
	LXShaderD3D11* PixelShader = nullptr;
	LXMaterialD3D11* Material = nullptr;
	LXPrimitiveD3D11* Primitive = nullptr;
	LXTextureD3D11* TextureOutput = nullptr;
	LXRenderTargetViewD3D11* RenderTarget = nullptr;
};

class LXRenderClusterJobBitmap : public LXRenderClusterJob
{
public:
	void Render(LXRenderCommandList* RCL, LXConstantBufferD3D11* ConstantBuffer) override;
	//LXBitmap* BitmapOutput = nullptr;
	//LXTextureD3D11* TextureCapture = nullptr;
};