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

class LXDepthStencilViewD3D11;
class LXRenderCluster;
class LXRenderCommandList;
class LXRenderer;
class LXRenderPass;
class LXRenderPassAA;
class LXRenderPassAux;
class LXRenderPassDepth;
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

	virtual void Clear() override;
	
	void RebuildShaders() override;
	void Resize(uint Width, uint Height) override;
	void Render(LXRenderCommandList* RenderCommandList) override;
	void PostRender() override;
		
	const LXRenderPassGBuffer* GetRenderPassGBuffer() const { return RenderPassGBuffer; }
	const LXRenderPassLighting* GetRenderPassLighting() const { return RenderPassLighting; }
	const LXRenderPassTransparency* GetRenderPassTransparency() const { return RenderPassTransparent; }
	const LXRenderPassAux* GetRenderPassAux() const { return RenderPassAux; }
	const LXRenderPassDynamicTexture* GetRenderPassDynamicTexture() const { return RenderPassDynamicTexture; }

	const LXRenderPass* GetPreviousRenderPass() const override { return _PreviousRenderPass; }
	
	// G-Buffer
	const LXTextureD3D11* GetDepthBuffer() const;
	const LXDepthStencilViewD3D11* GetDepthStencilView() const;
	const LXTextureD3D11* GetColorBuffer() const;
	const LXTextureD3D11* GetNormalBuffer() const;
	const LXTextureD3D11* GetMRULBuffer() const;
	const LXTextureD3D11* GetEmissiveBuffer() const;
	
	// Misc
	const LXConstantBufferD3D11* GetCBViewProjection() const { return _CBViewProjection; }
	const LXConstantBufferData0& GetCBViewProjectionData() const { return _CBViewProjectionData; }

	const LXTextureD3D11* GetOutput() const override;
	void GetTextureCoordinatesInAtlas(LXRenderCluster* RenderCluster, vec4f& outTextureCoordinates);
	const std::list<LXRenderCluster*>& GetRenderClusterAuxiliary() const { return _ListRenderClusterAuxiliary; }
	const std::list<LXRenderCluster*>& GetRenderClusterRenderToTextures() const { return _listRenderClusterRenderToTextures; }

private:

	void BuildRenderClusterLists();
	
private:

	LXRenderer* _Renderer = nullptr;

	// Render passes 
	LXRenderPassDynamicTexture* RenderPassDynamicTexture = nullptr;
	LXRenderPassDepth* RenderPassDepth = nullptr;
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
	std::unique_ptr<LXRenderPassDepthOfField> _renderPassDOF;
		
	// View ConstantBuffer
	LXConstantBufferD3D11* _CBViewProjection = nullptr;
	LXConstantBufferData0 _CBViewProjectionData;

	// Visible clusters in the main view frustum
	std::list<LXRenderCluster*> _ListRenderClusterOpaques;
	std::list<LXRenderCluster*> _ListRenderClusterTransparents;
	std::list<LXRenderCluster*> _ListRenderClusterAuxiliary;
	std::list<LXRenderCluster*> _ListRenderClusterLights;
	std::list<LXRenderCluster*> _listRenderClusterRenderToTextures;

	// Batching
	class LXRenderBatching* _batching = nullptr;
};

