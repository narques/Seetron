//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXAnimationManager.h"
#include "LXAssetManager.h"
#include "LXBitmap.h"
#include "LXConsoleManager.h"
#include "LXConstantBufferD3D11.h"
#include "LXEngine.h"
#include "LXDeviceResourceManager.h"
#include "LXLogger.h"
#include "LXMaterialBase.h"
#include "LXMaterialD3D11.h"
#include "LXPrimitiveD3D11.h"
#include "LXPrimitiveFactory.h"
#include "LXProject.h"
#include "LXRenderClusterManager.h"
#include "LXRenderCommandList.h"
#include "LXRenderPipelineDeferred.h"
#include "LXRenderer.h"
#include "LXRenderData.h"
#include "LXScene.h"
#include "LXShaderManager.h"
#include "LXSyncEvent.h"
#include "LXTask.h"
#include "LXTexture.h"
#include "LXTextureD3D11.h"
#include "LXTextureManager.h"
#include "LXThread.h"
#include "LXTimeD3D11.h"
#include "LXViewport.h"
#include "LXStatManager.h"
#include "LXStatistic.h"
#include "LXTaskManager.h"
#include "LXThreadManager.h"

//------------------------------------------------------------------------------------------------------
// Console commands and Settings
//------------------------------------------------------------------------------------------------------

// Binded on variable.
bool ShowWireframe = false;
LXConsoleCommandT<bool> CCShowWireframe(L"ShowWireframe", &ShowWireframe);

// Request for reseting the shaders
bool gResetShaders = false; 
LXConsoleCommandT<bool> CCResetShaders(L"ResetShaders", &gResetShaders);

// Request for changing DrawImmmediate parameter
bool gToogleDrawImmediate = false;
LXConsoleCommandT<bool> CCToogleDrawImmediate(L"ToggleDrawImmediate", &gToogleDrawImmediate);

// Binded on variable and set from an INI file.
//LXConsoleCommandT<bool> CSet_NoOverwriteConstantBuffer(L"Engine.ini", L"D3D111", L"NoOverwriteConstantBuffer", L"false", &NoOverwriteConstantBuffer);

// bool gShowDynamicTexture = false;
//LXConsoleCommandT<bool> CCShowDynamicTexture(L"ShowDynamicTexture", &gShowDynamicTexture);

// Retrieve the viewport resolution
LXConsoleCommandNoArg CCResolution(L"Resolution", []()
{
	LogI(Viewport, L"Viewport size: %ix%i", GetEngine().GetViewport()->GetWidth(), GetEngine().GetViewport()->GetHeight());
});

bool LXRenderer::gUseRenderThread = true;

//------------------------------------------------------------------------------------------------------

int LXRenderer::RenderFunc(void* pData)
{
	RenderThread = GetCurrentThreadId();
	LXRenderer* Renderer = (LXRenderer*)pData;
	Renderer->Run();
	RenderThread = 0;
	return 0;
}

LXRenderer::LXRenderer(HWND hWND):_hWND(hWND)
{
	if (gUseRenderThread)
	{
		Thread = new LXThread();
		EventBeginFrame = new LXSyncEvent(false);
		EventEndFrame = new LXSyncEvent(true);
		Thread->Run(&RenderFunc, this);
	}

	GetEngine().SetRenderer(this);
	GetLogger().Register(nullptr, [this](ELogType LogType, const wchar_t* Messsage)
	{
		ConsoleBuffer.push_back(Messsage);
		if (ConsoleBuffer.size() > (CONSOLE_MAX_LINE - 1))
			ConsoleBuffer.pop_front();
	});
}

LXRenderer::~LXRenderer()
{
	GetLogger().Unregister(nullptr);
	GetEngine().SetRenderer(nullptr);
	if (gUseRenderThread)
	{
		ExitRenderThread = true;
		EventBeginFrame->SetEvent();
		Thread->Wait();
		delete Thread;
		delete EventBeginFrame;
		delete EventEndFrame;
	}
	else
	{
		DeleteObjects();
	}
}

