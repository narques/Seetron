//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXTraverserFrustumCulling.h"
#include "LXActor.h"

class LXFrustum;
class LXRenderer;

class LXCORE_API LXRectangularSelectionTraverser : public LXTraverserFrustumCulling
{

public:

	LXRectangularSelectionTraverser(void);
	virtual ~LXRectangularSelectionTraverser(void);

	// Overridden from LXTraverser
	virtual void		Apply			( ) override;
	virtual void		OnActor			( LXActor* pActor )override;
	virtual void		OnPrimitive		( LXActorMesh* pMesh, LXWorldPrimitive* WorldPrimitive) override;

	void				SetRenderer		( LXRenderer* pRenderer ) { _Renderer = pRenderer; }
	const SetActors&	GetNodes		( ) const { return _setActors; }

private:

	LXRenderer*			_Renderer = nullptr;
	uint				_nTestedBoxes;
	uint				_nTestedTriangles;
	SetActors			_setActors;
	
};

