//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2016 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXRenderPassDynamicTexture.h"
#include "LXTextureD3D11.h"
#include "LXRenderer.h"
#include "LXRenderTargetViewD3D11.h"
#include "LXRenderCommandList.h"
#include "LXShaderManager.h"
#include "LXPrimitiveD3D11.h"
#include "LXCore.h"
#include "LXProject.h"
#include "LXAssetManager.h"
#include "LXMaterial.h"
#include "LXDirectX11.h"
#include "LXRenderCluster.h"
#include "LXBitmap.h"
#include "LXProceduralTexture.h"
#include "LXMaterialD3D11.h"
#include "LXConstantBufferD3D11.h"
#include "LXConsoleManager.h"
#include "LXMath.h"
#include "LXRenderer.h"
#include "LXRenderClusterManager.h"
#include "LXMemory.h" // --- Must be the last included ---

bool gResetSimulation = false;
LXConsoleCommandT<bool> CCResetSimulation(L"ResetSimulation", &gResetSimulation);

LXMaterialD3D11* LXRenderPassDynamicTexture::GetMaterialD3D11(LXRenderer* Renderer, LXMaterial* InMaterial)
{
// 	LXMaterialD3D11* MaterialD3D11 = nullptr;
// 
// 	if (!InMaterial)
// 		InMaterial = GetCore().GetDefaultMaterial();
// 
// 	MaterialD3D11 = InMaterial->D3D11Material;
// 
// 	if (!MaterialD3D11)
// 	{
// 		MaterialD3D11 = new LXMaterialD3D11(InMaterial);
// 		Renderer->RenderClusterManager->ListMaterials.push_back(MaterialD3D11);
// 		InMaterial->D3D11Material = MaterialD3D11;
// 	}
// 
// 	return MaterialD3D11;
	return nullptr;
}

LXRenderPassDynamicTexture::LXRenderPassDynamicTexture(LXRenderer* InRenderer) :LXRenderPass(InRenderer)
{
	// ConstantBuffers
	CBTime = new LXConstantBufferD3D11();
	LXConstantBufferData2 CB;
	CBTime->CreateConstantBuffer(&CB, sizeof(LXConstantBufferData2));
}

LXRenderPassDynamicTexture::~LXRenderPassDynamicTexture()
{
	for (LXRenderClusterJob* RenderClusterJob : ListRenderClusters)
	{
		delete RenderClusterJob;
	}

	delete CBTime;
}

void LXRenderPassDynamicTexture::Render(LXRenderCommandList* RCL)
{
	if (!GetCore().GetProject())
		return;

	if (ListRenderClusters.size() == 0)
		return;

	const LXTime& time = GetCore().Time;
		
	if (gResetSimulation)
	{
		ItStart = ListRenderClusters.begin();
		for (LXRenderClusterJob* RenderCluster : ListRenderClusters)
		{
			RenderCluster->Index = 0;
		}
		gResetSimulation = false;
	}

	if (ItStart == ListRenderClusters.end())
		return;

	RCL->BeginEvent(L"DynamicTexture");

	// Update Time
	static LXConstantBufferData2 CB;
	CB.Time = (float)time.DeltaTime();
	RCL->UpdateSubresource4(CBTime->D3D11Buffer, &CB);

	// Render
	for (auto It = ItStart; It != ListRenderClusters.end(); It++)
	{
		(*It)->Render(RCL, CBTime);
	}

	// Increment only the Start iterator.
	(*ItStart)->Index++;
	if ((*ItStart)->Index >= (*ItStart)->Iteration)
		ItStart++;
				
	RCL->EndEvent();
}

void LXRenderPassDynamicTexture::Resize(uint Width, uint Height)
{
}

LXRenderClusterJob* LXRenderPassDynamicTexture::TraverseNode(LXGraphNode* LinkedNode, LXRenderClusterJob* LinkedJob, LXGraphNode* Node )
{
	if (MapRenderClusters.find(Node) != MapRenderClusters.end())
	{
		// Node already added as RenderCluster
		CHK(0);
		return nullptr;
	}

// 	if (LXGraphNodeMaterial* NodeMaterial = dynamic_cast<LXGraphNodeMaterial*>(Node))
// 	{
// 		LXRenderClusterJobTexture* RenderClusterJobTexture = new LXRenderClusterJobTexture(Renderer, NodeMaterial->Material);
// 
// 		// Parameters
// 		CHK(0 && "Revoir tout ce truc")
// 		//RenderClusterJobTexture->Material->ListTextures.push_back(TextureD3D11);
// 		
// 		DXGI_FORMAT Format = DXGI_FORMAT_UNKNOWN;
// 		switch (Texture->GetInsternalFormat())
// 		{
// 		case ETextureFormat::LX_R16_USHORT: Format = DXGI_FORMAT_R16_UNORM; break;
// 		case ETextureFormat::LX_R16G16_USHORT: Format = DXGI_FORMAT_R16G16_UNORM; break;
// 		case ETextureFormat::LX_R16G16_FLOAT: Format = DXGI_FORMAT_R16G16_FLOAT; break;
// 		case ETextureFormat::LX_R32G32_FLOAT: Format = DXGI_FORMAT_R32G32_FLOAT; break;
// 		case ETextureFormat::LX_RGBA8: Format = DXGI_FORMAT_B8G8R8A8_UNORM;  break;
// 		default: CHK(0 && "Bad texture format");
// 			break;
// 		}
// 		
// 		RenderClusterJobTexture->TextureOutput = new LXTextureD3D11(Texture->GetWidth(), Texture->GetHeight(), Format);;
// 		RenderClusterJobTexture->RenderTarget = new LXRenderTargetViewD3D11(RenderClusterJobTexture->TextureOutput);
// 		RenderClusterJobTexture->Iteration = NodeMaterial->Iteration;
// 		
// 		// Traverse links
// 		if (NodeMaterial->From)
// 		{
// 			LXRenderClusterJob* PreviousNode = TraverseNode(Node, RenderClusterJobTexture, NodeMaterial->From);
// 		}
// 
// 		MapRenderClusters[Node] = RenderClusterJobTexture;
// 		ListRenderClusters.push_back(RenderClusterJobTexture);
// 
// 		return RenderClusterJobTexture;
// 
// 	}
// 	else
		CHK(0);

	return nullptr;
}

