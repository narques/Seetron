//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXAssetManager.h"
#include "LXProject.h"
#include "LXSettings.h"
#include "LXScript.h"
#include "LXLogger.h"
#include "LXMaterial.h"
#include "LXShader.h"
#include "LXTexture.h"
#include "LXProceduralTexture.h"
#include "LXDirectory.h"
#include "LXImporter.h"
#include "LXAssetMesh.h"
#include "LXMesh.h"
#include "LXConsoleManager.h"
#include "LXMemory.h" // --- Must be the last included ---

#define LX_DEFAULT_MATERIAL L"Materials/M_Default.smat"

//------------------------------------------------------------------------------------------------------
// Console commands
//------------------------------------------------------------------------------------------------------

LXConsoleCommand1S CCCreateNewMaterial(L"CreateNewMaterial", [](const LXString& inMaterialName)
{
	LXProject* Project = GetCore().GetProject();
	if (!Project)
	{
		LogW(Core, L"No project");
	}
	else
	{
		Project->GetAssetManager().CreateNewMaterial(inMaterialName, LX_DEFAULT_MATERIAL_FOLDER);
	}
});

LXConsoleCommand1S CCCreateNewTexture(L"CreateNewTexture", [](const LXString& inTextureName)
{
	LXProject* Project = GetCore().GetProject();
	if (!Project)
	{
		LogW(Core, L"No project");
	}
	else
	{
		Project->GetAssetManager().CreateNewTexture(inTextureName, LX_DEFAULT_TEXTURE_FOLDER);
	}
});

LXConsoleCommand1S CCCreateNewShader(L"CreateNewShader", [](const LXString& inShaderName)
{
	LXProject* Project = GetCore().GetProject();
	if (!Project)
	{
		LogW(Core, L"No project");
	}
	else
	{
		Project->GetAssetManager().CreateNewShader(inShaderName, LX_DEFAULT_SHADER_FOLDER);
	}
});

//------------------------------------------------------------------------------------------------------

const bool ForceLowercase = false;

LXAssetManager::LXAssetManager(LXProject* Project) :_pDocument(Project)
{
	_ListAssetExtentions.push_back(LX_MATERIAL_EXT);
	_ListAssetExtentions.push_back(LX_TEXTURE_EXT);
	_ListAssetExtentions.push_back(LX_SHADER_EXT);
	// Engine Asset
	LoadFromFolder(GetSettings().GetDataFolder(), EResourceOwner::LXResourceOwner_Engine);
	// Project Asset
	LoadFromFolder(Project->GetAssetFolder(), EResourceOwner::LXResourceOwner_Project);
	// Internal Engine Asset 
	BuildEngineMaterials();
}

LXAssetManager::~LXAssetManager()
{
	for (auto It : _MapAssets)
		delete It.second;

	LX_SAFE_DELETE(_defaultMaterial);
}

LXMaterial*	LXAssetManager::GetDefaultMaterial()
{
	// Default Material is not more dynamic. Now it's an asset

	LXMaterial* Material = GetMaterial(LX_DEFAULT_MATERIAL);
	CHK(Material);
	return Material;

//	if (!m_defaultMaterial)
//	{
//
// 		m_defaultMaterial = new LXMaterial;
// 		m_defaultMaterial->SetName(L"DefaulMaterial");
// 		m_defaultMaterial->SetFilepath(GetSettings().GetMaterialsFolder() + L"M_Default." + LX_MATERIAL_EXT);
// 		m_defaultMaterial->SetTwoSided(true);
// 		m_defaultMaterial->SetColor(L"Textures/grid.stex", 0);
// 		m_defaultMaterial->State = LXAsset::EResourceState::LXResourceState_Loaded;
// 		m_defaultMaterial->Save();
//	}

//	return m_defaultMaterial;
}

