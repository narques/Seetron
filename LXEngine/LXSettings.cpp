//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXSettings.h"
#include "LXEngine.h"
#include "LXLogger.h"
#include "LXDirectory.h"

enum LXERenderer
{
	Renderer_D3D11,
	Renderer_D3D11_1
};

LXSettings& GetSettings()
{
	return *GetEngine().GetSettings();
}

LXSettings::LXSettings(void)
{
	_DataFolder = L"Data/";
	Load();
}

LXSettings::~LXSettings(void)
{
}

bool LXSettings::IsFolder(const LXString& strPath)
{
	return LXDirectory::Exists(strPath.GetBuffer());
}

void LXSettings::Load()
{
	LXFilepath strAppPath = LXEngine::GetAppPath();

	_DataFolder = strAppPath + L"Data/";
 
	if (!IsFolder(_DataFolder))
		 _DataFolder = strAppPath + L"../../Data/";
 
	 if (!IsFolder(_DataFolder))
		 _DataFolder = strAppPath + L"../../../Data/";
 
	 if(!IsFolder(_DataFolder))
		 _DataFolder = strAppPath + L"../../../ShaderShaker/Data/";

	if (!IsFolder(_DataFolder))
	{
		CHK(0);
		LogE(Settings, L"FATAL ERROR : Data folder not found.");
		MessageBox(0, L"Data folder not found", L"FATAL Error", MB_ICONERROR);
		exit(0);
	}

	InitPaths();
}

//
// ConcatPath replace "..\\" in the given path by a "user friendly" path
// c:\\Folder\\..\\File.ext ->  c:\\File.ext
//

void ConcatPath(LXString& strFilepath)
{
	int nPos = strFilepath.Find(L"../");
	if (nPos == -1)
		return;
	
	LXString left = strFilepath.Left(nPos-1);
	int nPos2 = left.ReverseFind(L'/');
	strFilepath = left.Left(nPos2+1) + strFilepath.Right(strFilepath.GetLength() - nPos - 3);
	ConcatPath(strFilepath);
}

void LXSettings::InitPaths()
{
	_TexturesFolder   = _DataFolder + L"Textures/";
	_MaterialsFolder  = _DataFolder + L"Materials/";
	_ModelsFolder     = _DataFolder + L"Models/";
	_ShadersFolder    = _DataFolder + L"Shaders/";
	_ProjectsFolder   = _DataFolder + L"../Projects/";
	_EngineFolder		  = _DataFolder + L"Engine/";
	_PluginsFolder    = LXEngine::GetAppPath();
	_Scripts		  = _DataFolder + L"Scripts/";

	ConcatPath(_DataFolder);
	ConcatPath(_TexturesFolder);
	ConcatPath(_MaterialsFolder);
	ConcatPath(_ModelsFolder);
	ConcatPath(_ShadersFolder);
	ConcatPath(_ProjectsFolder);
	ConcatPath(_EngineFolder);
	ConcatPath(_PluginsFolder);

	CHK(IsFolder(_TexturesFolder));
	CHK(IsFolder(_ShadersFolder));
	CHK(IsFolder(_EngineFolder));
	CHK(IsFolder(_PluginsFolder));
}