void LXRenderer::Init()
{
	//
	// Create objects
	//

	DirectX11 = new LXDirectX11(_hWND);
	RenderCommandList = new LXRenderCommandList();

	//
	// Create common Rasterizers, Sample
	//

	CreateCommonResources();

	//
	// Misc
	//

	ShaderManager = new LXShaderManager();
	ShaderManager->CreateDefaultShaders();

	_deviceResourceManager = new LXDeviceResourceManager();
	
	TextureManager = new LXTextureManager();
	RenderClusterManager = new LXRenderClusterManager();
	RenderCommandList->Renderer = this;
	RenderCommandList->ShaderManager = ShaderManager;
	
	// ScreenSpace Triangle
	SSTriangle = new LXPrimitiveD3D11();
	SSTriangle->CreateSSTriangle();
	
	_RenderPipeline = new LXRenderPipelineDeferred(this);
}

void LXRenderer::CreateCommonResources()
{
	//
	// Rasterizers
	//

	// Default rasterizer
	CD3D11_RASTERIZER_DESC RasterizerDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
	RasterizerDesc.FrontCounterClockwise = TRUE;
	RasterizerDesc.AntialiasedLineEnable = TRUE;
	HRESULT hr = DirectX11->GetCurrentDevice()->CreateRasterizerState(&RasterizerDesc, &D3D11RasterizerState);
	DirectX11->GetCurrentDeviceContext()->RSSetState(D3D11RasterizerState);

	// TwoSided Default rasterizer
	RasterizerDesc.CullMode = D3D11_CULL_NONE;
	hr = DirectX11->GetCurrentDevice()->CreateRasterizerState(&RasterizerDesc, &D3D11RasterizerStateTwoSided);

	// Wireframe rasterizer
	ZeroMemory(&RasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	RasterizerDesc.CullMode = D3D11_CULL_NONE;
	RasterizerDesc.FrontCounterClockwise = FALSE;
	RasterizerDesc.DepthBias = 0;
	RasterizerDesc.DepthBiasClamp = 0.0f;
	RasterizerDesc.SlopeScaledDepthBias = 0.0f;
	RasterizerDesc.DepthClipEnable = TRUE;
	RasterizerDesc.ScissorEnable = FALSE;
	RasterizerDesc.MultisampleEnable = FALSE;
	RasterizerDesc.AntialiasedLineEnable = FALSE;
	RasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	hr = DirectX11->GetCurrentDevice()->CreateRasterizerState(&RasterizerDesc, &D3D11RasterizerStateWireframe);

	//
	// BlendStates
	//

	// No Blend
	D3D11_BLEND_DESC BlendState = CD3D11_BLEND_DESC(CD3D11_DEFAULT());
	hr = DirectX11->GetCurrentDevice()->CreateBlendState(&BlendState, &D3D11BlendStateNoBlend);

	// Alpha blending
	BlendState.RenderTarget[0].BlendEnable = TRUE;
	BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	hr = DirectX11->GetCurrentDevice()->CreateBlendState(&BlendState, &D3D11BlendStateBlend);

	// Deferred lighting accumulation (ADD)
	D3D11_BLEND_DESC BlendStateAdd = CD3D11_BLEND_DESC(CD3D11_DEFAULT());
	BlendStateAdd.RenderTarget[0].BlendEnable = TRUE;
	BlendStateAdd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	BlendStateAdd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	BlendStateAdd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	hr = DirectX11->GetCurrentDevice()->CreateBlendState(&BlendStateAdd, &D3D11BlendStateAdd);

	//
	//  SamplerStates
	//

	// Default textures
	{
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));
		sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MaxAnisotropy = 16;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		hr = DirectX11->GetCurrentDevice()->CreateSamplerState(&sampDesc, &D3D11SamplerStateTexturing);
	}

	// RenderBuffer
	{
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MaxAnisotropy = 0;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		hr = DirectX11->GetCurrentDevice()->CreateSamplerState(&sampDesc, &D3D11SamplerStateRenderTarget);
	}
}

