//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXDocumentOwner.h"
#include "LXSmartObject.h"

class LXProject;
class LXMaterial;

class LXCORE_API LXManager : public LXDocumentOwner
{

public:

	LXManager (const LXProject* pDocument);
	virtual ~LXManager (void);
};

//----------------------------------------------------------------------------------//

class LXCORE_API LXManager2 : public LXSmartObject
{

public:

	LXManager2(){ }
	virtual ~LXManager2(){ }
	virtual void  GetObjects ( ListObjects& listObject ) = 0;

};

