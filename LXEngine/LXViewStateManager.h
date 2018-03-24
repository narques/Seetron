//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXManager.h"
#include "LXViewState.h"
#include "LXObserver.h"

class LXCORE_API LXViewStateManager : public LXManager2
{

public:

	LXViewStateManager(LXProject* pDocument);
	virtual ~LXViewStateManager(void);

	// Overridden from LXSmartObject
	virtual bool	OnSaveChild						( const TSaveContext& saveContext  ) override;
	virtual bool	OnLoadChild						( const TLoadContext& loadContext ) override;
	
	// Overridden from LXManager2
	virtual void	GetObjects						( ListObjects& listObject ) override;

	// Misc	
	void			AddViewState					( LXViewState* pViewstate );
	const			ListViewStates& GetListViewStates(	) const { return _listViewStates; }
	LXViewState*	CreateViewState					( LXProject* pDocument );
	LXViewState*	GetViewState					( const LXString& name );

private:

	LXProject*		_Project = nullptr;
	ListViewStates	_listViewStates;
};
