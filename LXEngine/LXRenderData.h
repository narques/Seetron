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

class LXMaterial;
class LXMesh;
class LXPrimitiveInstance;
class LXRenderCluster;

class LXWorldPrimitive
{

public:

	LXWorldPrimitive(const shared_ptr<LXPrimitiveInstance>& primitiveInstance);
	LXWorldPrimitive(const shared_ptr<LXPrimitiveInstance>& primitiveInstance, const LXMatrix& matrix, const LXBBox& box);
	~LXWorldPrimitive();

	void SetMaterial(LXMaterial* material);
	
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
	virtual ~LXRenderData();

	const LXActor* GetActor() const { return _actor; }

	// Common
	const TWorldPrimitives& GetPrimitives() { return _worldPrimitives; }
	void ComputePrimitiveWorldMatrices();
	void ComputePrimitiveWorldBounds();

	// RenderThread only
	int32 GetActorType() const { return _actorType; }
	bool GetCastShadows() const { return _castShadows; }
	bool ShowPrimitiveBBox() const { return _primitiveBBox; }
	bool ShowActorBBox() const { return _actorBBox; }
	const LXBBox& GetBBoxWorld()const { return _bboxWorld; }

public:

	// RenderThread only
	list<LXRenderCluster*> RenderClusters;

private:
	
	LXActor* _actor;
	LXBBox _bboxWorld;
	shared_ptr<LXMesh> _mesh;
	TWorldPrimitives _worldPrimitives;
	int32 _actorType;
	bool _castShadows;
	bool _primitiveBBox;
	bool _actorBBox;
};

