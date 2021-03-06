//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include "LXProperty.h"

class LXCORE_API LXCommand : public LXObject
{

public:

	LXCommand(void);
	virtual ~LXCommand(void);
	
	virtual bool Do() = 0;
	virtual bool Undo() = 0;
	
	void SetDescription( const LXString& strDescrition ) { _Description = strDescrition; }
	const LXString& GetDescription( ) const { return _Description; }

private:

	LXString _Description;
};

typedef list<LXCommand*> ListCommands;
