//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXAsset.h"
#include "LXDelegate.h"

enum class EMaterialType // ! DataModel 
{
	MaterialTypeStandard,
	MaterialTypeTexture
};

class LXCORE_API LXMaterialBase : public LXAsset, public std::enable_shared_from_this<LXMaterialBase>
{

public:

	LXMaterialBase();
	virtual ~LXMaterialBase();

	// Rendering
	void ReleaseDeviceMaterials();

	virtual bool Compile() = 0;

	// Gives a unique name used to generates the shader filenames
	virtual LXString GetShaderBaseName() const = 0;

	virtual bool IsTransparent() const = 0;
	virtual bool GetFloatParameter(const LXString& textureName, float& outValue) const = 0;
	virtual bool SetFloatParameter(const LXString& textureName, float value) const = 0;
	virtual EMaterialType GetType() const = 0;

public:

	// Fired when the material device is created (including the Graph to HLSL translation)
	LXDelegate<> Compiled;
};

