//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXController.h"
#include "LXThread.h"
#include "LXMaterial.h"
#include "LXActor.h"
#include "LXMaterialD3D11.h"
#include "LXCore.h"
#include "LXRenderer.h"
#include "LXActorMesh.h"
#include "LXProperty.h"
#include "LXActor.h"
#include "LXMemory.h" // --- Must be the last included ---

LXController::LXController()
{

	// Listen the properties

	LXSmartObject::RegisterCB_OnPropertiesChanged(this, [this](LXSmartObject* SmartObject, LXProperty* Property)
	{
		if (LXMaterial* Material = dynamic_cast<LXMaterial*>(SmartObject))
		{
			AddMaterialToUpdateRenderStateSet(Material);
		}
		else if (LXActor* Actor = dynamic_cast<LXActor*>(SmartObject))
		{
			if (LXPropertyAssetPtr* PropertyAsset = dynamic_cast<LXPropertyAssetPtr*>(Property))
			{
				if (LXMaterial* Marterial = dynamic_cast<LXMaterial*>(PropertyAsset->GetValue()))
				{
					// this will call AddActorToUpdateRenderStateSet
					Actor->InvalidateRenderState();
				}
			}
		}
	});
}

LXController::~LXController()
{
	LXSmartObject::UnregisterCB_OnPropertiesChanged(this);
	Purge();
}

void LXController::SetRenderer(LXRenderer* Renderer)
{
	_Renderer = Renderer; 
}

void LXController::Purge()
{
	_SetActorToUpdateRenderState.clear();
	_SetActorToDelete.clear();
	_SetActorToMove.clear();
	_SetMaterialToUpdateRenderState.clear();

	_SetActorToUpdateRenderStateRT.clear();
	_SetActorToDeleteRT.clear();
	_SetMaterialToUpdateRenderStateRT.clear();

	for (LXRendererUpdate* RendererUpdate : _RendererUpdates)
	{
		delete RendererUpdate;
	}

	_RendererUpdates.clear();
}

LXMaterialD3D11* GetMaterialD3D11(const LXMaterial* Material)
{
	CHK(IsRenderThread());
	return GetRenderer()->GetMaterialD3D11(Material);
}

void LXController::AddActorToUpdateRenderStateSet(LXActor* Actor)
{
	CHK(IsMainThread() || IsLoadingThread());
	LXMutex Mutex;
	Mutex.Lock();
	LogD(LXController, L"AddActorToUpdateRenderStateSet %s", Actor->GetName().GetBuffer());
	_SetActorToUpdateRenderState.insert(Actor);
	Mutex.Unlock();
}

void LXController::AddActorToDeleteSet(LXActor* Actor)
{
	CHK(IsMainThread());
	_SetActorToDelete.insert(Actor);
}

void LXController::AddMaterialToUpdateRenderStateSet(LXMaterial* Material)
{
	CHK(IsMainThread());
	_SetMaterialToUpdateRenderState.insert(Material);
}

void LXController::MaterialChanged(LXActor* ActorMesh, LXMaterial* Material)
{
	CHK(IsMainThread());
	AddActorToUpdateRenderStateSet(ActorMesh);
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
		for (const LXWorldPrimitive& WorldPrimitive : WorldPrimitives)
		{
			LXRendererUpdateMatrix* RendererUpdateMatrix = new LXRendererUpdateMatrix();
			RendererUpdateMatrix->PrimitiveInstance = WorldPrimitive.PrimitiveInstance;
			RendererUpdateMatrix->Matrix = WorldPrimitive.MatrixWorld;
			RendererUpdateMatrix->BBox = WorldPrimitive.BBoxWorld;
			_RendererUpdates.push_back(RendererUpdateMatrix);
		}
	}
}

void LXController::Run()
{
	if (!RenderThread && LXRenderer::gUseRenderThread)
		return;

	// Now the RenderTread must wait
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

	//
	// Actors
	//

	{
		LXMutex Mutex; 
		Mutex.Lock();
		SetActors& Actors = _SetActorToUpdateRenderState;
		// Previous actors should be consumed
		CHK(_SetActorToUpdateRenderStateRT.size() == 0);
		_SetActorToUpdateRenderStateRT.insert(Actors.begin(), Actors.end());

		for (const LXActor* Actor : _SetActorToUpdateRenderStateRT)
		{
			LogD(LXController, L"Synchronised: %s", Actor->GetName().GetBuffer());
		}
		
		Actors.clear();
		Mutex.Unlock();
	}

	//
	// Material
	//
	
	{
		LXMutex Mutex;
		Mutex.Lock();
		SetMaterials& Materials = _SetMaterialToUpdateRenderState;
		// Previous materials should be consumed
		CHK(_SetMaterialToUpdateRenderStateRT.size() == 0);
		_SetMaterialToUpdateRenderStateRT.insert(Materials.begin(), Materials.end());
		Materials.clear();
		Mutex.Unlock();
	}

	//
	// Destroy Actors marked for delete
	//
	
	{
		SetActors& Actors = _SetActorToDelete;
		for (SetActors::iterator It = Actors.begin(); It != Actors.end();)
		{
			if ((*It)->IsRenderStateValid())
			{
				delete *It;
				It = Actors.erase(It);
			}
			else
				It++;
		}
	}
}
