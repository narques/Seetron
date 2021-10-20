//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXTraverserFrustumCulling.h"

class LXRenderer;

class LXENGINE_API LXRectangularSelectionTraverser : public LXTraverserFrustumCulling
{

public:

	LXRectangularSelectionTraverser(void);
	virtual ~LXRectangularSelectionTraverser(void);

	// Overridden from LXTraverser
	virtual void Apply() override;
	virtual void OnActor(LXActor* actor)override;
	virtual void OnPrimitive(LXActorMesh* actorMesh, LXComponentMesh* componentMesh, LXWorldPrimitive* worldPrimitive) override;

	void SetRenderer(LXRenderer* renderer) { _renderer = renderer; }
	const SetActors& GetNodes() const { return _setActors; }

private:

	LXRenderer*	_renderer = nullptr;
	uint _testedBoxes;
	uint _testedTriangles;
	SetActors _setActors;
	
};

