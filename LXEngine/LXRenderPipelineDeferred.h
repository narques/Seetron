//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXRenderPipeline.h"
#include "LXConstantBufferD3D11.h"

class LXRenderCluster;
class LXRenderCommandList;
class LXRenderer;
class LXRenderPass;
class LXRenderPassAA;
class LXRenderPassAux;
class LXRenderPassDownsample;
class LXRenderPassDynamicTexture;
class LXRenderPassGBuffer;
class LXRenderPassLighting;
class LXRenderPassShadow;
class LXRenderPassSSAO;
class LXRenderPassToneMapping;
class LXRenderPassTransparency;
class LXRenderPassUI;
class LXRenderPassDepthOfField;

enum class LXTextureSlot
{
	//0-9 reserved for the user textures
	Material_IBL = 10 // Radiance and Irradiance
};

enum class LXConstantBufferSlot
{
	CB_Material_Data = 2,
	CB_Material_IBL = 3
};

class LXRenderPipelineDeferred : public LXRenderPipeline
{
	
	friend LXRenderPassGBuffer;
	friend LXRenderPassLighting;
	friend LXRenderPassShadow;
	friend LXRenderPassToneMapping;
	friend LXRenderPassTransparency;
	friend LXRenderPassDownsample;
	friend LXRenderPassDepthOfField;

public:

	LXRenderPipelineDeferred(LXRenderer* Renderer);
	virtual ~LXRenderPipelineDeferred();
	
	void RebuildShaders() override;
	void Resize(uint Width, uint Height) override;
	void Render(LXRenderCommandList* RenderCommandList) override;
	void PostRender() override;
		
	const LXRenderPassGBuffer* GetRenderPassGBuffer() const { return RenderPassGBuffer; }
	const LXRenderPassLighting* GetRenderPassLighting() const { return RenderPassLighting; }
	const LXRenderPassTransparency* GetRenderPassTransparency() const { return RenderPassTransparent; }
	const LXRenderPassAux* GetRenderPassAux() const { return RenderPassAux; }

	const LXRenderPass* GetPreviousRenderPass() const override { return _PreviousRenderPass; }
	
	// G-Buffer
	const LXTextureD3D11* GetDepthBuffer() const;
	const LXTextureD3D11* GetColorBuffer() const;
	const LXTextureD3D11* GetNormalBuffer() const;
	const LXTextureD3D11* GetMRULBuffer() const;
	const LXTextureD3D11* GetEmissiveBuffer() const;
	
	// Misc
	const LXConstantBufferD3D11* GetCBViewProjection() const { return _CBViewProjection; }
	const LXTextureD3D11* GetOutput() const override;
	void GetTextureCoordinatesInAtlas(LXRenderCluster* RenderCluster, vec4f& outTextureCoordinates);
	const LXTextureD3D11* GetTextureNoise4x4() const { return _TextureNoise4x4; }
	const list<LXRenderCluster*>& GetRenderClusterAuxiliary() const { return _ListRenderClusterAuxiliary; }

private:

	void BuildRenderClusterLists();
	
private:

	LXRenderer* _Renderer = nullptr;

	// Render passes 
	LXRenderPassDynamicTexture* RenderPassDynamicTexture = nullptr;
	LXRenderPassGBuffer* RenderPassGBuffer = nullptr;
	LXRenderPassAux* RenderPassAux = nullptr;
	LXRenderPassShadow* RenderPassShadow = nullptr;
	LXRenderPassTransparency* RenderPassTransparent = nullptr;
	LXRenderPassLighting* RenderPassLighting = nullptr;
	LXRenderPassToneMapping* RenderPassToneMapping = nullptr;
	LXRenderPassDownsample* RenderPassDownsample = nullptr;
	LXRenderPassUI* RenderPassUI = nullptr;
	LXRenderPassSSAO* RenderPassSSAO = nullptr;
	LXRenderPassAA* RenderPassAA = nullptr;
	unique_ptr<LXRenderPassDepthOfField> _renderPassDOF;
		
	// View ConstantBuffer
	LXConstantBufferD3D11* _CBViewProjection = nullptr;
	LXConstantBufferData0 _CBViewProjectionData;

	// Visible clusters in the main view frustum
	list<LXRenderCluster*> _ListRenderClusterOpaques;
	list<LXRenderCluster*> _ListRenderClusterTransparents;
	list<LXRenderCluster*> _ListRenderClusterAuxiliary;
	list<LXRenderCluster*> _ListRenderClusterLights;
	
	// Global textures
	LXTextureD3D11* _TextureD3D11IBL = nullptr;
	LXTextureD3D11* _TextureNoise4x4 = nullptr;
};

