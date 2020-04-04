//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXAsset.h"
#include "LXMaterialNode.h"

class LXShader;
class LXGraphMaterial;
class LXMaterialD3D11;
class LXTexture;

enum class EMaterialType // ! DataModel 
{
	MaterialTypeStandard,
	MaterialTypeTexture
};

enum class EMaterialLightingModel
{
	Unlit,
	Lit,
	Transparent
};

class LXCORE_API LXMaterial : public LXAsset
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
	bool Compile();

	// Misc
	
	bool IsTransparent() const { return _LightingModel == EMaterialLightingModel::Transparent; }
	bool GetTwoSided() const { return _bTwoSided; }
	void SetTwoSided(bool b) { _bTwoSided = b; }
	LXTexture* GetTextureDisplacement(const LXString& textureName) const;
	LXPropertyAssetPtr* GetPropertyTextureByName(const LXString& textureName) const;
	bool GetFloatParameter(const LXString& textureName, float& outValue) const;
	EMaterialLightingModel GetLightingModel() const { return _LightingModel; }

	// Graph
	
	LXGraphMaterial* GetGraph() const;
	void ReleaseGraph();

	//
	// Rendering
	//

	const LXMaterialD3D11* GetDeviceMaterial() const { return _materialD3D11; }
	void SetDeviceMaterial(LXMaterialD3D11* materialD3D11) { _materialD3D11 = materialD3D11; }

	void DefineProperties();
	void CreateDeviceMaterial();
	void ReleaseDeviceMaterial();
	
public:

	// Misc
	EMaterialType MaterialType;

	// Delegates & Events
	
	// Fired when the material device is created (including the Graph to HLSL translation)
	LXDelegate<> Compiled;

private:

	// Graph 
	unique_ptr<LXGraphMaterial> GraphMaterial;

	EMaterialLightingModel _LightingModel = EMaterialLightingModel::Lit;
	bool  _bTwoSided = false;

	// Rendering
	LXMaterialD3D11* _materialD3D11 = nullptr;
};

