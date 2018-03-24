//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include "LXViewport.h"
#include "LXObserver.h"

class LXCORE_API LXViewportManager : public LXObserverCommandManager
{

	friend class LXViewport;
	
public:

	LXViewportManager(void);
	virtual ~LXViewportManager(void);

	const ListViewports&	GetListViewport(	) const { return _listViewports; }
	LXViewport*				GetViewport(	) const;
	
	// Overridden from LXObserverCommandManager
	virtual void			OnChangeProperty( const LXProperty* pProperty );
	virtual void			OnChangeHighlight( LXActor* pActor );
	
	// Misc
	void					ClearRenderList( );
	
private:

	void					AddViewport( LXViewport* viewport ) { _listViewports.push_back(viewport); }
	void					RemoveViewport( LXViewport* viewport ) { _listViewports.remove(viewport); }

protected:
	 
	ListViewports			_listViewports;
};
