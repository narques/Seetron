//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

class LXCORE_API LXObject
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

typedef list<LXObject*> ListObjects;
typedef vector<LXObject*> VectorObjects;
typedef set<LXObject*> SetObjects;
typedef map<LXString, int> MapObjects;