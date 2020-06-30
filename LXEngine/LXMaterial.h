//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXMaterialBase.h"
#include "LXMaterialNode.h"

class LXShader;
class LXGraphMaterial;
class LXTexture;

enum class EMaterialLightingModel
{
	Unlit,
	Lit,
	Transparent
};

class LXCORE_API LXMaterial : public LXMaterialBase
{

public:

	LXMaterial(EMaterialType InMaterialType = EMaterialType::MaterialTypeStandard);
	virtual ~LXMaterial(void);
	
	//
	// Overridden From LXSmartObject
	//

	void GetChildProperties(ListProperties& UserProperties) const override;
	void OnPropertyChanged(LXProperty* pProperty) override;
	
	//
	// Overridden From LXAsset
	//

	LXString GetFileExtension() override { return LX_MATERIAL_EXT; }
	bool Load() override;
	bool Reload() override;
	bool Compile() override;

	// Misc
	
	void DefineProperties();
	virtual bool IsTransparent() const override { return _LightingModel == EMaterialLightingModel::Transparent; }
	bool GetTwoSided() const { return _bTwoSided; }
	void SetTwoSided(bool b) { _bTwoSided = b; }
	const LXPropertyAssetPtr* GetPropertyTextureByName(const LXString& textureName) const;
	virtual bool GetFloatParameter(const LXString& textureName, float& outValue) const override;
	EMaterialLightingModel GetLightingModel() const { return _LightingModel; }
	virtual EMaterialType GetType() const override { return MaterialType; }

	// Graph
	
	LXGraphMaterial* GetGraph() const;
	void ReleaseGraph();

	// Rendering
	virtual LXString GetShaderBaseName() const override;

public:

	int InstanceCount = 0;

private:

	EMaterialType MaterialType;

	// Graph 
	unique_ptr<LXGraphMaterial> GraphMaterial;

	EMaterialLightingModel _LightingModel = EMaterialLightingModel::Lit;
	bool  _bTwoSided = false;
};

