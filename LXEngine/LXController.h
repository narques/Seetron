//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include "LXVec4.h"
#include "LXMatrix.h"
#include "LXBBox.h"

class LXPrimitiveInstance;
class LXMaterial;
class LXMutex;
class LXRenderer;
class LXString;

class LXRendererUpdate
{
public:

	LXRendererUpdate() {}
	virtual ~LXRendererUpdate(){}
};

// As the MainThread can modify matrix and the RenderThread use it in Parallel.
// We need to store the updated matrix at the end of MainThread Tick.
// The RenderTherad will get the following object.

class LXRendererUpdateMatrix : public LXRendererUpdate
{
public:
	LXPrimitiveInstance* PrimitiveInstance;
	LXMatrix Matrix; // World Matrix
	LXBBox BBox;	 // World BBox
};

typedef list<LXRendererUpdate*> ListRendererUpdates;

class LXCORE_API LXController : public LXObject
{

public:

	LXController();
	virtual ~LXController();

	void SetRenderer(LXRenderer* Renderer);
	void Purge();

	//
	// Material
	//

	void AddMaterialToUpdateRenderStateSet(LXMaterial* Material);
	void AddMaterialToRebuild(LXMaterial* material);

	//
	// Actor
	//

	void AddActorToUpdateRenderStateSet(LXActor* Actor);
	void AddActorToDeleteSet(LXActor* Actor);
	void MaterialChanged(LXActor* Actor, LXMaterial* Material);

	void ActorWorldMatrixChanged(LXActor* Actor);

	//
	// PrimitiveInstance
	//
		
	SetActors& GetActorsToDeleteRT() { return _SetActorToDelete_RT; }
	SetActors& GetActorToUpdateRenderStateSetRT() { return _SetActorToUpdateRenderState_RT; }
	SetMaterials& GetMaterialToUpdateRenderStateSetRT() { return _SetMaterialToUpdateRenderState_RT; }
	SetMaterials& GetMaterialToRebuild_RT() { return _SetMaterialToRebuild_RT; }

	ListRendererUpdates& GetRendererUpdate() { return _RendererUpdates; }
	
	// Prepare the Data for RenderThread
	void Run();
	
	void AddRendererUpdateMatrix(LXActor* Actor);
	
private:

	LXRenderer*		_Renderer = nullptr;

	// Fed by MainTread or LoadingThread
	SetActors		_SetActorToUpdateRenderState;
	SetActors		_SetActorToDelete;
	SetActors		_SetActorToMove;
	SetMaterials	_SetMaterialToUpdateRenderState;
	SetMaterials	_SetMaterialToRebuild;
	
	// Consumed by RenderThread
	SetActors		_SetActorToUpdateRenderState_RT;
	SetActors		_SetActorToDelete_RT;
	SetMaterials	_SetMaterialToUpdateRenderState_RT;
	SetMaterials	_SetMaterialToRebuild_RT;

	// Shared
	ListRendererUpdates	_RendererUpdates;

	// Threads
	LXMutex* _mutex; // _SetActorToUpdateRenderState
};

