//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXComponent.h"

struct LXTerrainInfo
{
	// Number of MetaPatch side
	int _metaPatchPerTerrainSide;
	
	// Number of Patch per MetaParch side (PowerOfTwo)
	int _patchPerMetaPatchSide;
	
	// Number of Quad per Patch side (PowerOfTwo)
	int _quadPerPatchSide;

	// The quad dimension in engine base unit (cm)
	float _quadSize;

	// The base material for all the MetaPatches
	std::shared_ptr<LXMaterial> Material;

	// Returns the MetaPatch terrain side size
	float GetMetaPatchSize() const { return _patchPerMetaPatchSide * _quadPerPatchSide * _quadSize; }

	// Gives the ideal HeightMap texture size in pixel.
	int GetHeightMapTextureSize() const { return _patchPerMetaPatchSide * _quadPerPatchSide + 1; }
};

class LXComponentTerrainGenerator : public LXComponent
{

public:

	LXComponentTerrainGenerator(LXActor* actor);
	virtual ~LXComponentTerrainGenerator();

	void Build(const LXTerrainInfo& terrainInfo);

	void Built();

private:

	// Creates MetaPatch Assets: Actors, Components, Materials & Textures.
	void CreateMetaPatches(const LXTerrainInfo& terrainInfo);

	// Creates volatile resources, used during the generation.
	void CreateGenerationResources(const LXTerrainInfo& terrainInfo);

	// Generates the height maps. The volatile resources are released !
	void GenerateHeigtMap(const LXTerrainInfo& terrainInfo);

private:

	bool _building = false;

	// Kept the current terrain information. Useful for partial rebuild.
	LXTerrainInfo _terrainInfo = {};

	// MetaPatches  
	std::list<LXActor*> _actorMetaPatches;
//	std::list<LXComponentMetaPatch*> _componentMetaPatches;
};