LXTextureD3D11* LXRenderPassDynamicTexture::AddGraph(LXTexture* InTexture)
{
// 	Texture = InTexture;
// 
// 	LXGraphTexture* Graph = static_cast<LXGraphTexture*>(Texture->GetGraph());
// 	CHK(Graph);
// 
// 	const set<LXGraphNodeOutput*>& Outputs = Graph->GetOutput2();
// 
// 	for (LXGraphNodeOutput* Output : Outputs)
// 	{
// 		if (LXGraphNodeOutputTexture* OutputTexture = dynamic_cast<LXGraphNodeOutputTexture*>(Output))
// 		{
// 			CHK(Texture == OutputTexture->Texture);
// 
// 			// Create the final texture
// 			DXGI_FORMAT Format = DXGI_FORMAT_UNKNOWN;
// 			switch (Texture->GetInsternalFormat())
// 			{
// 			case ETextureFormat::LX_R16_USHORT: Format = DXGI_FORMAT_R16_UNORM; break;
// 			case ETextureFormat::LX_R16G16_USHORT: Format = DXGI_FORMAT_R16G16_UNORM; break;
// 			case ETextureFormat::LX_R16G16_FLOAT: Format = DXGI_FORMAT_R16G16_FLOAT; break;
// 			case ETextureFormat::LX_R32G32_FLOAT: Format = DXGI_FORMAT_R32G32_FLOAT; break;
// 			case ETextureFormat::LX_RGBA8: Format = DXGI_FORMAT_B8G8R8A8_UNORM;  break;
// 			default: CHK(0 && "Bad texture format");
// 				break;
// 			}
// 			TextureD3D11 = new LXTextureD3D11(Texture->GetWidth(), Texture->GetHeight(), Format);
// 
// 			// Pass trough the "Parent" node(s)
// 			LXGraphNode* Node = OutputTexture->GetNodeIn();
// 			CHK(Node);
// 			TraverseNode(OutputTexture, nullptr, Node);
// 		}
// 		else if (LXGraphNodeOutputBitmap* OutputBitmap = dynamic_cast<LXGraphNodeOutputBitmap*>(Output))
// 		{
// 
// 			LXRenderClusterJobBitmap* RenderClusterJobBitmap = new LXRenderClusterJobBitmap();
// 			ListRenderClusters.push_back(RenderClusterJobBitmap);
// 		}
// 		else
// 			CHK(0);
// 	}
// 
// 	ItStart = ListRenderClusters.begin();
// 
// 	CHK(TextureD3D11);
// 	return TextureD3D11;

	return nullptr;

}


LXRenderClusterJobTexture::LXRenderClusterJobTexture(LXRenderer* Renderer, LXMaterial* InMaterial)
{
	Material = LXRenderPassDynamicTexture::GetMaterialD3D11(Renderer, InMaterial);
	Primitive = Renderer->SSTriangle;

	// Retrieve the shader according the usage context and/or material
	Renderer->SetShaders(this);
}

LXRenderClusterJobTexture::~LXRenderClusterJobTexture()
{
	LX_SAFE_DELETE(RenderTarget);
}

void LXRenderClusterJobTexture::Render(LXRenderCommandList* RCL, LXConstantBufferD3D11* ConstantBuffer)
{
	CHK(0) // TODO, WE NEED THE RENDERPASS
#if 0
	RCL->OMSetRenderTargets2(RenderTarget, nullptr);
	RCL->CBViewProjection = nullptr;

	// Vertex Shader
	if (VertexShader)
	{
		RCL->IASetInputLayout(VertexShader);
		RCL->VSSetShader(VertexShader);
	}

	// Pixel Shader	
	if (PixelShader)
		RCL->PSSetShader(PixelShader);

	// Material (Shader resources)
	if (Material)
		Material->Render(RCL);

	if (ConstantBuffer)
		RCL->PSSetConstantBuffers(0, 1, ConstantBuffer);
	
	Primitive->Render(RCL);
	
	RCL->PSSetShader(nullptr);
	RCL->VSSetShader(nullptr);
	RCL->OMSetRenderTargets2(nullptr, nullptr);

// 	if(Material->Albedo)
// 		RCL->CopyResource(Material->Albedo->D3D11Texture2D, TextureOutput->D3D11Texture2D);
#endif

}

void LXRenderClusterJobBitmap::Render(LXRenderCommandList* RCL, LXConstantBufferD3D11* ConstantBuffer)
{
	// Copy the M
	// Copy to Bitmap
	// 		if (RenderCluster->TextureCapture && RenderCluster->BitmapOutput)
	// 		{
	// 			RCL->CopyResource(RenderCluster->TextureCapture->D3D11Texture2D, RenderCluster->TextureOutput->D3D11Texture2D);
	// 			RCL->CopyResourceToBitmap(RenderCluster->BitmapOutput, RenderCluster->TextureCapture->D3D11Texture2D);
	// 		}
	// 		*/
}

