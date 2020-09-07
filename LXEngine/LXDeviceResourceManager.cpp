//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXDeviceResourceManager.h"

// Seetron
#include "LXCore.h"
#include "LXMaterialBase.h"
#include "LXMaterialD3D11.h"
#include "LXPrimitive.h"
#include "LXPrimitiveD3D11.h"

template<class T>
void hash_combine(std::size_t& seed, const T& data)
{
	std::hash<T> h;
	seed ^= h(data) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

size_t BuildKey(ERenderPass renderPass, const LXMaterialBase* material)
{
	std::size_t seed = 0;
	hash_combine(seed, (int)renderPass);
	hash_combine(seed, (uint64)material);
	return seed;
}

const shared_ptr<LXMaterialD3D11>& LXDeviceResourceManager::GetShaderResources(ERenderPass renderPass, const LXMaterialBase* material)
{
	size_t key = BuildKey(renderPass, material);

	map<size_t, shared_ptr<LXMaterialD3D11>>::iterator it = _shaderResources.find(key);
	if (it != _shaderResources.end())
	{
		return it->second;
	}
	else
	{
		shared_ptr<LXMaterialD3D11> materialD3D11 = LXMaterialD3D11::CreateFromMaterial(material);
		_shaderResources[key] = materialD3D11;
		GetCore().EnqueueInvokeDelegate(&material->Compiled);
		return _shaderResources[key];
	}
}

void LXDeviceResourceManager::RemoveShaderResources(const LXMaterialBase* material)
{
	for (auto i = 0; i < (int)ERenderPass::Last; i++)
	{
		size_t key = BuildKey((ERenderPass)i, material);
		_shaderResources.erase(key);
	}
}

void LXDeviceResourceManager::UpdateShaderResources(const LXMaterialBase* material)
{
	for (auto i = 0; i < (int)ERenderPass::Last; i++)
	{
		size_t key = BuildKey((ERenderPass)i, material);
		map<size_t, shared_ptr<LXMaterialD3D11>>::iterator it = _shaderResources.find(key);
		if (it != _shaderResources.end())
		{
			it->second->Update(material);
		}
	}
}

const std::shared_ptr<LXPrimitiveD3D11>& LXDeviceResourceManager::GetPrimitive(const LXPrimitive* primitive)
{
	auto It = _primitives.find(pair<const LXPrimitive*, uint>(primitive, /*ArrayInstancePosition ? (uint)ArrayInstancePosition->size() : */0));

	if (It != _primitives.end())
	{
		return It->second;
	}
	else
	{
		const auto Key = pair<const LXPrimitive*, uint>(primitive, /*ArrayInstancePosition ? (uint)ArrayInstancePosition->size() : */(uint)0);
		_primitives[Key] = make_shared<LXPrimitiveD3D11>();
		_primitives[Key]->Create(primitive, /*ArrayInstancePosition*/ nullptr);
		return _primitives[Key];
	}
}

void LXDeviceResourceManager::Run()
{
	CHK(IsRenderThread());
	DeleteUnusedPrimitives();
	DeleteUnusedResources();
}

void LXDeviceResourceManager::DeleteUnusedResources()
{
	for (auto It = _shaderResources.begin(); It != _shaderResources.end();)
	{
		if (It->second.use_count() == 1)
		{
			It = _shaderResources.erase(It);
		}
		else
		{
			It++;
		}
	}
}

void LXDeviceResourceManager::DeleteUnusedPrimitives()
{
	for (auto It = _primitives.begin(); It != _primitives.end();)
	{
		if (It->second.use_count() == 1)
		{
			It = _primitives.erase(It);
		}
		else
		{
			It++;
		}
	}
}