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
		
	SetActors& GetActorsToDeleteRT() { return _SetActorToDeleteRT; }
	SetActors& GetActorToUpdateRenderStateSetRT() { return _SetActorToUpdateRenderStateRT; }
	SetMaterials& GetMaterialToUpdateRenderStateSetRT() { return _SetMaterialToUpdateRenderStateRT; }

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
	
	// Consumed by RenderThread
	SetActors		_SetActorToUpdateRenderStateRT;
	SetActors		_SetActorToDeleteRT;
	SetMaterials	_SetMaterialToUpdateRenderStateRT;

	// Shared
	ListRendererUpdates	_RendererUpdates;
};

