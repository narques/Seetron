//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXSmartObject.h"
#include "LXColor.h"
#include "LXFlags.h"

class LXTexture;
class LXString;

const wchar_t kMaterrialNode_ValuePropertyName[] = L"Value";
const wchar_t kMaterialNode_TexturePropertyName[] = L"Texture";

enum class EShader // DataModel
{
	UndefinedShader = 0,
	VertexShader = LX_BIT(0),
	HullShader = LX_BIT(1),
	DomainShader = LX_BIT(2),
	GeometryShader = LX_BIT(3),
	PixelShader = LX_BIT(4)
};

typedef LXFlags<EShader> EShaderFlags;

enum class EMaterialLink
{
	NotLinked,
	LinkToDiplacement,
	LinkToColor
};

enum class EMaterialNodeType
{
	MaterialInputTypeFloat,
	MaterialInputTypeColor,
	MaterialInputTypeTextureSampler
};

class LXCORE_API LXMaterialNode : public LXSmartObject
{

public:

	LXMaterialNode(LXMaterial* InMaterial, EMaterialNodeType InType);
	virtual ~LXMaterialNode();
	void OnPropertyChanged(LXProperty* pProperty) override;
	EMaterialNodeType Type;
	EMaterialLink Link = EMaterialLink::LinkToColor;
protected:
	LXMaterial* Material = nullptr;
};


//------------------------------------------------------------------------------------------------------

class LXMaterialNodeFloat : public LXMaterialNode
{
public:
	LXMaterialNodeFloat(LXMaterial* InMaterial, const float& InValue);
	virtual ~LXMaterialNodeFloat();
	void OnPropertyChanged(LXProperty* pProperty) override;
	static EMaterialNodeType GetType() { return EMaterialNodeType::MaterialInputTypeFloat; }
	float Value;
};

//------------------------------------------------------------------------------------------------------

class LXMaterialNodeColor : public LXMaterialNode
{
public:
	LXMaterialNodeColor(LXMaterial* InMaterial, const LXColor4f& InColor);
	virtual ~LXMaterialNodeColor();
	void OnPropertyChanged(LXProperty* pProperty) override;
	static EMaterialNodeType GetType() { return EMaterialNodeType::MaterialInputTypeColor; }
	LXColor4f Value;

};

//------------------------------------------------------------------------------------------------------

class LXMaterialNodeTextureSampler : public LXMaterialNode
{
public:
	LXMaterialNodeTextureSampler(LXMaterial* InMaterial, LXTexture* InTexture, EShaderFlags InAffectedShaders, uint InSlot);
	virtual ~LXMaterialNodeTextureSampler();
	static EMaterialNodeType GetType() { return EMaterialNodeType::MaterialInputTypeTextureSampler; }
	EShaderFlags AffectedShaders;
	uint Slot;
	LXTexture* GetTexture() const { return Texture; }
private:
	LXTexture* Texture;
};

//------------------------------------------------------------------------------------------------------

template <typename T>
static T* CastMaterialNode(const LXMaterialNode* MI)
{
	return MI?T::GetType() == MI->Type ? (T*)MI : nullptr: nullptr;
}






