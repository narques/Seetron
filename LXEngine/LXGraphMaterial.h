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

	void GetChildProperties(ListProperties& UserProperties) const override;
	LXTexture* GetTextureDisplacement(const LXString& textureName) const;
	bool GetFloatParameter(const LXString& textureName, float& outValue) const;

private:

	void OnPropertyChanged(LXProperty* property) override;

public:

	LXMaterial* Material = nullptr;
};






