//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXAssetManager.h"
#include "LXAnimation.h"
#include "LXAssetMesh.h"
#include "LXConsoleManager.h"
#include "LXDirectory.h"
#include "LXFileWatcher.h"
#include "LXGraphTemplate.h"
#include "LXImporter.h"
#include "LXLogger.h"
#include "LXMaterial.h"
#include "LXMesh.h"
#include "LXProceduralTexture.h"
#include "LXProject.h"
#include "LXScript.h"
#include "LXSettings.h"
#include "LXShader.h"
#include "LXTexture.h"
#include "LXMemory.h" // --- Must be the last included ---

const wchar_t kDefaultMaterial[] = L"Materials/DefaultGrid.smat";
const wchar_t kDefaultTexture[] = L"Textures/grid.stex";
const wchar_t kTextureNoise4x4[] = L"Textures/Noise4x4.stex";
const shared_ptr<LXAsset> emptyAsset;

//------------------------------------------------------------------------------------------------------
// Console commands
//------------------------------------------------------------------------------------------------------

LXConsoleCommand1S CCCreateNewMaterial(L"Asset.CreateMaterial", [](const LXString& inMaterialName)
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
}, []()
{
	return GetCore().GetProject() != nullptr;
});

LXConsoleCommand1S CCCreateNewTexture(L"Asset.CreateTexture", [](const LXString& inTextureName)
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
}, []()
{
	return GetCore().GetProject() != nullptr;
});

LXConsoleCommand1S CCCreateNewShader(L"Asset.CreateShader", [](const LXString& inShaderName)
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
}, []()
{
	return GetCore().GetProject() != nullptr;
});

LXConsoleCommand1S CCCreateNewAnimation(L"Asset.CreateAnimation", [](const LXString& inAnimationName)
{
	LXProject* Project = GetCore().GetProject();
	if (!Project)
	{
		LogW(Core, L"No project");
	}
	else
	{
		Project->GetAssetManager().CreateNewAnimation(inAnimationName, LX_DEFAULT_ANIMATION_FOLDER);
	}
}, []()
{
	return GetCore().GetProject() != nullptr;
});

LXConsoleCommandT<bool> CSet_MonitorAssetFiles(L"Engine.ini", L"AssetManager", L"MonitorAssetFiles", L"false");

//------------------------------------------------------------------------------------------------------

const bool ForceLowercase = false;

LXAssetManager::LXAssetManager(LXProject* Project) :_pDocument(Project)
{
	CHK(IsMainThread());

	_graphMaterialTemplate = make_unique<LXGraphTemplate>();
	VRF(_graphMaterialTemplate->LoadWithMSXML(GetSettings().GetCoreFolder() + L"/GraphMaterialTemplate.xml"));
				
	_ListAssetExtentions.push_back(LX_MATERIAL_EXT);
	_ListAssetExtentions.push_back(LX_TEXTURE_EXT);
	_ListAssetExtentions.push_back(LX_SHADER_EXT);
	_ListAssetExtentions.push_back(LX_ANIMATION_EXT);
	
	// Engine Asset
	LoadFromFolder(GetSettings().GetDataFolder(), EResourceOwner::LXResourceOwner_Engine);
	
	// Project Asset
	if (Project->GetSeetronProject())
	{
		LoadFromFolder(Project->GetAssetFolder(), EResourceOwner::LXResourceOwner_Project);

		if (CSet_MonitorAssetFiles.GetValue() == true)
		{
			_projectFileWatcher = make_unique<LXFileWatcher>(Project->GetAssetFolder(), true);
			_projectFileWatcher->OnFileChanded([this, Project](const wstring& filepath)
				{
					UpdateAsset(filepath.c_str(), Project->GetAssetFolder(), EResourceOwner::LXResourceOwner_Project);
				});

			_projectFileWatcher->OnFileAdded([this, Project](const wstring& filepath)
				{
					AddAsset(filepath.c_str(), Project->GetAssetFolder(), EResourceOwner::LXResourceOwner_Project);
				});

			_projectFileWatcher->OnFileRemoved([this, Project](const wstring& filepath)
				{
					RemoveAsset(filepath.c_str(), Project->GetAssetFolder(), EResourceOwner::LXResourceOwner_Project);
				});
		}
	}
}

