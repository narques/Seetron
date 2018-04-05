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
class LXShaderManager;
class LXTextureManager;
class LXRenderPipeline;
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

class LXRenderer : public LXObject
{
	friend class LXRenderPipelineDeferred;
	friend LXMaterialD3D11;
	friend LXRenderCluster;
	friend LXRenderClusterJobTexture;
	
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
	LXShaderManager* GetShaderManager() const { return ShaderManager; }
	LXTextureManager* GetTextureManager() const { return TextureManager; }
	LXRenderPipeline* GetRenderPipeline() const { return _RenderPipeline; }

	// Shared objects
	ID3D11RasterizerState* GetDefaultRasterizerState () const { return D3D11RasterizerState; }
	ID3D11BlendState* GetBlendStateOpaque() const { return D3D11BlendStateNoBlend; }
	ID3D11BlendState* GetBlendStateTransparent() const { return D3D11BlendStateBlend; }
	LXPrimitiveD3D11* GetSSTriangle() const { return SSTriangle; }
	const LXDirectX11* GetDirectX11() const { return DirectX11; }
	
	// Misc
	void ResetShaders();
	const LXTime& GetTime() { return Time; }
	void Render_MainThread();
	void DrawScreenSpacePrimitive(LXRenderCommandList* RCL);
	const list<LXString>& GetConsoleBuffer() const { return ConsoleBuffer; }
	const LXViewport* GetViewport() const {	return Viewport; }
			
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

	LXRenderClusterManager * RenderClusterManager = nullptr;

	UINT Width = 1920;
	UINT Height = 1080;

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

	// Misc
	LXPrimitiveD3D11* SSTriangle = nullptr;

	ID3D11RasterizerState* D3D11RasterizerState = nullptr;
	ID3D11RasterizerState* D3D11RasterizerStateTwoSided = nullptr;
	ID3D11RasterizerState* D3D11RasterizerStateWireframe = nullptr;
	ID3D11BlendState* D3D11BlendStateNoBlend = nullptr;
	ID3D11BlendState* D3D11BlendStateBlend = nullptr;

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

	// Managers
	LXShaderManager* ShaderManager = nullptr;
	LXTextureManager* TextureManager = nullptr;
	LXRenderPipeline* _RenderPipeline = nullptr;

	// Misc
	LXTime Time;
	unsigned __int64 Frame = 0;
};

