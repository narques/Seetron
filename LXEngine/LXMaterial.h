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

class LXMaterialNode;
class LXMaterialNodeTextureSampler;
class LXShader;

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

	void GetChildProperties(ListProperties& UserProperties) override;
	
	//
	// Overridden From LXAsset
	//

	LXString GetFileExtension() override { return LX_MATERIAL_EXT; }
	bool Load() override;

	// Shader
	void SetVertexShader(const LXString& Filename);
	void SetPixelShader(const LXString& Filename);

	// Create nodes
	LXMaterialNode* AddTexture(const LXString& strFilepath, EShaderFlags Shaders, uint Slot);
	LXMaterialNode* AddTexture();
	LXMaterialNode* AddFloat();
	LXMaterialNode* AddColor(const LXColor4f& InColor);
	LXMaterialNode* AddColor();

	// Retrieve nodes
	LXMaterialNode* GetMaterialNodeFromName(const LXString& Name);
	LXMaterialNodeFloat* GetMaterialNodeFloatFromName(const LXString& Name);
	LXMaterialNodeTextureSampler* GetMaterialNodeTextureSamplerFromName(const LXString& Name);
	
	// Create nodes and attach them to the corresponding inputs
	void SetColor(const LXString& Filename, uint Slot);
	void SetColor(const LXColor4f& InColor);
	
	// Misc
	void GetTextures(list<LXTexture*>& listTextures);
	void GetTextureSamplers(list<LXMaterialNodeTextureSampler*>& listTextureSamplers) const;
	LXMaterialNode* GetMaterialNodeFromUID(const LXString& UID);
	const vector<LXMaterialNode*>& GetMaterialNodes() const { return MaterialNodes; }
	bool IsTransparent() const { return _LightingModel == EMaterialLightingModel::Transparent; }
	bool GetTwoSided() const { return _bTwoSided; }
	void SetTwoSided(bool b) { _bTwoSided = b; }

private:
	
	void DefineProperties();
	void AddMaterialNode(LXMaterialNode* MaterialNode);

public:

	// Misc
	EMaterialType MaterialType;
	
	// Nodes
	typedef vector<LXMaterialNode*> ArrayMaterialNodes;
	ArrayMaterialNodes MaterialNodes;
	// Asset Shaders
	LXShader* VertexShader = nullptr;
	LXShader* HullShader = nullptr;
	LXShader* DomainShader = nullptr;
	LXShader* GeometryShader = nullptr;
	LXShader* PixelShader = nullptr;
		
	// Material inputs
	LXMaterialNode* Color = nullptr;		// ColorRGB
	LXMaterialNode* Opacity = nullptr;		// Float
	LXMaterialNode* Normal = nullptr;		// VectorXYZ
	LXMaterialNode* Metallic = nullptr;		// Float
	LXMaterialNode* Roughness = nullptr;	// Float
	LXMaterialNode* Emissive = nullptr;		// ColorRGB
	LXMaterialNode* Displacement = nullptr;	// Float

private:

	EMaterialLightingModel _LightingModel = EMaterialLightingModel::Lit;
	bool  _bTwoSided = false;
};

