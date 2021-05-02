//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXPrimitiveD3D11.h"
#include "LXPrimitive.h"
#include "LXRenderCommandList.h"
#include "LXShaderD3D11.h"
#include "LXStatistic.h"
#include "LXInputElementDescD3D11Factory.h"
#include "LXMemory.h" // --- Must be the last included ---

LXPrimitiveD3D11::LXPrimitiveD3D11()
{
	LX_COUNTSCOPEINC(LXPrimitiveD3D11)
}

LXPrimitiveD3D11::~LXPrimitiveD3D11()
{
	LX_COUNTSCOPEDEC(LXPrimitiveD3D11)
	CHK(IsRenderThread());
	LX_SAFE_RELEASE(VertexBuffer);
	LX_SAFE_RELEASE(IndexBuffer);
	LX_SAFE_RELEASE(InstanceBuffer);
}

void LXPrimitiveD3D11::Render(LXRenderCommandList* RCL)
{
	LX_PERFOSCOPE(LXPrimitiveD3D11_Render)

	CHK(VertexCount);

	// InputAssembly & Draw
	
	RCL->IASetPrimitiveTopology(PrimitiveTopology);
	RCL->IASetVertexBuffer(this);
			
	if (IndexCount > 0)
	{
		RCL->IASetIndexBuffer(this);

		if (InstanceCount > 0)
		{
			RCL->DrawIndexedInstanced(IndexCount, InstanceCount, 0, 0);
		}
		else
		{
			RCL->DrawIndexed(IndexCount);
		}
		
		// Statistics
		RCL->DrawCallCount++;
		RCL->TriangleCount += IndexCount / 3;
	}
	else
	{
		if (InstanceCount > 0)
		{
			RCL->DrawInstanced(VertexCount, InstanceCount, 0, 0);
		}
		else
		{
			RCL->Draw(VertexCount, 0);
		}

		// Statistics
		RCL->DrawCallCount++;
		RCL->TriangleCount += VertexCount / 3;
	}
}

bool LXPrimitiveD3D11::Create(const LXPrimitive* Primitive, const ArrayVec3f* ArrayInstancePosition/* = nullptr*/)
{
	CHK(Primitive);
	CHK(!VertexBuffer);
	CHK(!IndexBuffer);

	// --- Create a temporary interleaved vertex buffer ---
	int mask = Primitive->GetMask();
	VertexCount = Primitive->GetVertices();
	int VertexStructSize = 0;
	void* Vertices = Primitive->CreateInterleavedVertexArray(mask, VertexCount, &VertexStructSize);
	if (Vertices == nullptr)
		return false;

	VertexStride = VertexStructSize;

	// --- Determine the D3D11 Layout to use according the mask ---

	// The Layout mask is different
	// Remove the indices bit
	layoutMask = mask & ~LX_PRIMITIVE_INDICES;
	
	// Add the InstancePosition bit
	if (ArrayInstancePosition && ArrayInstancePosition->size() > 0)
	{
		layoutMask |= LX_PRIMITIVE_INSTANCEPOSITIONS;
		InstanceCount  = (UINT)ArrayInstancePosition->size();
	}
	
	Layout2 = const_cast<LXArrayInputElementDesc*>(&GetInputElementDescD3D11Factory().GetInputElement(layoutMask));
	
	// --- Determine the primitive topology ---

	switch (Primitive->GetTopology())
	{
	case LX_TRIANGLES: PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; break;
	case LX_TRIANGLE_STRIP: PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP; break;
	case LX_3_CONTROL_POINT_PATCH: PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST; break;
	case LX_4_CONTROL_POINT_PATCH: PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST; break;
	case LX_LINES: PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST; break;
	default: CHK(0);
	}
		
	// --- Create the D3D11 vertex buffer
	CreateVertexBuffer(Vertices, VertexStructSize, VertexCount);
	delete Vertices;


	// --- Create the D3D11 instance buffer
	if (ArrayInstancePosition)
	{
		//InstanceBufferStride = sizeof(ArrayInstancePosition[0]);
		InstanceBufferStride = sizeof(vec3f);
		CreateInstanceBuffer(*ArrayInstancePosition);
	}
	
		
	// --- Create the D3D11 Index buffer ---
	if (Primitive->GetArrayIndices().size() > 0)
	{
		IndexCount = (UINT)Primitive->GetArrayIndices().size();
		
#ifdef INDEXTYPE_USHORT
		CHK(IndexCount <= UINT16_MAX);
		unsigned short* Indices = new unsigned short[IndexCount];
#else
		UINT* Indices = new UINT[IndexCount];
#endif

		for(unsigned int i = 0; i < IndexCount;i++)
		{
			Indices[i] = Primitive->GetArrayIndices()[i];
		}
		
		CreateIndexBuffer(Indices, IndexCount);
		delete Indices;
	}

	return true;
}

