//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXSmartObject.h"

class LXNodeTemplate;

class LXCORE_API LXGraphTemplate : public LXSmartObject
{

public:

	LXGraphTemplate();
	virtual ~LXGraphTemplate();
	std::vector<LXNodeTemplate*> NodeTemplates;

	const LXNodeTemplate* GetNodeTemplate(const LXString& templateID) const;

private:

	virtual bool OnLoadChild(const TLoadContext& loadContext) override;

	
};


