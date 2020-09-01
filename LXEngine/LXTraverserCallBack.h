//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXTraverser.h"

class LXTraverserLambda : public LXTraverser
{

public:

	LXTraverserLambda(LXScene* pScene);
	virtual void OnActor(LXActor* pGroup) override;
	virtual void OnMesh(LXActorMesh* pMesh) override;
	virtual void OnPrimitive(LXActorMesh* pMesh, LXComponentMesh* componentMesh, LXWorldPrimitive* WorldPrimitive) override;
	
	void SetLambdaOnGroup(std::function<void(LXActor*)> eval) { _lambdaOnGroup = eval; }
	void SetLambdaOnMesh(std::function<void(LXActorMesh*)> eval) { _lambdaOnMesh = eval; }
	void SetLamndaOnPrimitive(std::function<void(LXActorMesh*, LXWorldPrimitive*)> eval) { _lambdaOnPrimitive = eval; }
	
private:

	std::function<void(LXActor*)>		_lambdaOnGroup;
	std::function<void(LXActorMesh*)>	_lambdaOnMesh;
	std::function<void(LXActorMesh*, LXWorldPrimitive*)> _lambdaOnPrimitive;
};