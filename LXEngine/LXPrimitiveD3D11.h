//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXCore/LXObject.h"
#include "LXVec3.h"
#include "LXDirectX11.h"
#include "LXInputElementDescD3D11Factory.h"
#include <directxmath.h>
using namespace DirectX;

struct ID3D11Buffer;
class LXPrimitive;
class LXRenderCommandList;

//#define INDEXTYPE_USHORT

class LXPrimitiveD3D11 : public LXObject
{

public:

	LXPrimitiveD3D11();
	virtual ~LXPrimitiveD3D11();

	void Render(LXRenderCommandList* RCL);

	///
	/// Create the buffers
	/// \param ArrayInstancePosition an optional per instance array of position
	
	bool Create(const LXPrimitive* Primitive, const ArrayVec3f* ArrayInstancePosition = nullptr);
	bool CreateSSTriangle();
	bool CreateLine(const vec3f& v0, const vec3f& v1);

	// Helpers
	static std::shared_ptr<LXPrimitiveD3D11> CreateFromPrimitive(const LXPrimitive* primitive);

private:

#ifdef INDEXTYPE_USHORT
	bool CreateIndexBuffer(unsigned short* Indices, UINT InIndexCount);
#else
	bool CreateIndexBuffer(unsigned int* Indices, UINT InIndexCount);
#endif
	bool CreateVertexBuffer(void* Vertices, UINT VertexStructSize, UINT InVertexCount);
	bool CreateInstanceBuffer(const ArrayVec3f& ArrayInstancePosition);
		
public:

	ID3D11Buffer* IndexBuffer = nullptr;
	ID3D11Buffer* VertexBuffer = nullptr;
	ID3D11Buffer* InstanceBuffer = nullptr;
		
	UINT IndexCount = 0;
	UINT VertexCount = 0;
	UINT VertexStride = 0; // Size of the used vertex struct
	UINT VertexBufferOffset = 0;
	
	UINT PrimitiveTopology = 0; // D3D_PRIMITIVE_TOPOLOGY_UNDEFINED
	
	LXArrayInputElementDesc* Layout2 = nullptr;
	int layoutMask = 0;

	UINT InstanceCount = 0;
	UINT InstanceBufferStride = 0;
	UINT InstanceBufferOffset = 0;
};

