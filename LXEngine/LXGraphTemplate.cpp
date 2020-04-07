//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXGraphTemplate.h"
#include "LXNode.h"
#include "LXMemory.h" // --- Must be the last included --- 

LXGraphTemplate::LXGraphTemplate()
{
	DefineProperty("Nodes", (ArraySmartObjects*)&NodeTemplates);
}

LXGraphTemplate::~LXGraphTemplate()
{
	for (LXNodeTemplate* nodeTemplate : NodeTemplates)
	{
		delete nodeTemplate;
	}
}

const LXNodeTemplate* LXGraphTemplate::GetNodeTemplate(const LXString& templateID) const
{
	for (const LXNodeTemplate* nodeTemplate : NodeTemplates)
	{
		if (nodeTemplate->GetName() == templateID)
		{
			return nodeTemplate;
		}
	}

	CHK(0);
	return nullptr;
}

bool LXGraphTemplate::OnLoadChild(const TLoadContext& loadContext)
{
	return true;
}
