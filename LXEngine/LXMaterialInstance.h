//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXMaterialBase.h"
#include "LXOverride.h"

class LXTexture;

class LXCORE_API LXMaterialInstance : public LXMaterialBase, public LXOverride<LXMaterial>
{

public:

	LXMaterialInstance();
	~LXMaterialInstance();
	
	// Helper
	static shared_ptr<LXMaterialInstance> Create(const shared_ptr<LXMaterial>& material);

	// Overridden from LXAsset
	virtual bool Load();
	virtual LXString GetFileExtension() override { return LX_MATERIALINSTANCE_EXT; }

	// Overridden from LXMaterialBase
	virtual bool Compile() override;
	virtual LXString GetShaderBaseName() const override;
	virtual bool IsTransparent() const override;
	virtual bool GetFloatParameter(const LXString& textureName, float& outValue) const override;
	virtual bool SetFloatParameter(const LXString& textureName, float value) const override;
	virtual EMaterialType GetType() const override;
		
	// Create and set an override
	bool SetPropertyTextureByName(const LXString& textureName, const shared_ptr<LXTexture>& texture);

private:

	int InstanceID = -1;
};

