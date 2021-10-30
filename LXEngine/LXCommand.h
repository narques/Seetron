//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXCore/LXObject.h"
#include "LXProperty.h"

class LXENGINE_API LXCommand : public LXObject
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

typedef std::list<LXCommand*> ListCommands;