void LXRenderer::Run()
{
	bool init = false;

	// Tasks
	_mainTasks = std::make_unique<LXTaskManager>();
			
	// Render !
	while (!ExitRenderThread || HasPendingTasks())
	{
		LX_PERFOSCOPE(RenderThread);

		// Do not remove the brackets, needed to measure the wait time.
		{
			LX_PERFOSCOPE(RenderThread_WaitTime);
			EventBeginFrame->Wait();
		}

		EventBeginFrame->Reset();

		if (!init)
		{
			CHK(LXEngine::FrameNumber == 0);
			LXPerformance perf;
			Init();
			init = true;
			LogI(Renderer, L"Renderer initialized in %.2f ms.", perf.GetTime());
		}

		Render();

		EventEndFrame->SetEvent();
	}
	
	Empty();
	DeleteObjects();
}

void LXRenderer::DeleteObjects()
{
	CHK(IsRenderThread());
	LX_SAFE_DELETE(ShaderManager);
	LX_SAFE_DELETE(_deviceResourceManager);
	LX_SAFE_DELETE(TextureManager);
	LX_SAFE_DELETE(RenderClusterManager);
	LX_SAFE_DELETE(SSTriangle);
	LX_SAFE_DELETE(_RenderPipeline);
	LX_SAFE_DELETE(RenderCommandList);
	LX_SAFE_RELEASE(D3D11RasterizerState);
	LX_SAFE_RELEASE(D3D11RasterizerStateTwoSided);
	LX_SAFE_RELEASE(D3D11RasterizerStateWireframe);
	LX_SAFE_RELEASE(D3D11BlendStateNoBlend);
	LX_SAFE_RELEASE(D3D11BlendStateBlend);
	LX_SAFE_RELEASE(D3D11BlendStateAdd);
	LX_SAFE_RELEASE(D3D11SamplerStateTexturing);
	LX_SAFE_RELEASE(D3D11SamplerStateRenderTarget);
	LX_SAFE_DELETE(DirectX11);
}

void LXRenderer::Render_MainThread()
{
	CHK(!gUseRenderThread);

	if (!_bInit)
	{
		Init();
		_bInit = true;
	}

	Render();
}

void LXRenderer::DrawScreenSpacePrimitive(LXRenderCommandList* RCL)
{
	if (SSTriangle)
		SSTriangle->Render(RCL);
}

void LXRenderer::SetDocument(LXProject* Project)
{ 
	_NewProject = Project;
	if (_NewProject)
	{
		_TextureNoise4x4 = GetAssetManager()->GetNoiseTexture4x4();
	}
	else
	{
		_TextureNoise4x4 = nullptr;
	}
}

const LXProject* LXRenderer::GetProject() const
{
	return _Project;
}
 
LXSyncEvent* LXRenderer::GetBeginEvent() const
{
	CHK(RenderThread); 
	return EventBeginFrame;
}

LXSyncEvent* LXRenderer::GetEndEvent() const
{
	CHK(RenderThread); 
	return EventEndFrame;
}


void LXRenderer::ResetShaders()
{
	gResetShaders = true;
}

void LXRenderer::Sync()
{
	CHK(IsMainThread());
	_actorsToUpdate.Swap();
	_toRelease.Swap();
}