void LXAssetManager::Init()
{
	// Immediately load engine assets
	_defaultMaterial = GetDefaultMaterial();
}


LXAssetManager::~LXAssetManager()
{
}

const shared_ptr<LXMaterial> LXAssetManager::GetDefaultMaterial()
{
	const shared_ptr<LXMaterial>& Material = GetMaterial(kDefaultMaterial);
	CHK(Material);
	return Material;
}

const shared_ptr<LXTexture> LXAssetManager::GetDefaultTexture()
{
	const shared_ptr<LXTexture>& texture = GetTexture(kDefaultTexture);
	CHK(texture);
	return texture;
}

const shared_ptr<LXAsset> LXAssetManager::GetAsset(const LXString& Name) const
{
	LXString key = Name;
	key.MakeLower();

	auto It = _MapAssets.find(key);
	if (It != _MapAssets.end())
	{

		shared_ptr<LXAsset> asset;

		// Create the asset if needed
		if (It->second.Asset.expired())
		{
			asset = CreateAsset(It->second);
			It->second.Asset = asset;
		}

		const shared_ptr<LXAsset> Resource = It->second.Asset.lock();

		if (Resource->State == LXAsset::EResourceState::LXResourceState_Unloaded && !IsRenderThread())
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
			return emptyAsset;
		}
	}
	else
	{
		LogE(AssetManager, L"Resource %s not exists in ResourceManager", Name.GetBuffer());
		return emptyAsset;
	}
}

template <typename T>
const  shared_ptr<T> LXAssetManager::GetResourceT(const LXString& Name) const
{
	const shared_ptr<LXAsset>& asset = GetAsset(Name);
	return dynamic_pointer_cast<T>(asset);
}

const shared_ptr<LXMaterial> LXAssetManager::GetMaterial(const LXString& strFilename) const
{
	return GetResourceT<LXMaterial>(strFilename);
}

const shared_ptr<LXShader> LXAssetManager::GetShader(const LXString& strFilename) const
{
	return GetResourceT<LXShader>(strFilename);
}

const shared_ptr<LXTexture> LXAssetManager::GetNoiseTexture4x4()
{
	const shared_ptr<LXTexture>& texture = GetTexture(kTextureNoise4x4);
	CHK(texture);
	return texture;
}

const shared_ptr<LXTexture> LXAssetManager::GetTexture(const LXString& strFilename) const
{
	return GetResourceT<LXTexture>(strFilename);
}

const shared_ptr<LXAssetMesh> LXAssetManager::GetAssetMesh(const LXString& strFilename) const
{
	return GetResourceT<LXAssetMesh>(strFilename);
}

const shared_ptr<LXScript> LXAssetManager::GetScript(const LXString& strFilename) const
{
	return GetResourceT<LXScript>(strFilename);
}

void LXAssetManager::AddAsset(shared_ptr<LXAsset> asset)
{
	LXAssetFileInfo afi;
	afi.FullFileName = asset->GetFilepath();
	afi.Owner = asset->Owner;
	afi.Asset = asset;
	const LXFilepath AssetFolderpath = GetCore().GetProject()->GetAssetFolder();
	const LXFilepath relativeAssetFilepath = AssetFolderpath.GetRelativeFilepath(afi.FullFileName);
	LXString key = BuildKey(asset->Owner, relativeAssetFilepath);
	_MapAssets[key] = afi;
}

