//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include "LXTime.h"

class LXActor;
class LXConstantBufferD3D11;
class LXDirectX11;
class LXMaterialD3D11;
class LXMatrix;
class LXPrimitive;
class LXPrimitiveD3D11;
class LXProject;
class LXRenderCluster;
class LXRenderClusterJobTexture;
class LXRenderClusterManager;
class LXRenderCommandList;
class LXRenderPass;
class LXRenderPassAux;
class LXRenderPassDownsample;
class LXRenderPassDynamicTexture;
class LXRenderPassGBuffer;
class LXRenderPassLighting;
class LXRenderPassShadow;
class LXRenderPassToneMapping;
class LXRenderPassTransparency;
class LXShaderManager;
class LXString;
class LXSyncEvent;
class LXTextureD3D11;
class LXThread;
class LXViewport;
class LXWorldTransformation;
struct ID3D11RasterizerState;
struct ID3D11BlendState;

#define CONSOLE_MAX_LINE 20

extern bool ShowWireframe;

enum class EShowBuffer
{
	Default,
	Depth,
	Color,
	Normal,
	Shadow
};

class LXRenderer : public LXObject
{
	friend LXRenderPassShadow;
	friend LXRenderPassGBuffer;
	friend LXRenderPassAux;
	friend LXRenderPassLighting;
	friend LXRenderPassToneMapping;
	friend LXRenderPassDynamicTexture;
	friend LXMaterialD3D11;
	friend LXRenderCluster;
	friend LXRenderClusterJobTexture;
	friend LXRenderPassTransparency;

public:

	LXRenderer(HWND hWND);
	virtual ~LXRenderer();

	void SetDocument(LXProject* InDocument);
	const LXProject* GetProject() const;
	void SetViewport(LXViewport* InViewport) { Viewport = InViewport; }
	
	// Thread management
	LXThread* GetThread() const { return Thread; }
	LXSyncEvent* GetBeginEvent() const;
	LXSyncEvent* GetEndEvent() const;

	// Show 
	void ShowBounds(bool Show);
	bool ShowBounds() const;

	// Resources
	LXMaterialD3D11* GetMaterialD3D11(const LXMaterial* Material);

	// Misc
	LXShaderManager* GetShaderManager() const { return ShaderManager; }
	void ResetShaders();
	const LXTime& GetTime() { return Time; }
	LXPrimitiveD3D11* GetSSTriangle() const { return SSTriangle; }
	void Render_MainThread();
	const LXRenderPassGBuffer* GetRenderPassGBuffer() const { return RenderPassGBuffer; }
	void DrawScreenSpacePrimitive(LXRenderCommandList* RCL);
	const LXRenderPass* GetPreviousRenderPass() const  { return PreviousRenderPass; }

	// Default Pipeline Buffers 

	// G-Buffer
	const LXTextureD3D11* GetDepthBuffer() const;
	const LXTextureD3D11* GetColorBuffer() const;
	const LXTextureD3D11* GetNormalBuffer() const;
	const LXTextureD3D11* GetMRULBuffer() const;
	const LXTextureD3D11* GetEmissiveBuffer() const;


private:

	static int RenderFunc(void* pData);
	void Run();
	void Init();
	void Render();
	void DeleteObjects();
	void Empty();
		
	void UpdateStates();
	void AddActor(LXActor* Actor);
	void RemoveActor(LXActor* Actor);
	
	// RenderCluster management
	bool SetShaders(LXRenderClusterJobTexture* RenderCluster);

	// Ref. Render context
	const LXWorldTransformation& GetWorldTransform() const;
	const LXMatrix& GetMatrixView() const;
	const LXMatrix& GetMatrixProjection() const;
	
public:

	UINT Width = 1920;
	UINT Height = 1080;

	// Dev
	EShowBuffer ShowBuffer = EShowBuffer::Default;

	static bool gUseRenderThread;

private:
		
	HWND _hWND = NULL;
	
	// RenderThread Management
	LXThread* Thread = nullptr;
	LXSyncEvent* EventBeginFrame = nullptr;
	LXSyncEvent* EventEndFrame = nullptr;

	// Thread communication throw flags
	bool ExitRenderThread = false;

	// Without RenderThread. Needed to init the Renderer in the Render_MainThread function
	bool _bInit = false;
	
	// RenderThread objects
	LXDirectX11* DirectX11 = nullptr;
	
	LXRenderCommandList* RenderCommandList = nullptr;

	LXConstantBufferD3D11* CBViewProjection = nullptr;
	
	// Render passes 
	LXRenderPassDynamicTexture* RenderPassDynamicTexture = nullptr;
	LXRenderPassGBuffer* RenderPassGBuffer = nullptr;
	LXRenderPassAux* RenderPassAux = nullptr;
	LXRenderPassShadow* RenderPassShadow = nullptr;
	LXRenderPassTransparency* RenderPassTransparent = nullptr;
	LXRenderPassLighting* RenderPassLighting = nullptr;
	LXRenderPassToneMapping* RenderPassToneMapping = nullptr;
	LXRenderPassDownsample* RenderPassDownsample = nullptr;

	// Misc
	LXRenderPass* PreviousRenderPass = nullptr;
	vector<LXRenderPass*> RenderPasses;
	
	LXPrimitiveD3D11* SSTriangle = nullptr;

	ID3D11RasterizerState* D3D11RasterizerState = nullptr;
	ID3D11RasterizerState* D3D11RasterizerStateTwoSided = nullptr;
	ID3D11RasterizerState* D3D11RasterizerStateWireframe = nullptr;
	ID3D11BlendState* D3D11BlendStateNoBlend = nullptr;
	ID3D11BlendState* D3D11BlendStateBlend = nullptr;

	ID2D1SolidColorBrush* pRedBrush = nullptr;
	ID2D1SolidColorBrush* pWhiteBrush = nullptr;
	ID2D1SolidColorBrush* pYellowBrush = nullptr;
	ID2D1SolidColorBrush* pBlackBrush = nullptr;
	ID2D1SolidColorBrush* pGrayBrush = nullptr;
	ID2D1SolidColorBrush* pDarkBlue = nullptr;
	ID2D1SolidColorBrush* pConsoleBackgroundBrush = nullptr;
	
	// Scene & Document
	LXProject* _Project = nullptr;
	LXProject* _NewProject = nullptr;
	LXViewport* Viewport = nullptr;

	// Console
	list<LXString> ConsoleBuffer;

	// Show
	bool bShowBounds = false;

	// Actions
	bool DoShowBounds = false;
	bool DoHideBounds = false;
	
	// Misc
	LXTime Time;
	unsigned __int64 Frame = 0;
	LXShaderManager* ShaderManager = nullptr;
	LXRenderClusterManager* RenderClusterManager = nullptr;
};

