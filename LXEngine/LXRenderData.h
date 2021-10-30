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

#include "LXCore/LXObject.h"
#include "LXBBox.h"

class LXComponent;
class LXMaterialBase;
class LXMeshBase;
class LXPrimitiveInstance;
class LXRenderCluster;

class LXWorldPrimitive
{

public:

	LXWorldPrimitive(const std::shared_ptr<LXPrimitiveInstance>& primitiveInstance);
	LXWorldPrimitive(const std::shared_ptr<LXPrimitiveInstance>& primitiveInstance, const LXMatrix& matrix, const LXBBox& box);
	~LXWorldPrimitive();

	void SetMaterial(const LXMaterialBase* material);
	
	std::shared_ptr<LXPrimitiveInstance> PrimitiveInstance;
	LXMatrix MatrixWorld;
	LXBBox BBoxWorld;

	// Rendering
	LXRenderCluster* RenderCluster = nullptr;
};

typedef std::vector<LXWorldPrimitive*> TWorldPrimitives;

struct LXLODData
{
	float MaxDistance;
};

class LXRenderData : public LXObject
{

public:

	LXRenderData(LXActor* actor);
	LXRenderData(LXActor* actor, LXComponent* component);
	virtual ~LXRenderData();

	const LXActor* GetActor() const { return _actor; }

	// Common
	const TWorldPrimitives& GetPrimitives(int LODIndex = 0) { return _worldPrimitives[LODIndex]; }
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

	const float* GetLODDistances() const { return &_LODMaxDistance[0]; };

	// Delegates & Events
	// LXDelegate<> PrimitiveWorldBoundsComputed; // NOWAY :( RenderData is volatile...

public:

	// RenderThread only
	std::list<LXRenderCluster*> RenderClusters;

private:

	// Common
	std::atomic<bool> _primitiveBBox = false;

	bool _valid = false;
	
	LXActor* _actor = nullptr;
	LXComponent* _component = nullptr;
	LXBBox _bboxWorld;
	std::shared_ptr<LXMeshBase> _mesh;
	
	float _LODMaxDistance[LX_MAX_LODS];
	TWorldPrimitives _worldPrimitives[LX_MAX_LODS];
	
	bool _validPrimitiveWorldMatrices = false;
	bool _validPrimitiveWorldBounds = false;

	int32 _actorType;
	bool _castShadows;
	bool _actorBBox;
};

