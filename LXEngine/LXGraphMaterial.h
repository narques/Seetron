//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXGraph.h"

class LXNode;
class LXNodeTemplate;
class LXTexture;

class LXCORE_API LXGraphMaterial : public LXGraph
{

public:

	LXGraphMaterial(LXMaterial* material);
	virtual ~LXGraphMaterial();

	virtual void AddNode(LXNode* node) override;
	virtual void RemoveNode(LXNode* node) override;

	void GetChildProperties(ListProperties& UserProperties) const override;
	LXTexture* GetTextureDisplacement(const LXString& textureName) const;
	LXPropertyAssetPtr* GetPropertyTextureByName(const LXString& textureName) const;
	bool GetFloatParameter(const LXString& textureName, float& outValue) const;

private:

	void OnLoaded() override;
	void OnPropertyChanged(LXProperty* property) override;

public:

	LXMaterial* Material = nullptr;

private:

	multimap<LXString, LXPropertyAssetPtr*> _assetTextures;
};






