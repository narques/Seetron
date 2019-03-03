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
class LXGraphTemplate;
class LXGraphTexture;
class LXMaterial;
class LXProject;
class LXScript;
class LXShader;
class LXTexture;

typedef map<LXString, LXAsset*> MapAssets;

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

	// Asset
	LXAsset*			GetAsset(const LXString& Name) const;
	
	// Material
	LXMaterial*			GetDefaultMaterial();
	LXMaterial*			GetMaterial(const LXString& filename) const;

	// Shader
	LXShader*			GetShader(const LXString& filename) const;

	// Texture
	LXTexture*			GetTexture(const LXString& filename) const;

	// AssetMesh	
	LXAssetMesh*		GetAssetMesh(const LXString& filename) const;

	// Scripts
	LXScript*			GetScript(const LXString& filename) const;

	// Scripts
	LXGraphTexture*		GetGraphTexture(const LXString& filename) const;

	//
	// Create resources
	//

	// New blank asset
	LXMaterial*			CreateNewMaterial(const LXString& MaterialName, const LXString& RelativeAssetFolder);
	LXShader*			CreateNewShader(const LXString& MaterialName, const LXString& RelativeAssetFolder);
	LXTexture*			CreateNewTexture(const LXString& MaterialName, const LXString& RelativeAssetFolder);
	LXAnimation*		CreateNewAnimation(const LXString& AnimationName, const LXString& RelativeAssetFolder);
	
	// Import from existing file
	LXAsset*			Import(const LXFilepath& Filepath, const LXString& RelativeAssetFolder, bool Transcient = false);

	//
	// Misc
	//

	template<typename T>
	void				GetAssets(list<T>& listAssets)const;
	void				GetScripts(list<LXScript*>& listScripts)const;
	void				GetTextures(list<LXTexture*>& listTextures)const;
	void				GetMaterials(list<LXMaterial*>& listMaterial)const;
	void				GetShaders(list<LXShader*>& listShader)const;
	void				GetMeshes(list<LXAssetMesh*>& listMeshes)const;
	void				GetAssets(list<LXAsset*>& listAssets)const;
	void				GetAssetsOfType(list<LXAsset*>& listAssets, EAssetType assetType)const;
	LXAsset*			FindAsset(const LXString& RelativeFilepath)const;
	const MapAssets&	GetAssets() const { return _MapAssets; }

	// Update the renamed asset in the map
	void				OnAssetRenamed(const LXString& Path, const LXString& OldName, const LXString& NewName, EResourceOwner ResourceOwner);

	const LXGraphTemplate* GetGraphMaterialTemplate() 
	{
		return _graphMaterialTemplate.get();
	}

private:

	template <typename T>
	T GetResourceT(const LXString& Name) const;

	void LoadFromFolder(const LXFilepath& Folderpath, EResourceOwner ResourceOwner);
	void Add(LXAsset* Resource);
	LXMaterial* CreateEngineMaterial(const wchar_t* szName);
	LXString BuildKey(EResourceOwner ResourceOwner, const LXString& RelativeFilepath);
	
protected:

	LXProject*		_pDocument;
	MapAssets		_MapAssets;
	
private:

	unique_ptr<LXGraphTemplate> _graphMaterialTemplate;
	list<LXString>	_ListAssetExtentions;

};