void LXRenderer::Render()
{
	LX_PERFOSCOPE(RenderThread_Render);

	const LXTime& time = GetEngine().Time;

	// If project changed
	if (_Project != _NewProject)
	{
		_Project = _NewProject;
		LX_SAFE_DELETE(_renderBBoxResources);
		Empty();
		
		if (_Project)
		{
			_renderBBoxResources = new LXRenderBBoxResources();
		}
	}

	// Objects set to Update/Release/Delete/...
	UpdateActors_RT();
	ReleaseRenderData_RT();
			
	//
	// Commands & Tasks
	//

	_mainTasks->Run((float)time.DeltaTime());


	// "Tick" the ShaderManager & the ResourceManager
	// Maybe they have things to delete/release
	ShaderManager->Run();
	_deviceResourceManager->Run();
			
	if (gResetShaders)
	{
		ShaderManager->RebuildShaders();
		_RenderPipeline->RebuildShaders();
		gResetShaders = false;
	}

	if (gToogleDrawImmediate)
	{
		RenderCommandList->DirectMode = !RenderCommandList->DirectMode;
		gToogleDrawImmediate = false;
	}
	
	// Resize if needed
	if (Width != Viewport->GetWidth() || Height != Viewport->GetHeight())
	{
		Width = std::max(Viewport->GetWidth(), (uint)32);
		Height = std::max(Viewport->GetHeight(), (uint)32);

		DirectX11->Resize(Width, Height);
						
		_RenderPipeline->Resize(Width, Height);
	}
	
	if (_Project)
	{
		_Project->GetAnimationManager().Update(time.DeltaTime());
	}

	// TODO END
	//-----------------
	
	// Clear the command list
	RenderCommandList->Empty();
	_RenderPipeline->Render(RenderCommandList);

	//
	// To back buffer.
	//

	if(1)
	{
		const LXTextureD3D11* Color = _RenderPipeline->GetOutput();
		
		RenderCommandList->BeginEvent(L"DrawToBackBuffer");
		RenderCommandList->RSSetViewports(Viewport->GetWidth(), Viewport->GetHeight());

		// Bind the back buffer 
		RenderCommandList->OMSetRenderTargets(DirectX11->_D3D11RenderTargetView); // Hack

		// Render a triangle : GBuffer TextureColor
		RenderCommandList->IASetInputLayout(ShaderManager->VSDrawToBackBuffer);
		RenderCommandList->VSSetShader(ShaderManager->VSDrawToBackBuffer);
		RenderCommandList->PSSetShader(ShaderManager->PSDrawToBackBuffer);
		
		RenderCommandList->PSSetShaderResources(0, 1, (LXTextureD3D11*)Color);
		RenderCommandList->PSSetSamplers(0, 1, D3D11SamplerStateRenderTarget);
		SSTriangle->Render(RenderCommandList);
		RenderCommandList->PSSetShaderResources(0, 1, nullptr);

		RenderCommandList->EndEvent();
	}
			
	// Execute the command list
	LXTimeD3D11 TimeGPU;
	double ElapsedGPU = 0.;
	RenderCommandList->Execute();
	ElapsedGPU = TimeGPU.Update();

	// Execute RenderCommandList time on GPU
	LX_COUNT(L"GPU (Execute commands): %.2f MS", ElapsedGPU);

	// DrawCalls
	LX_COUNT(L"DrawCalls : %f", RenderCommandList->DrawCallCount);

	// Triangles
	LX_COUNT(L"Triangles : %f", RenderCommandList->TriangleCount);

	// RenderClusters
	LX_COUNT(L"RenderClusters : %f", (int)RenderClusterManager->ListRenderClusters.size());

	// VertexShaders
	LX_COUNT(L"VertexShaders : %f", (int)ShaderManager->VertexShaders.size());

	// HullShaders
	LX_COUNT(L"HullShaders : %f", (int)ShaderManager->HullShaders.size());

	// DomainShaders
	LX_COUNT(L"DomainShaders : %f", (int)ShaderManager->DomainShaders.size());

	// PixelShaders
	LX_COUNT(L"PixelShaders : %f", (int)ShaderManager->PixelShaders.size());

	// Frame
	LX_COUNT(L"Frame : %f", (double)LXEngine::FrameNumber);
	
	_RenderPipeline->PostRender();

	DirectX11->Present();
}

bool LXRenderer::SetShaders(LXRenderClusterJobTexture* RenderCluster)
{
	//return ShaderManager->GetShaders(RenderCluster->Primitive, RenderCluster->Material, &RenderCluster->VertexShader, nullptr, nullptr, &RenderCluster->PixelShader);
	CHK(0); // It misses the RenderPass as GetShaders first parameter.
	return false;
}

const LXWorldTransformation& LXRenderer::GetWorldTransform() const
{
	return Viewport->WorldTransformation;
}

const LXMatrix& LXRenderer::GetMatrixView() const
{
	return Viewport->WorldTransformation.GetMatrixView();
}

const LXMatrix& LXRenderer::GetMatrixProjection() const
{
	return Viewport->WorldTransformation.GetMatrixProjection();
}

