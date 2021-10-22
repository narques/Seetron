//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXComponentMesh.h"
#include "LXDelegate.h"

class LXActorRenderToTexture;
class LXMaterialBase;
struct LXTerrainInfo;
class LXTexture;

#define LX_CMP_USEGENRESOURCES 1

class LXComponentMetaPatch : public LXComponentMesh
{

public:

	LXComponentMetaPatch(LXActor* actor);
	~LXComponentMetaPatch();
	
	// Creates the HeightMap & Material.
	void CreateMainResources(const LXTerrainInfo& terrainInfo, const vec2f& metaPatchPosition);

#if LX_CMP_USEGENRESOURCES
	// Creates the Generation Material & Actor resources.
	void CreateBuildResources(const LXTerrainInfo& terrainInfo, const vec2f& offset);

	// Starts the HeightMap Creation Scenario
	void GenerateHeigtMap(const LXTerrainInfo& terrainInfo);
#endif

	// Cleans resources at the end of the HeightMap Creation, 
	void OnCompiled();

	vec2f GetMinMaxHeight(const LXWorldPrimitive* worldPrimitive, const LXTexture* texture, float heightScale);
	float GetHeightAt(float x, float y) const;
	float GetHeightAt(float x, float y, const LXTexture* texture, float heightScale) const;

	// Delegates & Events
	LXDelegate<> Compiled;

private:

	virtual void ComputeBBoxLocal() override;

public:

	// Main resources
	std::shared_ptr<LXMaterialBase> Material;
	std::shared_ptr<LXTexture> HeightMap;

#if LX_CMP_USEGENRESOURCES
	// Generation resources
	std::shared_ptr<LXMaterial> _materialNoise;
	std::shared_ptr<LXMaterial> _materialErode;
	std::shared_ptr<LXMaterial> _materialFinalize;

	LXActorRenderToTexture* _renderToTextureNoise = nullptr;
	LXActorRenderToTexture* _renderToTextureErode = nullptr;
	LXActorRenderToTexture* _renderToTextureFinalize = nullptr;
#endif

	// States
	//bool _doneHeightMap = false;

	// 'Script' variables
	bool _compiling = false;
	int _erodeFrameCount = 400;

	// The MetaPatch position, in local coordinates (Same as the actor position)
	vec2f LocalPosition;
};

