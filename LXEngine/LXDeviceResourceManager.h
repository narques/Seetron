//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"

// Seetron
#include "LXRenderPassType.h"

class LXMaterialBase;
class LXMaterialD3D11;
class LXPrimitive;
class LXPrimitiveD3D11;
class LXShaderResources;

class LXDeviceResourceManager : public LXObject
{

public:

	// Returns the ShaderResources for the given RenderPass & Material.
	const shared_ptr<LXMaterialD3D11>& GetShaderResources(ERenderPass renderPass, const LXMaterialBase* material);

	// Remove & destroy the ShaderResources, regardless of the ref count.
	void RemoveShaderResources(const LXMaterialBase* material);

	// Update the resources 'properties' like the ConstantBuffers.
	void UpdateShaderResources(const LXMaterialBase* material);

	// Return the DevicePrimitive for the Give Primitive
	const shared_ptr<LXPrimitiveD3D11>& GetPrimitive(const LXPrimitive* primitive);
		   
	// Update
	void Run();

private:

	void DeleteUnusedResources();
	void DeleteUnusedPrimitives();

private:

	map<size_t, shared_ptr<LXMaterialD3D11>> _shaderResources;
	map<pair<const LXPrimitive*, uint>, shared_ptr<LXPrimitiveD3D11>> _primitives;

};

