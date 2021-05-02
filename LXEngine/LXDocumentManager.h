//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include "LXFilepath.h"

class LXProject;
class LXFilepath;
class LXWindow;
class LXThread;

typedef std::stack<LXFilepath> StackFilepath;

class LXCORE_API LXDocumentManager : public LXObject
{
	
public:

	LXDocumentManager(void);
	virtual ~LXDocumentManager(void);

	LXProject*			LoadFile			( const LXFilepath& strFilepath, bool bThreaded = true );
	LXProject*			GetDocument			( ) const { return _Project; }
	void				SetDocument			( LXProject* doc ) { _Project = doc; }
	
	StackFilepath&		GetStackFile		( ) { return _FilesToLoad; }
	friend int			OpenFunc			( void* pData );

	void				SetWindow			( HWND hWnd );
	
public:

	LXWindow*			_pWindow = nullptr;

private:

	LXProject*			_Project = nullptr;
	StackFilepath		_FilesToLoad;
	LXThread*			_LoadingThread = nullptr;
};


