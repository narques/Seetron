//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"

class LXProject;
class LXQuery;
class LXQueryManager;
class LXSelectionManager;

class LXCORE_API LXDocumentOwner : public virtual LXObject
{

public:

	LXDocumentOwner();
	LXDocumentOwner(const LXProject* pDocument);
	virtual ~LXDocumentOwner(void);

	LXProject*			GetDocument( ) { return _pDocument; }
	virtual	void		SetDocument( LXProject* pDocument ) { _pDocument = pDocument;}

	// Managers
	LXQueryManager*		GetQueryManager( ) const;
	LXQuery*			GetCurrentQuery( ) const;

	LXSelectionManager*	GetSelectionManager( ) const;
	

protected:

	LXProject*			_pDocument;

};