const shared_ptr<LXMaterial> LXAssetManager::CreateNewMaterial(const LXString& MaterialName, const LXString& RelativeAssetFolder)
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
			LogW(LXAssetManager, L"Material %s already exist (%S).", MaterialName.GetBuffer(), MaterialFilepath.GetBuffer());
			return GetMaterial(RelativeAssetFolder + MaterialName + L"." + LX_MATERIAL_EXT);
		}
	}
	

	shared_ptr<LXMaterial> Material = make_shared<LXMaterial>();
	Material->Owner = EResourceOwner::LXResourceOwner_Project;
	Material->SetFilepath(MaterialFilepath);
	Material->State = LXAsset::EResourceState::LXResourceState_Loaded;
	Material->Save();
	
	AddAsset(Material);

	LogI(LXAssetManager, L"Material %s created (%s).", MaterialName.GetBuffer(), MaterialFilepath.GetBuffer());
	return Material;
}

const shared_ptr<LXShader> LXAssetManager::CreateNewShader(const LXString& ShaderName, const LXString& RelativeAssetFolder)
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


	shared_ptr<LXShader> Shader = make_shared<LXShader>();
	Shader->Owner = EResourceOwner::LXResourceOwner_Project;
	Shader->SetFilepath(ShaderFilepath);
	Shader->State = LXAsset::EResourceState::LXResourceState_Loaded;
	Shader->SaveDefault();

	AddAsset(Shader);

	LogI(ResourceManager, L"Shader %s created (%s).", ShaderName.GetBuffer(), ShaderFilepath.GetBuffer());
	return Shader;
}

const shared_ptr<LXTexture> LXAssetManager::CreateNewTexture(const LXString& MaterialName, const LXString& RelativeAssetFolder)
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
			return nullptr;
		}
	}


	shared_ptr<LXTexture> Texture = make_shared<LXTexture>();
	Texture->Owner = EResourceOwner::LXResourceOwner_Project;
	Texture->SetFilepath(TextureFilepath);
	Texture->State = LXAsset::EResourceState::LXResourceState_Loaded;
	Texture->TextureSource = ETextureSource::TextureSourceMaterial;
	Texture->Save();

	AddAsset(Texture);

	LogW(ResourceManager, L"Texture %s created (%S).", MaterialName.GetBuffer(), TextureFilepath.GetBuffer());
	return Texture;
}

const shared_ptr<LXAnimation> LXAssetManager::CreateNewAnimation(const LXString& AnimationName, const LXString& RelativeAssetFolder)
{
	LXFilepath AssetFolderpath = GetCore().GetProject()->GetAssetFolder();
	LXFilepath AnimationFilepath;

	if (AnimationName.IsEmpty())
	{
		int i = 0;
		while (1)
		{
			LXString GeneratedAnimationName = L"M_Animation" + LXString::Number(i);
			AnimationFilepath = AssetFolderpath + RelativeAssetFolder + GeneratedAnimationName + L"." + LX_ANIMATION_EXT;
			if (!AnimationFilepath.IsFileExist())
				break;
			i++;
		}
	}
	else
	{
		AnimationFilepath = AssetFolderpath + RelativeAssetFolder + AnimationName + L"." + LX_ANIMATION_EXT;
		if (AnimationFilepath.IsFileExist())
		{
			LogW(ResourceManager, L"Animation %s already exist (%S).", AnimationName.GetBuffer(), AnimationFilepath.GetBuffer());
			return GetResourceT<LXAnimation>(RelativeAssetFolder + AnimationName + L"." + LX_ANIMATION_EXT);
		}
	}


	shared_ptr<LXAnimation> Animation = make_shared<LXAnimation>();
	Animation->Owner = EResourceOwner::LXResourceOwner_Project;
	Animation->SetFilepath(AnimationFilepath);
	Animation->State = LXAsset::EResourceState::LXResourceState_Loaded;
	Animation->Save();

	AddAsset(Animation);

	LogI(ResourceManager, L"Animation %s created (%s).", AnimationName.GetBuffer(), AnimationFilepath.GetBuffer());
	return Animation;
}

