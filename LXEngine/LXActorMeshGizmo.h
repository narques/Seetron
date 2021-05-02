//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXActorMesh.h"

class LXCORE_API LXActorMeshGizmo : public LXActorMesh
{

public:

	LXActorMeshGizmo();
	virtual ~LXActorMeshGizmo();
	
	// Overridden from LXActorMesh
	EConstraint GetConstraint(LXPrimitive* primitive) const override;
	
private:

	// Overridden from LXActor
	void Run(double frameTime) override;
	void OnSelectionChanged() override;
	void OnPositionChanged() override;
	void OnRotationChanged() override;
	
	void UpdateScale();

private:

	bool _UpdateActorTransform = true;
	std::map<LXPrimitive*, EConstraint> _primitiveConstraints;
	
};

