//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXExports.h"
#include "LXString.h"

class LXCORE_API LXObject
{

public:
		 
	LXObject();
	LXObject(const LXObject& Object) = delete;
	LXObject& operator=(const LXObject& Object) = delete;
	virtual ~LXObject();

	LXString GetObjectName( ) const;

	static void	TraceAll( );
};

typedef std::list<LXObject*> ListObjects;
typedef std::vector<LXObject*> VectorObjects;
typedef std::set<LXObject*> SetObjects;
typedef std::map<LXString, int> MapObjects;