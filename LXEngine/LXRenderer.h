//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXCore/LXObject.h"
#include "LXRenderClusterType.h"
#include "LXShaderProgramD3D11.h"
#include "LXSynchronisable.h"
#include "LXTime.h"

class LXConstantBufferD3D11;
class LXDeviceResourceManager;
class LXDirectX11;
class LXMaterialD3D11;
class LXMatrix;
class LXMaterialBase;
class LXPrimitive;
class LXPrimitiveD3D11;
class LXProject;
class LXRenderCluster;
class LXRenderClusterJobTexture;
class LXRenderClusterManager;
class LXRenderCommandList;
class LXShaderManager;
class LXTextureManager;
class LXRenderData;
class LXRenderPipeline;
class LXString;
class LXSyncEvent;
class LXTask;
class LXTaskManager;
class LXTexture;
class LXTextureD3D11;
class LXThread;
class LXViewport;
class LXWorldTransformation;
struct ID3D11BlendState;
struct ID3D11RasterizerState;
struct ID3D11SamplerState;

#define CONSOLE_MAX_LINE 20

extern bool ShowWireframe;

// Shared Resources to render the BBoxes
class LXRenderBBoxResources
{
public:
	LXRenderBBoxResources();
	~LXRenderBBoxResources();
	std::shared_ptr<LXPrimitive> WireframeCube;
	LXPrimitiveD3D11* WireframeCubeD3D11 = nullptr;
	LXShaderProgramD3D11 ShaderProgramD3D11;
	std::shared_ptr<LXMaterialD3D11> MaterialD3D11;
};

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

	// Resources
	LXShaderManager* GetShaderManager() const { return ShaderManager; }
	LXDeviceResourceManager* GetDeviceResourceManager() const { return _deviceResourceManager; }
	LXTextureManager* GetTextureManager() const { return TextureManager; }
	LXRenderPipeline* GetRenderPipeline() const { return _RenderPipeline; }

	const LXTextureD3D11* GetTextureNoise4x4() const;
	
	void UpdateActor(LXRenderData* renderData, LXFlagsRenderClusterRole renderStates);
	void ReleaseRenderData(LXRenderData* actor, LXFlagsRenderClusterRole renderStates);
		
	void CreateDeviceTexture(std::shared_ptr<LXTexture> texture);
	void ReleaseDeviceTexture(LXTexture* texture);
	void CopyDeviceTexture(LXTexture* texture);
	
	void ReleaseDeviceMaterials(LXMaterialBase* material);
	void UpdateDeviceMaterials(LXMaterialBase* material);

	// Shared objects & resources
	LXRenderBBoxResources* GetRenderBBoxResources() const { return _renderBBoxResources; }
	ID3D11RasterizerState* GetDefaultRasterizerState () const { return D3D11RasterizerState; }
	ID3D11BlendState* GetBlendStateOpaque() const { return D3D11BlendStateNoBlend; }
	ID3D11BlendState* GetBlendStateTransparent() const { return D3D11BlendStateBlend; }
	ID3D11BlendState* GetBlendStateAdd() const { return D3D11BlendStateAdd; }
	ID3D11SamplerState* GetSamplerStateTexturing() const { return D3D11SamplerStateTexturing; }
	ID3D11SamplerState* GetSamplerStateRenderTarget() const { return D3D11SamplerStateRenderTarget; }

	LXPrimitiveD3D11* GetSSTriangle() const { return SSTriangle; }
	const LXDirectX11* GetDirectX11() const { return DirectX11; }

	// Task
	void EnqueueTask(LXTask* task);
	bool HasPendingTasks() const;
	
	// Misc
	void ResetShaders();
	void Render_MainThread();
	void DrawScreenSpacePrimitive(LXRenderCommandList* RCL);
	const std::list<LXString>& GetConsoleBuffer() const { return ConsoleBuffer; }
	const LXViewport* GetViewport() const {	return Viewport; }
	void Sync();
			
private:

	static int RenderFunc(void* pData);
	void Run();
	void Init();
	void CreateCommonResources();
	void Render();
	void DeleteObjects();
	void Empty();
	
	// RenderCluster management
	bool SetShaders(LXRenderClusterJobTexture* RenderCluster);

	// Ref. Render context
	const LXWorldTransformation& GetWorldTransform() const;
	const LXMatrix& GetMatrixView() const;
	const LXMatrix& GetMatrixProjection() const;

	// Misc
	void UpdateActors_RT();
	void ReleaseRenderData_RT();
	void CreateDeviceTexture_RT(LXTexture* texture);
	void CopyDeviceTexture_RT(LXTexture* texture);

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
	ID3D11BlendState* D3D11BlendStateAdd = nullptr;
	ID3D11SamplerState* D3D11SamplerStateTexturing = nullptr;
	ID3D11SamplerState* D3D11SamplerStateRenderTarget = nullptr;

	// Resources
	LXRenderBBoxResources* _renderBBoxResources = nullptr;
	std::shared_ptr<LXTexture> _TextureNoise4x4;

	// Scene & Document
	LXProject* _Project = nullptr;
	LXProject* _NewProject = nullptr;
	LXViewport* Viewport = nullptr;

	// Console
	std::list<LXString> ConsoleBuffer;
	
	// Managers
	LXShaderManager* ShaderManager = nullptr;
	LXDeviceResourceManager* _deviceResourceManager = nullptr;
	LXTextureManager* TextureManager = nullptr;
	LXRenderPipeline* _RenderPipeline = nullptr;

	//
	// Tasks
	//

	// RenderData to Update or Release at the frame beginning.
	LXSynchronisableSet<std::pair<LXRenderData*, LXFlagsRenderClusterRole>> _actorsToUpdate;
	LXSynchronisableSet<std::pair<LXRenderData*, LXFlagsRenderClusterRole>> _toRelease;

	// RenderThread tasks run at the frame beginning.
	std::unique_ptr<LXTaskManager> _mainTasks;

	// Enqueued Textures for device creation. Set by the MainThread.
	// Avoid early texture destruction.
	std::map <LXTexture*, std::shared_ptr<LXTexture>> _enqueuedTexture;
	std::map <LXMaterialBase*, std::shared_ptr<LXMaterialBase>> _enqueuedMaterials;
};

