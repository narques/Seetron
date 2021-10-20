//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

class LXENGINE_API LXObject
{

public:
		 
	LXObject();
	LXObject(const LXObject& Object) = delete;
	LXObject& operator=(const LXObject& Object) = delete;
	virtual ~LXObject();

	LXString GetObjectName( ) const;

#if LX_TRACE_OBJECTS
	static void	TraceAll( );
#endif

};

typedef std::list<LXObject*> ListObjects;
typedef std::vector<LXObject*> VectorObjects;
typedef std::set<LXObject*> SetObjects;
typedef std::map<LXString, int> MapObjects;