//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXAsset.h"
#include "LXMaterialNode.h"

class LXShader;
class LXGraphMaterial;

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

	
	//
	// Overridden From LXAsset
	//

	LXString GetFileExtension() override { return LX_MATERIAL_EXT; }
	bool Load() override;
	
	// Misc
	bool IsTransparent() const { return _LightingModel == EMaterialLightingModel::Transparent; }
	bool GetTwoSided() const { return _bTwoSided; }
	void SetTwoSided(bool b) { _bTwoSided = b; }
	EMaterialLightingModel GetLightingModel() const { return _LightingModel; }

	// Graph
	LXGraphMaterial* GetGraph() const;
	void ReleaseGraph();

private:
	
	void DefineProperties();

public:

	// Misc
	EMaterialType MaterialType;

private:

	// Graph 
	unique_ptr<LXGraphMaterial> GraphMaterial;

	EMaterialLightingModel _LightingModel = EMaterialLightingModel::Lit;
	bool  _bTwoSided = false;
};