bool LXPrimitiveD3D11::CreateSSTriangle()
{
	Vertex_PT vertices[] = 
	{
		{ vec3f(-1.0f, 3.0f, 0.5f),  vec2f(0.f, -1.f) },
		{ vec3f(-1.0f, -1.0f, 0.5f), vec2f(0.f, 1.f) },
		{ vec3f(3.0f, -1.0f, 0.5f),  vec2f(2.f, 1.f) },
	};

	VertexCount = 3;
	VertexStride = sizeof(Vertex_PT);
	PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	CreateVertexBuffer(vertices, VertexStride, VertexCount);
	return true;
}

bool LXPrimitiveD3D11::CreateLine(const vec3f& v0, const vec3f& v1)
{
	Vertex_P vertices[] = { v0, v1 };
	VertexCount = 2;
	VertexStride = sizeof(Vertex_P);
	PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	CreateVertexBuffer(vertices, VertexStride, VertexCount);
	return true;
}

std::shared_ptr<LXPrimitiveD3D11> LXPrimitiveD3D11::CreateFromPrimitive(const LXPrimitive* primitive)
{
	std::shared_ptr<LXPrimitiveD3D11> primitiveD3D11 = std::make_shared<LXPrimitiveD3D11>();
	primitiveD3D11->Create(primitive);
	return primitiveD3D11;
}

#ifdef INDEXTYPE_USHORT
bool LXPrimitiveD3D11::CreateIndexBuffer(unsigned short* Indices,  UINT InIndexCount)
#else
bool LXPrimitiveD3D11::CreateIndexBuffer(UINT* Indices, UINT InIndexCount)
#endif
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;
#ifdef INDEXTYPE_USHORT
	bd.ByteWidth = sizeof(unsigned short) * InIndexCount;
#else
	bd.ByteWidth = sizeof(UINT) * InIndexCount;
#endif
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = Indices;
	
	HRESULT hr = LXDirectX11::GetCurrentDevice()->CreateBuffer(&bd, &InitData, &IndexBuffer);
	if (FAILED(hr))
	{
		CHK(0);
		return false;
	}
	else
		return true;
	
}

bool LXPrimitiveD3D11::CreateVertexBuffer(void* Vertices, UINT VertexStructSize, UINT InVertexCount)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = VertexStructSize * InVertexCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = Vertices;
	
	HRESULT hr = LXDirectX11::GetCurrentDevice()->CreateBuffer(&bd, &InitData, &VertexBuffer);
	if (FAILED(hr))
	{
		CHK(0);
		return false;
	}
	else
		return true;
}

bool LXPrimitiveD3D11::CreateInstanceBuffer(const ArrayVec3f& ArrayInstancePosition)
{
	D3D11_BUFFER_DESC bd = { 0 };

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = (UINT)(sizeof(ArrayInstancePosition[0]) * ArrayInstancePosition.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &ArrayInstancePosition[0];

	HRESULT hr = LXDirectX11::GetCurrentDevice()->CreateBuffer(&bd, &InitData, &InstanceBuffer);
	if (FAILED(hr))
	{
		CHK(0);
		return false;
	}
	else
		return true;
}