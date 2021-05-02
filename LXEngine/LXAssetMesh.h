//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once
#include "LXAsset.h"

class LXMesh;
class LXPrimitive;

typedef std::map<int, std::shared_ptr<LXPrimitive>> MapGeometries;

class LXAssetMesh : public LXAsset
{
	friend LXMesh; 
	friend LXPrimitive;

public:

	LXAssetMesh();
	LXAssetMesh(std::shared_ptr<LXMesh>& mesh);
	virtual ~LXAssetMesh();
	
	//
	// Overridden From LXResource
	//

	bool Load() override;
	LXString GetFileExtension() override { return LX_MESH_EXT; }

	std::shared_ptr<LXMesh>& GetMesh();

private:

	// Load & Save tools
	const MapGeometries& GetPrimitives() const { return _mapGeometries; }
	bool LoadGeometries(const LXFilepath& strFilename, MapGeometries& mapGeometries);
	LXFile* GetGeometryFile() { return nullptr; }

	//
	// Overridden From LXSmartObject
	//

	bool OnLoadChild(const TLoadContext& loadContext) override;
	bool OnSaveChild(const TSaveContext& saveContext) const override;
	void OnLoaded() override;
	
	// Misc
	void OnMeshesdTransformationChanged();
	void OnMeshesBoundingBoxInvalidated();
	void OnMeshVisibilityChanged();

private:

	std::shared_ptr<LXMesh> _Root;
	MapGeometries _mapGeometries; 

};