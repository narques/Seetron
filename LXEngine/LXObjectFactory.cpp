//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXObjectFactory.h"
#include "LXConnection.h"
#include "LXConnector.h"
#include "LXNode.h"
#include "LXGraph.h"
#include "LXMemory.h" // --- Must be the last included ---

LXObjectFactory::LXObjectFactory()
{
}

LXObjectFactory::~LXObjectFactory()
{
}

#define CREATE_OBJECT(name) \
if (objectClassName == L#name) return new name();

LXSmartObject* LXObjectFactory::CreateObject(const LXString& objectClassName, LXSmartObject* owner )
{
	CREATE_OBJECT(LXNodeTemplate);
	//CREATE_OBJECT(LXNode);

	if (objectClassName == L"LXNode")
	{
		LXGraph* graph = dynamic_cast<LXGraph*>(owner);
		CHK(graph);
		return new LXNode(graph);
	}



	//CREATE_OBJECT(LXConnector);
	CREATE_OBJECT(LXConnection);

	if (objectClassName == L"LXConnector")
	{
		LXNode* node = dynamic_cast<LXNode*>(owner);
		CHK(node);
		return new LXConnector(node);
	}
	
	LogW(LXObjectFactory, L"Unable to create object of class %s owned by %s (%s).", objectClassName.GetBuffer(), owner->GetName().GetBuffer(), owner->GetObjectName().GetBuffer());
	
	return nullptr;
}
