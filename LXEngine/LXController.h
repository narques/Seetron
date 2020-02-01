//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXBBox.h"
#include "LXMatrix.h"
#include "LXRenderClusterType.h"
#include "LXVec4.h"

class LXMutex;
class LXRenderer;
class LXString;
class LXWorldPrimitive;

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
	LXActor* Actor;
	LXWorldPrimitive* WorldPrimitive;
	LXMatrix Matrix; // World Matrix
	LXBBox BBox;	 // World BBox
};

typedef list<LXRendererUpdate*> ListRendererUpdates;
typedef pair<LXActor*, LXFlagsRenderClusterRole> ActorUpdate;
typedef set<ActorUpdate> SetActorToUpdate;

class LXCORE_API LXController : public LXObject
{

public:

	LXController();
	virtual ~LXController();

	void SetRenderer(LXRenderer* Renderer);
	void Purge();

	void ActorWorldMatrixChanged(LXActor* Actor);
	ListRendererUpdates& GetRendererUpdate() { return _RendererUpdates; }
	
	// Prepare the Data for RenderThread
	void Run();

private:
	
	void AddRendererUpdateMatrix(LXActor* Actor);
	
private:

	LXRenderer*		_Renderer = nullptr;

	// Fed by MainTread or LoadingThread
	SetActors		 _SetActorToMove;
	
	// Shared
	ListRendererUpdates	_RendererUpdates;
};

 