const shared_ptr<LXAsset> LXAssetManager::Import(const LXFilepath& Filepath, const LXString& RelativeAssetFolder, bool Transient)
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
		
		shared_ptr<LXTexture> pTexture = make_shared<LXTexture>();
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
	
		AddAsset(pTexture);

		LogI(AssetManager, L"Successful import %s", Filepath.GetBuffer());
		return pTexture;
	}
	else if (LXImporter* Importer = GetCore().GetPlugin(_pDocument, Extension))
	{
		shared_ptr<LXAssetMesh> AssetMesh;
		shared_ptr<LXMesh>& Mesh = Importer->Load(Filepath);
				
		if (Mesh)
		{
			// Path
			LXFilepath AssetFilepath = AssetFolderpath + RelativeAssetFolder + Filepath.GetFilename();
			AssetFilepath.RemoveExtension();
			AssetFilepath = AssetFilepath + L"." + LX_MESH_EXT;
			LXFilepath RelativeAssetFilepath = AssetFolderpath.GetRelativeFilepath(AssetFilepath);
			
			// Resource
			shared_ptr<LXAssetMesh> AssetMesh = make_shared<LXAssetMesh>(Mesh);
			AssetMesh->SetPersistent(!Transient);
			AssetMesh->Owner = EResourceOwner::LXResourceOwner_Project;
			AssetMesh->SetFilepath(AssetFilepath);
			//MiniScene->SetSource(Filepath);

			// As we create a new asset we can consider it as loaded.
			AssetMesh->State = LXAsset::EResourceState::LXResourceState_Loaded;
			AssetMesh->Save();

			AddAsset(AssetMesh);

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

bool LXAssetManager::IsValidFile(const LXFilepath& filepath)
{
	// Ignore the temp and generated file
	if (filepath.GetFilename().Left(1) == L"_")
	{
		return false;
	}

	return true;
}

void LXAssetManager::AddAsset(const LXFilepath& filepath, const LXFilepath& assetFolderpath, EResourceOwner resourceOwner)
{
	if (!IsValidFile(filepath))
		return;
	
	LXFilepath RelativeFilepath = assetFolderpath.GetRelativeFilepath(filepath);
	LXString Extension = filepath.GetExtension().MakeLower();
	LXString AssetKey = BuildKey(resourceOwner, RelativeFilepath);
	AssetKey.MakeLower();

	shared_ptr<LXAsset> asset = FindAsset(AssetKey);
 
	auto It = _MapAssets.find(AssetKey); 
	if (It != _MapAssets.end())
	{
		LogE(AssetManager, L"Asset %s already exist", AssetKey.GetBuffer())
		return;
	}

	LXAssetFileInfo assetFileInfo;
	assetFileInfo.FullFileName = filepath;
	assetFileInfo.Owner = resourceOwner;
	_MapAssets[AssetKey] = assetFileInfo;
	LogI(AssetManager, L"Added %s", filepath.GetBuffer());
}

shared_ptr<LXAsset> LXAssetManager::CreateAsset(const LXAssetFileInfo& assetFileInfo) const
{
	LXString Extension = assetFileInfo.FullFileName.GetExtension().MakeLower();
 
	shared_ptr<LXAsset> asset;
 
	if (Extension == LX_MATERIAL_EXT)
	{
		asset = make_shared<LXMaterial>();
	}
	else if (Extension == LX_TEXTURE_EXT)
	{
		asset = make_shared<LXTexture>();
	}
	else if (Extension == LX_MESH_EXT)
	{
		asset = make_shared<LXAssetMesh>();
	}
	else if (Extension == LX_SHADER_EXT)
	{
		asset = make_shared<LXShader>();
	}
	else if (Extension == LX_ANIMATION_EXT)
	{
		asset = make_shared<LXAnimation>();
	}
	
	if (asset)
	{
		asset->SetFilepath(assetFileInfo.FullFileName);
		asset->Owner = assetFileInfo.Owner;
	}
	else
	{
		CHK(0);
	}

	return asset;
}

void LXAssetManager::RemoveAsset(const LXFilepath& filepath, const LXFilepath& assetFolderpath, EResourceOwner resourceOwner)
{
	if (!IsValidFile(filepath))
		return;
}

void LXAssetManager::UpdateAsset(const LXFilepath& filepath, const LXFilepath& assetFolderpath, EResourceOwner resourceOwner)
{
	if (!IsValidFile(filepath))
		return;

	LXFilepath RelativeFilepath = assetFolderpath.GetRelativeFilepath(filepath);
	LXString Extension = filepath.GetExtension().MakeLower();
	LXString AssetKey = BuildKey(resourceOwner, RelativeFilepath);
	AssetKey.MakeLower();

	shared_ptr<LXAsset> asset = FindAsset(AssetKey);

	if (asset)
	{
		asset->Reload();
	}
}

void LXAssetManager::LoadFromFolder(const LXFilepath& assetFolderpath, EResourceOwner ResourceOwner)
{
	LXDirectory dir(assetFolderpath);
	for (auto& FileInfo : dir.GetListFileNames())
	{
		LXFilepath filepath(FileInfo.FullFileName);
		AddAsset(filepath, assetFolderpath, ResourceOwner);
	}
}

template<typename T>
void LXAssetManager::GetAssets(list<shared_ptr<T>>& listAssets) const
{
	for (const auto& it : _MapAssets)
	{
		if (shared_ptr<T> typedAsset = dynamic_pointer_cast<T>(it.second.Asset.lock()))
		{
			listAssets.push_back(typedAsset);
		}
	}
}

void LXAssetManager::GetAssetsOfType(list<shared_ptr<LXAsset>>& listAssets, EAssetType assetType) const
{
	switch (assetType)
	{
	case EAssetType::Material:
		GetMaterials((list<shared_ptr<LXMaterial>>&)listAssets);
		break;
	case EAssetType::Texture:
		GetTextures((list<shared_ptr<LXTexture>>&)listAssets);
		break;
	case EAssetType::Shader:
		GetShaders((list<shared_ptr<LXShader>>&)listAssets);
		break;
	case EAssetType::Mesh:
		GetMeshes((list<shared_ptr<LXAssetMesh>>&)listAssets);
		break;
	case EAssetType::Undefined:
	default:
		GetAssets(listAssets);
		break;
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
	
	shared_ptr<LXAsset> Asset = FindAsset(OldAssetKey);

	_MapAssets.erase(OldAssetKey);
	AddAsset(Asset);
}

shared_ptr<LXAsset> LXAssetManager::FindAsset(const LXString& RelativeFilepath) const
{
	LXString LC = RelativeFilepath;
	LC.MakeLower();
	auto It = _MapAssets.find(LC);
	if (It != _MapAssets.end())
	{
		return It->second.Asset.lock();
	}
	else
	{
		shared_ptr<LXAsset> empty;
		return empty;
	}
}

void LXAssetManager::GetTextures(list<shared_ptr<LXTexture>>& listTextures)const
{
	GetAssets<LXTexture>(listTextures);
}

void LXAssetManager::GetScripts(list<shared_ptr<LXScript>>& listScripts)const
{
	GetAssets<LXScript>(listScripts);
}

void LXAssetManager::GetMaterials(list<shared_ptr<LXMaterial>>& listMaterials)const
{
	GetAssets<LXMaterial>(listMaterials);
}

void LXAssetManager::GetShaders(list<shared_ptr<LXShader>>& listShaders)const
{
	GetAssets<LXShader>(listShaders);
}

void LXAssetManager::GetMeshes(list<shared_ptr<LXAssetMesh>>& listMeshes)const
{
	GetAssets<LXAssetMesh>(listMeshes);
}

void LXAssetManager::GetAssets(list<shared_ptr<LXAsset>>& listResources) const
{
	for (auto It : _MapAssets)
		listResources.push_back(It.second.Asset.lock());
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
