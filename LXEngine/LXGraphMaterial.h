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

	virtual void AddNode(std::shared_ptr<LXNode>& node) override;
	virtual void RemoveNode(std::shared_ptr<LXNode>& node) override;

	void GetChildProperties(ListProperties& UserProperties) const override;
	bool GetFloatParameter(const LXString& paramName, float& outValue) const;
	bool SetFloatParameter(const LXString& paramName, float value) const;

private:

	void OnLoaded() override;
	void OnPropertyChanged(LXProperty* property) override;

	// Create a transient property to simplify the node property access and allows overrides.
	void Register(const std::shared_ptr<LXNode>& node);
	void Unregister(const std::shared_ptr<LXNode>& node);

public:

	LXMaterial* Material = nullptr;
};