void LXRenderer::Empty()
{
	if (_RenderPipeline)
		_RenderPipeline->Clear();

	if (RenderClusterManager)
		RenderClusterManager->Empty();
}

void LXRenderer::EnqueueTask(LXTask* task)
{
	CHK(!ExitRenderThread);
	_mainTasks->EnqueueTask(task);
}

bool LXRenderer::HasPendingTasks() const
{
	CHK(IsRenderThread() || IsSyncPoint);
	return _mainTasks->HasTasks() || !_actorsToUpdate.IsEmpty() || !_toRelease.IsEmpty();
}

const LXTextureD3D11* LXRenderer::GetTextureNoise4x4() const
{
	return _TextureNoise4x4 ? _TextureNoise4x4->GetDeviceTexture() : nullptr;
}

void LXRenderer::UpdateActor(LXRenderData* renderData, LXFlagsRenderClusterRole renderStates)
{
	CHK(IsMainThread());
	_actorsToUpdate.In->insert(std::pair<LXRenderData*, LXFlagsRenderClusterRole>(renderData, renderStates));
}

void LXRenderer::ReleaseRenderData(LXRenderData* renderData, LXFlagsRenderClusterRole renderStates)
{
	CHK(IsMainThread());
	_toRelease.In->insert(std::pair<LXRenderData*, LXFlagsRenderClusterRole>(renderData, renderStates));
}

void LXRenderer::CreateDeviceTexture(std::shared_ptr<LXTexture> texture)
{
	if (IsRenderThread())
	{
		CreateDeviceTexture_RT(texture.get());
	}
	else
	{
		LXTexture* texturePtr = texture.get();

		LXTask* task = new LXTaskCallBack([this, texturePtr]()
		{
			CreateDeviceTexture_RT(texturePtr);
		});

		if (_enqueuedTexture.find(texturePtr) != _enqueuedTexture.end())
		{
			return;
		}
		
		_enqueuedTexture[texturePtr] = texture;
		EnqueueTask(task);
	}
}

void LXRenderer::CreateDeviceTexture_RT(LXTexture* texture)
{
	if (texture->GetDeviceTexture())
	{
		delete texture->GetDeviceTexture();
	}

	LXTextureD3D11* texture3D11 = LXTextureD3D11::CreateFromTexture(texture);
	texture->SetDeviceTexture(texture3D11);
	
	LXTask* task = new LXTaskCallBack([this, texture]()
	{
		_enqueuedTexture.erase(texture);
	});
	GetEngine().EnqueueInvokeDelegate(&texture->DeviceCreated);
	GetEngine().EnqueueTask(task);
}

void LXRenderer::ReleaseDeviceTexture(LXTexture* texture)
{
	const LXTextureD3D11* textureD3D11 = texture->GetDeviceTexture();
	texture->SetDeviceTexture(nullptr);

	if (IsRenderThread())
	{
		delete textureD3D11;
	}
	else
	{
		LXTask* task = new LXTaskCallBack([textureD3D11]()
		{
			CHK(IsRenderThread());
			delete textureD3D11;
		});

		EnqueueTask(task);
	}
}

void LXRenderer::CopyDeviceTexture(LXTexture* texture)
{
	if (IsRenderThread())
	{
		CopyDeviceTexture_RT(texture);
	}
	else
	{
		LXTask* task = new LXTaskCallBack([this, texture]()
		{
			CopyDeviceTexture_RT(texture);
		});
	
		EnqueueTask(task);
	}
}

