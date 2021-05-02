//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
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

#define CREATE_OWNED_OBJECT(name, OwnerClass)						\
if (objectClassName == L#name)								\
{															\
	OwnerClass* dcOwner = dynamic_cast<OwnerClass*>(owner); \
	CHK(dcOwner);											\
	return new name(dcOwner);								\
}															\

#define CREATE_SHARED_OBJECT(name) \
if (objectClassName == L#name) return std::make_shared<name>();

#define CREATE_SHARED_OWNED_OBJECT(name, OwnerClass)						\
if (objectClassName == L#name)								\
{															\
	OwnerClass* dcOwner = dynamic_cast<OwnerClass*>(owner); \
	CHK(dcOwner);											\
	return std::make_shared<name>(dcOwner);						\
}															\


LXSmartObject* LXObjectFactory::CreateObject(const LXString& objectClassName, LXSmartObject* owner )
{
	CREATE_OBJECT(LXConnection);
	CREATE_OBJECT(LXNodeTemplate);
		
	CREATE_OWNED_OBJECT(LXConnector, LXNode)
	CREATE_OWNED_OBJECT(LXNode, LXGraph)
			
	LogW(LXObjectFactory, L"Unable to create object of class %s owned by %s (%s).", objectClassName.GetBuffer(), owner->GetName().GetBuffer(), owner->GetObjectName().GetBuffer());
	
	return nullptr;
}

std::shared_ptr<LXSmartObject> LXObjectFactory::CreateSharedObject(const LXString& objectClassName, LXSmartObject* owner)
{
	CREATE_SHARED_OBJECT(LXConnection);
	CREATE_SHARED_OBJECT(LXNodeTemplate);

	CREATE_SHARED_OWNED_OBJECT(LXConnector, LXNode)
	CREATE_SHARED_OWNED_OBJECT(LXNode, LXGraph)

	LogW(LXObjectFactory, L"Unable to create object of class %s owned by %s (%s).", objectClassName.GetBuffer(), owner->GetName().GetBuffer(), owner->GetObjectName().GetBuffer());

	return nullptr;
}
