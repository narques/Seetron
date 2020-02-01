//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXController.h"
#include "LXActorMesh.h"
#include "LXRenderer.h"
#include "LXMemory.h" // --- Must be the last included ---

LXController::LXController()
{
}

LXController::~LXController()
{
	Purge();
}

void LXController::SetRenderer(LXRenderer* Renderer)
{
	_Renderer = Renderer; 
}

void LXController::Purge()
{
	_SetActorToMove.clear();
	for (LXRendererUpdate* RendererUpdate : _RendererUpdates)
	{
		delete RendererUpdate;
	}
	_RendererUpdates.clear();
}

void LXController::ActorWorldMatrixChanged(LXActor* Actor)
{
	if (!Actor->IsVisible())
		return;
	
	//LogD(LXController, L"ActorWorldMatrixChanged %s", Actor->GetName().GetBuffer());
	
	// All thread can Transform an Actor
	DWORD hThread = ::GetCurrentThreadId();

	if (IsMainThread())
	{
		// Main Thread, Use the Sync point
		_SetActorToMove.insert(Actor);
	}
	else if (IsRenderThread())
	{
		// RT, Create the UpudateState ans Store in fot the next RT Frame. As we do not know when our this event happens
		//LXRendererUpdateMatrix For a NextFrame !
		AddRendererUpdateMatrix(Actor);
	}
	else if (IsLoadingThread())
	{
		// OR Nothing do no. Wait unil actor loading is completed ( TODO OnActorLoaded Event ? )
		_SetActorToMove.insert(Actor);
	}
	else
	{
		CHK(0);
	}
}

void LXController::AddRendererUpdateMatrix(LXActor* Actor)
{
	if (LXActorMesh* ActorMesh = dynamic_cast<LXActorMesh*>(Actor))
	{
		const TWorldPrimitives& WorldPrimitives = ActorMesh->GetAllPrimitives();
		for (const LXWorldPrimitive* WorldPrimitive : WorldPrimitives)
		{
			LXRendererUpdateMatrix* RendererUpdateMatrix = new LXRendererUpdateMatrix();
			RendererUpdateMatrix->Actor = Actor;
			RendererUpdateMatrix->WorldPrimitive = const_cast<LXWorldPrimitive*>(WorldPrimitive);
			RendererUpdateMatrix->Matrix = WorldPrimitive->MatrixWorld;
			RendererUpdateMatrix->BBox = WorldPrimitive->BBoxWorld;
			_RendererUpdates.push_back(RendererUpdateMatrix);
		}
	}
}

void LXController::Run()
{
	if (!RenderThread && LXRenderer::gUseRenderThread)
		return;

	// Now the RenderTread is waiting
	// TODO CHK(RenderThread.IsWaiting());
	
	// All updates should be consumed.
	//CHK(RendererUpdates.size() == 0);
	
	// Actors to move
	if (_SetActorToMove.size())
	{
		for (LXActor* Actor : _SetActorToMove)
		{
			AddRendererUpdateMatrix(Actor);
		}
		_SetActorToMove.clear();
	}
}
