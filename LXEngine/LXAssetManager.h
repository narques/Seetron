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

struct LXAssetFileInfo
{
	LXFilepath FullFileName;
	EResourceOwner Owner;
	std::weak_ptr<LXAsset> Asset;
};

typedef std::map<LXString, LXAssetFileInfo> MapAssets;

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
	const std::shared_ptr<LXAsset> GetAsset(const LXString& Name) const;
	
	// Material
	const std::shared_ptr<LXMaterial> GetDefaultMaterial();
	const std::shared_ptr<LXMaterial> GetMaterial(const LXString& filename) const;

	// Shader
	const std::shared_ptr<LXShader> GetShader(const LXString& filename) const;

	// Texture
	const std::shared_ptr<LXTexture> GetDefaultTexture();
	const std::shared_ptr<LXTexture> GetNoiseTexture4x4();
	const std::shared_ptr<LXTexture> GetTexture(const LXString& filename) const;

	// AssetMesh	
	const std::shared_ptr<LXAssetMesh> GetAssetMesh(const LXString& filename) const;

	// Scripts
	const std::shared_ptr<LXScript> GetScript(const LXString& filename) const;

	//
	// Create resources
	//

	// New blank asset
	const std::shared_ptr<LXMaterial> CreateNewMaterial(const LXString& MaterialName, const LXString& RelativeAssetFolder);
	const std::shared_ptr<LXShader>	CreateNewShader(const LXString& MaterialName, const LXString& RelativeAssetFolder);
	const std::shared_ptr<LXTexture>	CreateNewTexture(const LXString& MaterialName, const LXString& RelativeAssetFolder);
	const std::shared_ptr<LXAnimation> CreateNewAnimation(const LXString& AnimationName, const LXString& RelativeAssetFolder);
	
	// Import from existing file
	const std::shared_ptr<LXAsset> Import(const LXFilepath& Filepath, const LXString& RelativeAssetFolder, bool Transcient = false);

	//
	// Misc
	//

	template<typename T>
	void				GetAssets(std::list<std::shared_ptr<T>>& listAssets)const;
	void				GetScripts(std::list<std::shared_ptr<LXScript>>& listScripts)const;
	void				GetTextures(std::list<std::shared_ptr<LXTexture>>& listTextures)const;
	void				GetMaterials(std::list<std::shared_ptr<LXMaterial>>& listMaterial)const;
	void				GetShaders(std::list<std::shared_ptr<LXShader>>& listShader)const;
	void				GetMeshes(std::list<std::shared_ptr<LXAssetMesh>>& listMeshes)const;
	void				GetAssets(std::list<std::shared_ptr<LXAsset>>& listAssets)const;
	void				GetAssetsOfType(std::list<std::shared_ptr<LXAsset>>& listAssets, EAssetType assetType)const;
	std::shared_ptr<LXAsset> FindAsset(const LXString& RelativeFilepath)const;
	const MapAssets&	GetAssets() const { return _MapAssets; }

	// Update the renamed asset in the std::map
	void				OnAssetRenamed(const LXString& Path, const LXString& OldName, const LXString& NewName, EResourceOwner ResourceOwner);

	const LXGraphTemplate* GetGraphMaterialTemplate() 
	{
		return _graphMaterialTemplate.get();
	}

private:

	template <typename T>
	const std::shared_ptr<T> GetResourceT(const LXString& Name) const;

	void LoadFromFolder(const LXFilepath& assetFolderpath, EResourceOwner resourceOwner);
	
	// Add a FileInfo asset to the library.
	void AddAsset(const LXFilepath& filepath, const LXFilepath& assetFolderpath, EResourceOwner resourceOwner);
	
	// Add a existing asset to the library.
	void AddAsset(std::shared_ptr<LXAsset> asset);

	// Create Asset from the given AssetFileInfo
	std::shared_ptr<LXAsset> CreateAsset(const LXAssetFileInfo& assetFileInfo) const;
	
	void UpdateAsset(const LXFilepath& filepath, const LXFilepath& assetFolderpath, EResourceOwner resourceOwner);
	void RemoveAsset(const LXFilepath& filepath, const LXFilepath& assetFolderpath, EResourceOwner resourceOwner);
	bool IsValidFile(const LXFilepath& filepath);
	LXString BuildKey(EResourceOwner ResourceOwner, const LXString& RelativeFilepath);
	
protected:

	LXProject*		_pDocument;
	mutable MapAssets _MapAssets;
	
private:

	std::shared_ptr<LXMaterial> _defaultMaterial;

	std::unique_ptr<LXGraphTemplate> _graphMaterialTemplate;

	std::unique_ptr<LXFileWatcher> _projectFileWatcher;

	std::list<LXString>	_ListAssetExtentions;
};


