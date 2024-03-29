//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXSmartObject.h"

typedef std::list<LXString> ListUID;

class LXENGINE_API LXSelection : public LXSmartObject
{

public:

	LXSelection();
	virtual ~LXSelection();

	//
	// Overridden from LXSmartObject
	//

	void						DefineProperties() ;
	virtual bool				OnSaveChild				( const TSaveContext& saveContext ) const override;
	virtual bool				OnLoadChild				( const TLoadContext& loadContext ) override;
	const SetSmartObjects&		Get						( ); 
	void						Set						( const SetSmartObjects& sps) { m_setSmartObjects = sps; }

	GetSet(LXString, _strDescription, Description);

protected:

	SetSmartObjects				m_setSmartObjects;
	ListUID						m_listUID;

};

typedef std::list<LXSelection*> ListSelections;
