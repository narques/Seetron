//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXTraverser.h"

class LXFrustum;

class LXENGINE_API LXTraverserFrustumCulling : public LXTraverser
{

public:

	LXTraverserFrustumCulling(void);
	virtual ~LXTraverserFrustumCulling(void);

	// Overridden from LXTraverser
	virtual void		OnActor			( LXActor* pGroup );

	void				SetFrustum		( LXFrustum* pFrustum ) { _Frustum = pFrustum; }

protected:

	LXFrustum*			_Frustum = nullptr;
	
};