LXAsset* LXAssetManager::GetAsset(const LXString& Name) const
{
	LXString key = Name;
	key.MakeLower();

	auto It = _MapAssets.find(key);
	if (It != _MapAssets.end())
	{
		LXAsset* Resource = It->second;
		if (Resource->State == LXAsset::EResourceState::LXResourceState_Unloaded)
		{
			Resource->Load();
			LogI(AssetManager, L"Loaded %s", Resource->GetFilepath().GetBuffer());
		}

		if (Resource->State == LXAsset::EResourceState::LXResourceState_Loaded)
		{
			return Resource;
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		LogE(AssetManager, L"Resource %s not exists in ResourceManager", Name.GetBuffer());
		return nullptr;
	}
}

template <typename T>
T LXAssetManager::GetResourceT(const LXString& Name) const
{
	return dynamic_cast<T>(GetAsset(Name));
}

LXMaterial* LXAssetManager::GetMaterial(const LXString& strFilename) const
{
	return GetResourceT<LXMaterial*>(strFilename);
}

LXShader* LXAssetManager::GetShader(const LXString& strFilename) const
{
	return GetResourceT<LXShader*>(strFilename);
}

LXTexture* LXAssetManager::GetTexture(const LXString& strFilename) const
{
	return GetResourceT<LXTexture*>(strFilename);
}

LXAssetMesh* LXAssetManager::GetAssetMesh(const LXString& strFilename) const
{
	return GetResourceT<LXAssetMesh*>(strFilename);
}

LXScript* LXAssetManager::GetScript(const LXString& strFilename) const
{
	return GetResourceT<LXScript*>(strFilename);
}

LXGraphTexture* LXAssetManager::GetGraphTexture(const LXString& strFilename) const
{
	return GetResourceT<LXGraphTexture*>(strFilename);
}

LXMaterial* LXAssetManager::CreateNewMaterial(const LXString& MaterialName, const LXString& RelativeAssetFolder)
{
	LXFilepath AssetFolderpath = GetCore().GetProject()->GetAssetFolder();
	LXFilepath MaterialFilepath;
	
	if (MaterialName.IsEmpty())
	{
		int i = 0;
		while (1)
		{
			LXString GeneratedMaterialName = L"M_Material" + LXString::Number(i);
			MaterialFilepath = AssetFolderpath + RelativeAssetFolder + GeneratedMaterialName + L"." + LX_MATERIAL_EXT;
			if (!MaterialFilepath.IsFileExist())
				break;
			i++;
		}
	}
	else
	{
		MaterialFilepath = AssetFolderpath + RelativeAssetFolder + MaterialName + L"." + LX_MATERIAL_EXT;
		if (MaterialFilepath.IsFileExist())
		{
			LogW(ResourceManager, L"Material %s already exist (%S).", MaterialName.GetBuffer(), MaterialFilepath.GetBuffer());
			return GetMaterial(RelativeAssetFolder + MaterialName + L"." + LX_MATERIAL_EXT);
		}
	}
	

	LXMaterial* Material = new LXMaterial();
	Material->Owner = EResourceOwner::LXResourceOwner_Project;
	Material->SetFilepath(MaterialFilepath);
	Material->State = LXAsset::EResourceState::LXResourceState_Loaded;
	Material->Save();
	
	LXFilepath RelativeAssetFilepath = AssetFolderpath.GetRelativeFilepath(MaterialFilepath);
	RelativeAssetFilepath.MakeLower();
	_MapAssets[RelativeAssetFilepath] = Material;

	LogI(ResourceManager, L"Material %s created (%s).", MaterialName.GetBuffer(), MaterialFilepath.GetBuffer());
	return Material;
}

LXShader* LXAssetManager::CreateNewShader(const LXString& ShaderName, const LXString& RelativeAssetFolder)
{
	LXFilepath AssetFolderpath = GetCore().GetProject()->GetAssetFolder();
	LXFilepath ShaderFilepath;

	if (ShaderName.IsEmpty())
	{
		int i = 0;
		while (1)
		{
			LXString GeneratedShaderName = L"M_Shader" + LXString::Number(i);
			ShaderFilepath = AssetFolderpath + RelativeAssetFolder + GeneratedShaderName + L"." + LX_SHADER_EXT;
			if (!ShaderFilepath.IsFileExist())
				break;
			i++;
		}
	}
	else
	{
		ShaderFilepath = AssetFolderpath + RelativeAssetFolder + ShaderName + L"." + LX_SHADER_EXT;
		if (ShaderFilepath.IsFileExist())
		{
			LogW(ResourceManager, L"Shader %s already exist (%S).", ShaderName.GetBuffer(), ShaderFilepath.GetBuffer());
			return GetShader(RelativeAssetFolder + ShaderName + L"." + LX_SHADER_EXT);
		}
	}


	LXShader* Shader = new LXShader();
	Shader->Owner = EResourceOwner::LXResourceOwner_Project;
	Shader->SetFilepath(ShaderFilepath);
	Shader->State = LXAsset::EResourceState::LXResourceState_Loaded;
	Shader->SaveDefault();

	LXFilepath RelativeAssetFilepath = AssetFolderpath.GetRelativeFilepath(ShaderFilepath);
	RelativeAssetFilepath.MakeLower();
	_MapAssets[RelativeAssetFilepath] = Shader;

	LogI(ResourceManager, L"Shader %s created (%s).", ShaderName.GetBuffer(), ShaderFilepath.GetBuffer());
	return Shader;
}

LXTexture* LXAssetManager::CreateNewTexture(const LXString& MaterialName, const LXString& RelativeAssetFolder)
{
	LXFilepath AssetFolderpath = GetCore().GetProject()->GetAssetFolder();
	LXFilepath TextureFilepath;

	if (MaterialName.IsEmpty())
	{
		int i = 0;
		while (1)
		{
			LXString GeneratedTextureName = L"T_Texture" + LXString::Number(i);
			TextureFilepath = AssetFolderpath + RelativeAssetFolder + GeneratedTextureName + L"." + LX_TEXTURE_EXT;
			if (!TextureFilepath.IsFileExist())
				break;
			i++;
		}
	}
	else
	{
		TextureFilepath = AssetFolderpath + RelativeAssetFolder + MaterialName + L"." + LX_TEXTURE_EXT;
		if (TextureFilepath.IsFileExist())
		{
			LogW(ResourceManager, L"Texture %s already exist (%S).", MaterialName.GetBuffer(), TextureFilepath.GetBuffer());
			return false;
		}
	}


	LXTexture* Texture = new LXTexture();
	Texture->Owner = EResourceOwner::LXResourceOwner_Project;
	Texture->SetFilepath(TextureFilepath);
	Texture->State = LXAsset::EResourceState::LXResourceState_Loaded;
	Texture->TextureSource = ETextureSource::TextureSourceDynamic;
	Texture->Save();

	LXFilepath RelativeAssetFilepath = AssetFolderpath.GetRelativeFilepath(TextureFilepath);
	RelativeAssetFilepath.MakeLower();
	_MapAssets[RelativeAssetFilepath] = Texture;

	LogW(ResourceManager, L"Texture %s created (%S).", MaterialName.GetBuffer(), TextureFilepath.GetBuffer());
	return Texture;
}

LXAsset* LXAssetManager::Import(const LXFilepath& Filepath, const LXString& RelativeAssetFolder, bool Transient)
{
	LXString Extension = Filepath.GetExtension().MakeLower();

	LXFilepath AssetFolderpath = GetCore().GetProject()->GetAssetFolder();
	LXFilepath RelativeSourceFilepath = AssetFolderpath.GetRelativeFilepath(Filepath);

	bool FileAlreadyInAssetFolder = false;
	
	if (RelativeSourceFilepath.size() > 0)
	{
		FileAlreadyInAssetFolder = true;
	}
	else
	{
		AssetFolderpath = GetSettings().GetDataFolder();
		RelativeSourceFilepath = AssetFolderpath.GetRelativeFilepath(Filepath);
		if (RelativeSourceFilepath.size() > 0)
		{
			FileAlreadyInAssetFolder = true;
		}
	}
	
	//
	// Textures
	//

	if ((Extension == L"jpg") || 
		(Extension == L"pbm") || 
		(Extension == L"png") || 
		(Extension == L"hdr") ||
		(Extension == L"tga"))
	{

		LXFilepath FinalFilepath = Filepath;

		// Copy file to folder asset
		if (!FileAlreadyInAssetFolder)
		{
			FinalFilepath = AssetFolderpath + RelativeAssetFolder + Filepath.GetFilename();
			if (!CopyFile(Filepath, FinalFilepath, FALSE))
			{
				DWORD LastError = GetLastError();
				LogE(LXAssetManager, L"Unable to copy file %s to %s. Windows LastError: %i", Filepath.GetBuffer(), FinalFilepath.GetBuffer(), LastError);
			}
			RelativeSourceFilepath = AssetFolderpath.GetRelativeFilepath(FinalFilepath);
		}
		
		LXTexture* pTexture = new LXTexture();
		pTexture->SetPersistent(!Transient);
		LXFilepath AssetFilepath = FinalFilepath;
		AssetFilepath.RemoveExtension();
		AssetFilepath = AssetFilepath + L"." + LX_TEXTURE_EXT;
		
		pTexture->Owner = EResourceOwner::LXResourceOwner_Project;
		pTexture->SetFilepath(AssetFilepath);
		pTexture->SetSource(RelativeSourceFilepath);
		pTexture->LoadSource();

		// As we create a new asset we can consider it as loaded.
		pTexture->State = LXAsset::EResourceState::LXResourceState_Loaded;
		pTexture->Save();

		LXFilepath RelativeAssetFilepath = AssetFolderpath.GetRelativeFilepath(AssetFilepath);
		RelativeAssetFilepath.MakeLower();
		_MapAssets[RelativeAssetFilepath] = pTexture;

		LogI(AssetManager, L"Successful import %s", Filepath.GetBuffer());
		return pTexture;
	}
	else if (LXImporter* Importer = GetCore().GetPlugin(_pDocument, Extension))
	{
		LXAssetMesh* AssetMesh = nullptr;
		LXMesh* Mesh = Importer->Load(Filepath);
		
		if (Mesh)
		{
			// Path
			LXFilepath AssetFilepath = AssetFolderpath + RelativeAssetFolder + Filepath.GetFilename();
			AssetFilepath.RemoveExtension();
			AssetFilepath = AssetFilepath + L"." + LX_MESH_EXT;
			LXFilepath RelativeAssetFilepath = AssetFolderpath.GetRelativeFilepath(AssetFilepath);
			
			// Resource
			AssetMesh = new LXAssetMesh(Mesh);
			AssetMesh->SetPersistent(!Transient);
			AssetMesh->Owner = EResourceOwner::LXResourceOwner_Project;
			AssetMesh->SetFilepath(AssetFilepath);
			//MiniScene->SetSource(Filepath);

			// As we create a new asset we can consider it as loaded.
			AssetMesh->State = LXAsset::EResourceState::LXResourceState_Loaded;
			AssetMesh->Save();

			_MapAssets[RelativeAssetFilepath] = AssetMesh;

			LogI(AssetManager, L"Successful import %s", Filepath.GetBuffer());
		}
		else
		{
			LogE(AssetManager, L"Failed to import import %s", Filepath.GetBuffer());
		}
		
		delete Importer;
		return AssetMesh;
	}
	
	LogE(AssetManager, L"File not supported: %s", Filepath.GetBuffer());
	return nullptr;
}

void LXAssetManager::Add(LXAsset* Resource)
{
	CHK(Resource);

	if (Resource->GetFilepath().IsEmpty())
	{
		LXFilepath Filepath = GetSettings().GetDataFolder() + Resource->GetName() + L"." + Resource->GetFileExtension();
		Resource->SetFilepath(Filepath);
	}

	LXString key = Resource->GetName();
	key.MakeLower();
	auto It = _MapAssets.find(key);
	if (It != _MapAssets.end())
	{
		CHK(0); 
		return;
	}

	_MapAssets[key] = Resource;
}

void LXAssetManager::LoadFromFolder(const LXFilepath& Folderpath, EResourceOwner ResourceOwner)
{
	LXFilepath AssetFolderpath = Folderpath;
	LXFilepath Path = AssetFolderpath + "*";
	//Path += "*";
	LXDirectory dir(Path);
	for (auto& FileInfo : dir.GetListFileNames())
	{
		LXFilepath Filepath(FileInfo.FullFileName);
		LXFilepath RelativeFilepath = AssetFolderpath.GetRelativeFilepath(Filepath);
		LXString Extension = Filepath.GetExtension().MakeLower();
		
		LXString AssetKey = BuildKey(ResourceOwner, RelativeFilepath);
		AssetKey.MakeLower();

		LXAsset* Asset = FindAsset(AssetKey);
		if (Asset)
		{
			LogE(AssetManager, L"Asset %s already exist", AssetKey.GetBuffer())
			continue;
		}
		
		if (Extension == LX_MATERIAL_EXT)
		{
			LXMaterial* pMaterial = new LXMaterial;
			pMaterial->SetFilepath(Filepath);
			pMaterial->Owner = ResourceOwner;
			_MapAssets[AssetKey] = pMaterial;
			LogI(AssetManager,L"Added %s", Filepath.GetBuffer());
		}
		else if (Extension == LX_TEXTURE_EXT)
		{
			LXTexture* pTexture = new LXTexture();
			pTexture->SetFilepath(Filepath);
			pTexture->Owner = ResourceOwner;
			_MapAssets[AssetKey] = pTexture;
			LogI(AssetManager,L"Added %s", Filepath.GetBuffer());
		}
		else if (Extension == LX_MESH_EXT)
		{
			LXAssetMesh* pMesh = new LXAssetMesh();
			pMesh->SetFilepath(Filepath);
			pMesh->Owner = ResourceOwner;
			_MapAssets[AssetKey] = pMesh;
			LogI(AssetManager, L"Added %s", Filepath.GetBuffer());
		}
		else if (Extension == LX_SHADER_EXT)
		{
			LXShader* Shader = new LXShader();
			Shader->SetFilepath(Filepath);
			Shader->Owner = ResourceOwner;
			_MapAssets[AssetKey] = Shader;
			LogI(AssetManager, L"Added %s", Filepath.GetBuffer());
		}
				

#ifdef LX_USE_RAW_TEXTURES_AS_ASSETS
		else if ((Extension == L"jpg") || 
				 (Extension == L"pbm") || 
				 (Extension == L"png") || 
				 (Extension == L"tga"))
		{
			LXTexture* pTexture = new LXTexture();
			pTexture->SetSource(RelativeFilepath);
			pTexture->Owner = ResourceOwner;
			_MapAssets[AssetKey] = pTexture;
			LogI(AssetManager,L"Added %s", Filepath.GetBuffer());
		}
#endif 	
		else
		{
			//LogE(AssetManager, L"Unknow resource file format %s", Filepath.GetBuffer());
		}
	}
}

LXMaterial* LXAssetManager::CreateEngineMaterial(const wchar_t* szName)
{
	LXMaterial* Material = new LXMaterial();
	Material->SetName(szName);
	Material->State = LXAsset::EResourceState::LXResourceState_NotAFile;
	Material->Owner = EResourceOwner::LXResourceOwner_Engine;
	Add(Material);
	return Material;
}

void LXAssetManager::BuildEngineMaterials()
{
	// Default Material
	{

	}
		
	// Light Icon
	{
		LXMaterial* pMaterialLight = CreateEngineMaterial(L"sysLight");
		//LXShaderProgram* pShader = pDocument->GetShaderManager().GetShader(LX_SHADER_RENDERANCHOR);
		//pMaterialLight->SetShader(pShader);
//		pMaterialLight->SetLighting(false);
		pMaterialLight->SetColor(RED);
		//pMaterialLight->SetTextureDiffuse(L"light.png");
		pMaterialLight->SetPersistent(false);
		pMaterialLight->SetSystem(true);
	}

	// Anchor
	{
		LXMaterial* pMaterialAnchor = CreateEngineMaterial(L"sysAnchor");
		//LXShaderProgram* pShader = pDocument->GetShaderManager().GetShader(LX_SHADER_RENDERANCHOR);
		//pMaterialAnchor->SetShader(pShader);
//		pMaterialAnchor->SetLighting(false);
		pMaterialAnchor->SetColor(RED);
		//pMaterialAnchor->SetTextureDiffuse(L"anchor.jpg");
		pMaterialAnchor->SetPersistent(false);
		pMaterialAnchor->SetSystem(true);
	}

	// Outline
	{
		LXMaterial* pBlack = CreateEngineMaterial(L"sysBlack");
//		pBlack->SetLighting(false);
		pBlack->SetColor(BLACK);
		pBlack->SetPersistent(false);
		pBlack->SetSystem(true);
		//pBlack->SetTextureDiffuse(L"stipple.jpg");
		/*
		Shader pour épaisseurs des lignes.
		TODO : Picking HS
		*/
		//LXShaderProgram* pShader = pDocument->GetShaderManager().GetShader(LX_SHADER_RENDERLINES);
		//pBlack->SetShader(pShader);
	}

	// White
	{
		LXMaterial* pWhite = CreateEngineMaterial(L"sysWhite");
		//pWhite->SetLighting(false);
		pWhite->SetColor(WHITE);
		pWhite->SetPersistent(false);
		pWhite->SetSystem(true);
	}

	// Red
	{
		LXMaterial* pRed = CreateEngineMaterial(L"sysRed");
		//pRed->SetLighting(false);
		pRed->SetColor(RED);
		pRed->SetPersistent(false);
		pRed->SetSystem(true);
	}

	// Green
	{
		LXMaterial* pGreen = CreateEngineMaterial(L"sysGreen");
		//pGreen->SetLighting(false);
		pGreen->SetColor(GREEN);
		pGreen->SetPersistent(false);
		pGreen->SetSystem(true);
	}

	// Blue
	{
		LXMaterial* pBlue = CreateEngineMaterial(L"sysBlue");
		//pBlue->SetLighting(false);
		pBlue->SetColor(BLUE);
		pBlue->SetPersistent(false);
		pBlue->SetSystem(true);
	}
}

template<typename T>
void LXAssetManager::GetAssets(list<T>& listResources) const
{
	for (auto It : _MapAssets)
	{
		if (T resource = dynamic_cast<T>(It.second))
			listResources.push_back(resource);
	}
}

void LXAssetManager::GetAssetsOfType(list<LXAsset*>& listResources, LXAsset* Asset) const
{
	if (dynamic_cast<LXMaterial*>(Asset))
	{
		GetMaterials((list<LXMaterial*>&)listResources);
	}
	else if (dynamic_cast<LXTexture*>(Asset))
	{
		GetTextures((list<LXTexture*>&)listResources);
	}
	else if (dynamic_cast<LXShader*>(Asset))
	{
		GetShaders((list<LXShader*>&)listResources);
	}
	else if (dynamic_cast<LXAssetMesh*>(Asset))
	{
		GetMeshes((list<LXAssetMesh*>&)listResources);
	}
	else
	{
		if (Asset != nullptr)
		{
			LogE(AssetManager, L"Unknown asset type. Return all of them");
		}

		GetAssets(listResources);
	}
}

void LXAssetManager::OnAssetRenamed(const LXString& Path, const LXString& OldName, const LXString& NewName, EResourceOwner ResourceOwner)
{
	LXFilepath OldFilepath = Path + L"/" + OldName;
	LXFilepath NewFilepath = Path + L"/" + NewName;

	CHK(NewFilepath.IsFileExist());

	LXFilepath AssetFolderpath;
	
	if (ResourceOwner == EResourceOwner::LXResourceOwner_Project)
	{
		LXProject* Project = GetCore().GetProject();
		AssetFolderpath = Project->GetAssetFolder();
	}
	else if (ResourceOwner == EResourceOwner::LXResourceOwner_Engine)
	{
		AssetFolderpath = GetSettings().GetDataFolder();
	}

	LXFilepath OldRelativeFilepath = AssetFolderpath.GetRelativeFilepath(OldFilepath);
	LXFilepath NewRelativeFilepath = AssetFolderpath.GetRelativeFilepath(NewFilepath);
	
	LXString OldAssetKey = BuildKey(ResourceOwner, OldRelativeFilepath);
	LXString NewAssetKey = BuildKey(ResourceOwner, NewRelativeFilepath);
	
	LXAsset* Asset = FindAsset(OldAssetKey);
	CHK(Asset);

	_MapAssets.erase(OldAssetKey);
	_MapAssets[NewAssetKey] = Asset;
}

LXAsset* LXAssetManager::FindAsset(const LXString& RelativeFilepath) const
{
	LXString LC = RelativeFilepath;
	LC.MakeLower();
	auto It = _MapAssets.find(LC);
	if (It != _MapAssets.end())
		return (*It).second;
	else 
		return nullptr;
}

void LXAssetManager::GetTextures(list<LXTexture*>& listTextures)const
{
	GetAssets<LXTexture*>(listTextures);
}

void LXAssetManager::GetScripts(list<LXScript*>& listScripts)const
{
	GetAssets<LXScript*>(listScripts);
}

void LXAssetManager::GetMaterials(list<LXMaterial*>& listMaterials)const
{
	GetAssets<LXMaterial*>(listMaterials);
}

void LXAssetManager::GetShaders(list<LXShader*>& listShaders)const
{
	GetAssets<LXShader*>(listShaders);
}

void LXAssetManager::GetMeshes(list<LXAssetMesh*>& listMeshes)const
{
	GetAssets<LXAssetMesh*>(listMeshes);
}

void LXAssetManager::GetAssets(list<LXAsset*>& listResources) const
{
	for (auto It : _MapAssets)
		listResources.push_back(It.second);
}

LXString LXAssetManager::BuildKey(EResourceOwner ResourceOwner, const LXString& RelativeFilepath)
{
	return RelativeFilepath;

	if (ResourceOwner == EResourceOwner::LXResourceOwner_Engine)
	{
		return L"engine:" + RelativeFilepath;
	}
	else if (ResourceOwner == EResourceOwner::LXResourceOwner_Project)
	{
		return L"project:" + RelativeFilepath;
	}
	else
	{
		CHK(0);
		return L"";
	}
}