void LXRenderer::CopyDeviceTexture_RT(LXTexture* texture)
{
	CHK(IsRenderThread());

	ID3D11Device* D3D11Device = LXDirectX11::GetCurrentDevice();
	ID3D11DeviceContext* D3D11DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	ID3D11Texture2D* srcResource = texture->GetDeviceTexture()->D3D11Texture2D;
	LXBitmap* bitmap = texture->GetBitmap(0);
	
	//
	// Create a readable D3D11texture
	//

	ID3D11Texture2D* D3D11Texture2D;
	D3D11_TEXTURE2D_DESC desc;
	srcResource->GetDesc(&desc);
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	HRESULT hr = D3D11Device->CreateTexture2D(&desc, NULL, &D3D11Texture2D);
	if (FAILED(hr))
		CHK(0);

	//
	// Copy Source to the readable texture.
	//

	D3D11DeviceContext->CopyResource(D3D11Texture2D, srcResource);

	//
	// Copy readable texture to bitmap.
	//

	uint32 subresource = D3D11CalcSubresource(0, 0, 1);
	D3D11_MAPPED_SUBRESOURCE mappedSubresource = {};
	HRESULT result = D3D11DeviceContext->Map(D3D11Texture2D, subresource, D3D11_MAP_READ, 0, &mappedSubresource);
	if (result == S_OK)
	{
		const unsigned char* source = static_cast<const unsigned char*>(mappedSubresource.pData);
		unsigned char* destination = (unsigned char*)bitmap->GetPixels();
		int size = bitmap->GetBitmapSize();
		if (bitmap->IsPowerOfTwo())
			memcpy(destination, source, size);
		else
		{
			// Textures are 128 Bytes aligned, Bitmap is not
			int padding = (128 - (bitmap->GetLineSize() % 128)) % 128;
			int sourcelineSize = bitmap->GetLineSize() + padding;
			int destinationlineSize = bitmap->GetLineSize();
			for (uint i = 0; i < bitmap->GetHeight(); i++)
			{
				memcpy(destination, source, destinationlineSize);
				destination += destinationlineSize;
				source += sourcelineSize;
			}
		}
		D3D11DeviceContext->Unmap(srcResource, subresource);
	}
	else
	{
		LXDirectX11::LogError(result);
	}
 
	D3D11Texture2D->Release();
	texture->CopyDeviceToBitmapEnqueued = false;
	GetEngine().EnqueueInvokeDelegate(&texture->BitmapChanged);
}

void LXRenderer::ReleaseDeviceMaterials(LXMaterialBase* material)
{
	if (IsRenderThread())
	{
		_deviceResourceManager->RemoveShaderResources(material);
	}
	else
	{
		LXTask* task = new LXTaskCallBack([this, material]()
		{
			CHK(IsRenderThread());
			_deviceResourceManager->RemoveShaderResources(material);
		});

		EnqueueTask(task);
	}
}

void LXRenderer::UpdateDeviceMaterials(LXMaterialBase* material)
{
	if (IsRenderThread())
	{
		_deviceResourceManager->UpdateShaderResources(material);
	}
	else
	{
		LXTask* task = new LXTaskCallBack([this, material]()
		{
			CHK(IsRenderThread());
			_deviceResourceManager->UpdateShaderResources(material);
		});

		EnqueueTask(task);
	}
}

void LXRenderer::UpdateActors_RT()
{
	for(std::pair<LXRenderData*, LXFlagsRenderClusterRole> it : *_actorsToUpdate.Out)
	{
		LXRenderData* renderData = it.first;
		RenderClusterManager->UpdateActor(renderData, it.second);
	}

	_actorsToUpdate.Out->clear();
}

void LXRenderer::ReleaseRenderData_RT()
{
	for (std::pair<LXRenderData*, LXFlagsRenderClusterRole>it : *_toRelease.Out)
	{
		LXRenderData* renderData = it.first;

		// Release All the associated RenderCluster
		RenderClusterManager->RemoveActor(renderData, ERenderClusterRole::All);
		
		// Give object destruction to the MainThread
		// TODO: could be done on the _mainTasks instead of the _syncTasks
		GetEngine().AddObjectForDestruction(renderData);
	}

	_toRelease.Out->clear();
}

LXRenderBBoxResources::LXRenderBBoxResources()
{
	WireframeCube = GetPrimitiveFactory()->GetWireframeCube();
	WireframeCubeD3D11 = new LXPrimitiveD3D11();
	WireframeCubeD3D11->Create(WireframeCube.get());
}

LXRenderBBoxResources::~LXRenderBBoxResources()
{
	delete WireframeCubeD3D11;
}

