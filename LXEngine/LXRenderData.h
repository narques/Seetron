//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------
// Data exchange between MainThread and RenderThread
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include "LXBBox.h"

class LXComponent;
class LXMaterialBase;
class LXMesh;
class LXPrimitiveInstance;
class LXRenderCluster;

class LXWorldPrimitive
{

public:

	LXWorldPrimitive(const shared_ptr<LXPrimitiveInstance>& primitiveInstance);
	LXWorldPrimitive(const shared_ptr<LXPrimitiveInstance>& primitiveInstance, const LXMatrix& matrix, const LXBBox& box);
	~LXWorldPrimitive();

	void SetMaterial(const LXMaterialBase* material);
	
	shared_ptr<LXPrimitiveInstance> PrimitiveInstance;
	LXMatrix MatrixWorld;
	LXBBox BBoxWorld;

	// Rendering
	LXRenderCluster* RenderCluster = nullptr;
};

typedef vector<LXWorldPrimitive*> TWorldPrimitives;

class LXRenderData : public LXObject
{

public:

	LXRenderData(LXActor* actor);
	LXRenderData(LXActor* actor, LXComponent* component);
	virtual ~LXRenderData();

	const LXActor* GetActor() const { return _actor; }

	// Common
	const TWorldPrimitives& GetPrimitives() { return _worldPrimitives; }
	bool IsValid() const { return _valid; }
	void Validate() { _valid = true; }
	void Invalidate() { _valid = false; }

	bool AreValideWorldMatrices() const { return _validPrimitiveWorldMatrices; }
	void InvalidateWorldMatrices() { _validPrimitiveWorldMatrices = false; }
	
	bool AreValideWorldBounds() const { return _validPrimitiveWorldBounds; }
	void InvalidateWorldBounds() { _validPrimitiveWorldBounds = false;  }

	void ComputePrimitiveWorldMatrices();
	void ComputePrimitiveWorldBounds();

	void ShowPrimitiveBBox(bool show) { _primitiveBBox = show; }
	bool ShowPrimitiveBBox() const { return _primitiveBBox; }

	// RenderThread only
	int32 GetActorType() const { return _actorType; }
	bool GetCastShadows() const { return _castShadows; }
	
	bool ShowActorBBox() const { return _actorBBox; }
	const LXBBox& GetBBoxWorld()const { return _bboxWorld; }

	// Delegates & Events
	// LXDelegate<> PrimitiveWorldBoundsComputed; // NOWAY :( RenderData is volatile...

public:

	// RenderThread only
	list<LXRenderCluster*> RenderClusters;

private:

	// Common
	atomic<bool> _primitiveBBox = false;

	bool _valid = false;
	
	LXActor* _actor = nullptr;
	LXComponent* _component = nullptr;
	LXBBox _bboxWorld;
	shared_ptr<LXMesh> _mesh;
	
	TWorldPrimitives _worldPrimitives;
	bool _validPrimitiveWorldMatrices = false;
	bool _validPrimitiveWorldBounds = false;

	int32 _actorType;
	bool _castShadows;
	bool _actorBBox;
};

