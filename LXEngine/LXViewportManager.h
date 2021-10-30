//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXCore/LXObject.h"
#include "LXViewport.h"

class LXENGINE_API LXViewportManager : public LXObject
{

	friend class LXViewport;
	
public:

	LXViewportManager(void);
	virtual ~LXViewportManager(void);

	const ListViewports&	GetListViewport(	) const { return _listViewports; }
	LXViewport*				GetViewport(	) const;
	
private:

	void					AddViewport( LXViewport* viewport ) { _listViewports.push_back(viewport); }
	void					RemoveViewport( LXViewport* viewport ) { _listViewports.remove(viewport); }

protected:
	 
	ListViewports			_listViewports;
};
