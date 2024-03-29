//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXActorMesh.h"

class LXENGINE_API LXActorMeshPlane : public LXActorMesh
{

public:

	LXActorMeshPlane(LXProject*);
	virtual ~LXActorMeshPlane();

private:

	virtual void OnPropertyChanged(LXProperty* pProperty) override;

private:

	std::shared_ptr<LXMaterial> _Material;

};

