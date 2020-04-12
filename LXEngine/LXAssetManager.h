//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXSmartObject.h"
#include "LXAsset.h"

class LXAnimation;
class LXAssetMesh;
class LXFileWatcher;
class LXGraphTemplate;
class LXMaterial;
class LXProject;
class LXScript;
class LXShader;
class LXTexture;

typedef map<LXString, shared_ptr<LXAsset>> MapAssets;

#define LX_DEFAULT_MESH_FOLDER		L"Meshes/"
#define LX_DEFAULT_MATERIAL_FOLDER	L"Materials/"
#define LX_DEFAULT_SHADER_FOLDER	L"Shaders/"
#define LX_DEFAULT_TEXTURE_FOLDER	L"Textures/"
#define LX_DEFAULT_ANIMATION_FOLDER L"Animations/"

class LXCORE_API LXAssetManager : public LXSmartObject
{

public:

	LXAssetManager(LXProject* pDocument);
	virtual ~LXAssetManager();
	void Init();

	// Asset
	const shared_ptr<LXAsset>&	GetAsset(const LXString& Name) const;
	
	// Material
	const shared_ptr<LXMaterial> GetDefaultMaterial();
	const shared_ptr<LXMaterial> GetMaterial(const LXString& filename) const;

	// Shader
	const shared_ptr<LXShader> GetShader(const LXString& filename) const;

	// Texture
	const shared_ptr<LXTexture> GetDefaultTexture();
	const shared_ptr<LXTexture> GetNoiseTexture4x4();
	const shared_ptr<LXTexture> GetTexture(const LXString& filename) const;

	// AssetMesh	
	const shared_ptr<LXAssetMesh> GetAssetMesh(const LXString& filename) const;

	// Scripts
	const shared_ptr<LXScript> GetScript(const LXString& filename) const;

	//
	// Create resources
	//

	// New blank asset
	const shared_ptr<LXMaterial> CreateNewMaterial(const LXString& MaterialName, const LXString& RelativeAssetFolder);
	const shared_ptr<LXShader>	CreateNewShader(const LXString& MaterialName, const LXString& RelativeAssetFolder);
	const shared_ptr<LXTexture>	CreateNewTexture(const LXString& MaterialName, const LXString& RelativeAssetFolder);
	const shared_ptr<LXAnimation> CreateNewAnimation(const LXString& AnimationName, const LXString& RelativeAssetFolder);
	
	// Import from existing file
	const shared_ptr<LXAsset> Import(const LXFilepath& Filepath, const LXString& RelativeAssetFolder, bool Transcient = false);

	//
	// Misc
	//

	template<typename T>
	void				GetAssets(list<shared_ptr<T>>& listAssets)const;
	void				GetScripts(list<shared_ptr<LXScript>>& listScripts)const;
	void				GetTextures(list<shared_ptr<LXTexture>>& listTextures)const;
	void				GetMaterials(list<shared_ptr<LXMaterial>>& listMaterial)const;
	void				GetShaders(list<shared_ptr<LXShader>>& listShader)const;
	void				GetMeshes(list<shared_ptr<LXAssetMesh>>& listMeshes)const;
	void				GetAssets(list<shared_ptr<LXAsset>>& listAssets)const;
	void				GetAssetsOfType(list<shared_ptr<LXAsset>>& listAssets, EAssetType assetType)const;
	shared_ptr<LXAsset>& FindAsset(const LXString& RelativeFilepath)const;
	const MapAssets&	GetAssets() const { return _MapAssets; }

	// Update the renamed asset in the map
	void				OnAssetRenamed(const LXString& Path, const LXString& OldName, const LXString& NewName, EResourceOwner ResourceOwner);

	const LXGraphTemplate* GetGraphMaterialTemplate() 
	{
		return _graphMaterialTemplate.get();
	}

private:

	template <typename T>
	const shared_ptr<T> GetResourceT(const LXString& Name) const;

	void LoadFromFolder(const LXFilepath& assetFolderpath, EResourceOwner resourceOwner);
	void AddAsset(const LXFilepath& filepath, const LXFilepath& assetFolderpath, EResourceOwner resourceOwner);
	void UpdateAsset(const LXFilepath& filepath, const LXFilepath& assetFolderpath, EResourceOwner resourceOwner);
	void RemoveAsset(const LXFilepath& filepath, const LXFilepath& assetFolderpath, EResourceOwner resourceOwner);
	bool IsValidFile(const LXFilepath& filepath);
	LXString BuildKey(EResourceOwner ResourceOwner, const LXString& RelativeFilepath);
	
protected:

	LXProject*		_pDocument;
	MapAssets		_MapAssets;
	
private:

	unique_ptr<LXGraphTemplate> _graphMaterialTemplate;

	unique_ptr<LXFileWatcher> _projectFileWatcher;

	list<LXString>	_ListAssetExtentions;
};


