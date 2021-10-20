//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXActorMesh.h"
#include "LXCore.h"

#if LX_ANCHOR

class LXENGINE_API LXAnchor : public LXActorMesh
{

public:

	LXAnchor(LXProject* pDocument);
	virtual ~LXAnchor(void);

	// Overridden from LXActor
	virtual bool	ParticipateToSceneBBox() const override{ return false; }
	virtual bool	IsVisible() const override { return _bVisible && !GetCore().GetPlayMode(); }

	int Id() const { return _id; }
	void Id(int val) { _id = val; }
	void SetOwner(LXActor* owner);
	LXActor* GetOwner() const { return _owner; }

private:

	int	_id = -1;
	LXMaterial* _material = 0;
	LXActor* _owner = 0;

};

#endif