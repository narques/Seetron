//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXCore.h"
#include "LXSettings.h"
#include "LXProject.h"
#include "LXViewport.h"
#include "LXImporter.h"
#include "LXLogger.h"
#include "LXPropertyManager.h"
#include "LXCommandManager.h"
#include "LXDocumentManager.h"
#include "LXViewportManager.h"
#include "LXAnimationManager.h"
#include "LXAssetManager.h"
#include "LXRenderer.h"
#include "LXThread.h"
#include "LXWindow.h"
#include "LXPerformance.h"
#include "LXPlatform.h"
#include "LXLibrary.h"
#include "LXDirectory.h"
#include "LXStatistic.h"
#include "LXConsoleManager.h"
#include "LXScene.h"
#include "LXController.h"
#include "LXActorFactory.h"
#include "LXStatManager.h"
#include "LXEventManager.h"
#include "LXMemory.h" // --- Must be the last included ---

//------------------------------------------------------------------------------------------------------
// Console commands
//------------------------------------------------------------------------------------------------------

LXConsoleCommandNoArg CCListAllCommands(L"ListAllCommands", []()
{
	for (LXConsoleCommand* ConsoleCommand : GetConsoleManager().ListCommands)
	{
		LXString Msg = L"- " + ConsoleCommand->Name;
		LogI(Core,L"%s", Msg.GetBuffer());
	}
});

LXConsoleCommandNoArg CCExits(L"Exit", []()
{
	exit(0);
});

LXConsoleCommandNoArg CCListProjects(L"ListProjects", []()
{
	LXString str = GetSettings().GetProjectsFolder() + L"*";
	LXDirectory dir(str);
	auto& list = dir.GetListFileNames();
	for (auto& It : list)
	{
		LogI(Core, L"%s", It.FileName.c_str());
	}
});

LXConsoleCommand2S CCCreateNewProject(L"CreateNewProject", [](const LXString& ProjectName, const LXString& FolderPath)
{
	GetCore().CreateNewProject(ProjectName, FolderPath);
});

LXConsoleCommandNoArg CCSaveProject(L"SaveProject", []()
{
	if (GetCore().GetProject() == nullptr)
	{
		LogW(Core, L"No project");
		return;
	}

	if (GetCore().GetProject()->SaveFile() == true)
		LogI(Core, L"Saved project %s", GetCore().GetProject()->GetFilepath().GetBuffer())
	else
		LogE(Core, L"Failed to save project %s", GetCore().GetProject()->GetFilepath().GetBuffer())
});

LXConsoleCommand2S CCLoadProject(L"LoadProject", [](const LXString& ProjectName, const LXString& FolderPath)
{
	GetCore().LoadProject(ProjectName);
});

//------------------------------------------------------------------------------------------------------

LXCore::LXCore()
{
	MainThread = GetCurrentThreadId();

	LogI(Core,L"------ Seetron Engine ------");
	StatManager = new LXStatManager();

	GetLogger().LogConfigurationAndPlatform();
	GetLogger().LogDateAndTime();

	//LXPropertServer settings
	SetManageDefaultProperties(true);
	SetName(L"Application");

	// Properties
	m_nUndo = 16;
	m_bImportTextures = true;
	m_eSelectionMode = ESelectionMode::SelectionModeActor;
	
	// Load default properties
	LoadDefaultProperties();

	// Managers
	m_documentManager = new LXDocumentManager;
	m_viewportManager = new LXViewportManager;
	m_commandManager = new LXCommandManager;
	m_propertyManager = new LXPropertyManager;
	_ActorFactory = std::make_unique<LXActorFactory>();
	_Controller = std::make_unique<LXController>();
	
	// Observer
	m_commandManager->AddObserver(m_viewportManager);
	
	// Plugins
	EnumPlugins();
	DefineProperties();
}

/*virtual*/
LXCore::~LXCore()
{
	// Observer
	m_commandManager->RemoveObserver(m_viewportManager);

	delete m_commandManager;  
	delete m_documentManager;
	delete m_viewportManager;
	delete m_propertyManager;
	//delete m_pScriptEngine;
	delete StatManager;

	LogI(Core,L"------ Core deleted ------\n");
}

/*virtual*/
void LXCore::DefineProperties( ) 
{
	//--------------------------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Misc");
	//--------------------------------------------------------------------------------------------------------------------------------

	// Selection Mode
	{
		LXPropertyEnum* p = DefinePropertyEnum( L"SelectionMode", LXPropertyID::CORE_SELECTIONMODE, (uint*)&m_eSelectionMode );
		p->AddChoice(L"Mesh", (uint)ESelectionMode::SelectionModeActor);
		p->AddChoice(L"Primitive", (uint)ESelectionMode::SelectionModePrimitive);
		p->AddChoice(L"Material", (uint)ESelectionMode::SelectionModeMaterial);
		p->SetPersistent(false);
	}

	// Undo
	{
		LXPropertyUint* pPropUint = DefinePropertyUint( L"UndoStackSize", LXPropertyID::CORE_UNDOSTACKSIZE, &m_nUndo);
		pPropUint->SetMinMax(0, 256);
	}

	//--------------------------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Import");
	//--------------------------------------------------------------------------------------------------------------------------------

	DefinePropertyBool(L"ImportMaterials", LXPropertyID::CORE_IMPORTMATERIALS, &m_bImportMaterials);
	DefinePropertyBool(L"ImportTextures", LXPropertyID::CORE_IMPORTTEXTURES, &m_bImportTextures);
}

LXCore& LXCore::GetCore()
{
	static LXCore core;
	return core;
}

void LXCore::EnumPlugins()
{
	LXString str = GetSettings().GetPluginsFolder() + "LXImporter*.dll";
	LXDirectory dir(str);
	
	const ListFileInfos& list = dir.GetListFileNames();

	for (auto& It: list)
		RegisterPlugin(It.FileName.c_str());
}

void LXCore::RegisterPlugin(const LXFilepath& strFilepath)
{
	LXLibrary library;

	if (library.Load(strFilepath))
	{
		typedef LXImporter* (*PFNGETIMPORTER) (LXProject*);
		
		PFNGETIMPORTER pGetImporter = (PFNGETIMPORTER)library.GetFunction("GetImporter");

		if (pGetImporter)
		{
			LXImporter* pImporter = pGetImporter(nullptr);

			if (pImporter)
			{
				ListStrings listExtensions;
				pImporter->GetExtensions(listExtensions);
				delete pImporter;
				if (listExtensions.size())
				{
					for (ListStrings::iterator It = listExtensions.begin(); It!= listExtensions.end(); It++)
					{
						m_mapExtension2Plugin[*It] = strFilepath;
						LogI(Core, L"Loaded %s", strFilepath.GetBuffer());
					}
				}
				else
					LogW(Core,L"No extensions");
			}
		}
		else
		{
			LogW(Core,L"Entry point not found : GetImporter");
		}
		
		library.Release();
	}
	else
	{
		LogW(Core, L"Failed to load %s", strFilepath.GetBuffer());
	}
}

LXImporter* LXCore::GetPlugin(LXProject* Project, const LXString& strExtension)
{
	CHK(Project);
	if (!Project)
		return NULL;

	MapExtension2Plugin::iterator It = m_mapExtension2Plugin.find(strExtension);
	if (It!=m_mapExtension2Plugin.end())
	{
		LXLibrary library;
		if (library.Load(It->second))
		{
			typedef LXImporter* (*PFNGETIMPORTER) (LXProject*);
			PFNGETIMPORTER pGetImporter = (PFNGETIMPORTER)library.GetFunction("GetImporter"); 
			if (pGetImporter)
			{
				LXImporter* pImporter = pGetImporter(Project);
				return pImporter;
			}
			
			library.Release();
		}
	}
	else
	{
		LogW(Core, L"No plugin for extension %s", strExtension.GetBuffer());
	}
	return NULL;
}

/*static*/
LXFilepath LXCore::GetAppPath( )
{
	LXFilepath str = LXPlatform::GetApplicationFileName().c_str();
	str.ReplaceAll(L"\\", L"/");
	return str.GetFilepath();
}

LXProject*	LXCore::GetProject( ) const
{ 
	return m_documentManager->GetDocument(); 
}

LXViewport*	LXCore::GetViewport( ) const
{ 
	return m_viewportManager->GetViewport(); 
}

void LXCore::BuildOpenDialogFilter(LXString& filter)
{
	filter = "Seetron Projects (*.seproj);;";
	for (auto el : m_mapExtension2Plugin)
	{
		LXString ext = el.first.GetBuffer();
		filter += L"(*." +  ext + L");;";
	}
	filter += "All Files (*.*)";
}

ECreateProjectResult LXCore::CreateNewProject(const LXString& Name, const LXString& FolderPath )
{
	ECreateProjectResult result = ECreateProjectResult::Error_Unknown;
	bool CreateProject = false;
	
	LXFilepath ValidatedFolder;
	if (!FolderPath.IsEmpty())
		ValidatedFolder = FolderPath + Name + L"/";
	else
		ValidatedFolder = GetSettings().GetProjectsFolder() + Name + L"/";

	if (ValidatedFolder.IsFolderExist())
	{
		if (ValidatedFolder.IsFolderEmpty())
		{
			CreateProject = true;
		}
		else
		{
			LogE(Core,L"Folder %s is not empty", ValidatedFolder.GetBuffer());
			result = ECreateProjectResult::Error_FolderNoEmpty;
		}
	}
	else
	{
		if (CreateDirectory(ValidatedFolder, NULL) == TRUE)
		{
			LogI(Core,L"Created folder %s", ValidatedFolder.GetBuffer());
			CreateDirectory(ValidatedFolder + L"Assets/", NULL);
			CreateDirectory(ValidatedFolder + L"Assets/Textures/", NULL);
			CreateDirectory(ValidatedFolder + L"Assets/Materials/", NULL);
			CreateDirectory(ValidatedFolder + L"Assets/Shaders/", NULL);
			CreateDirectory(ValidatedFolder + L"Assets/Meshes/", NULL);
			CreateProject = true;
		}
		else
		{
			auto Error = GetLastError();
			switch (Error)
			{
			case ERROR_ALREADY_EXISTS: LogE(Core, L"Failed to create folder %s (ERROR_ALREADY_EXISTS)", ValidatedFolder.GetBuffer()); break;
			case ERROR_PATH_NOT_FOUND: LogE(Core, L"Failed to create folder %s (ERROR_PATH_NOT_FOUND)", ValidatedFolder.GetBuffer()); break;
			default: LogE(Core,L"Failed to create folder %s", ValidatedFolder.GetBuffer());
			}
			
			result = ECreateProjectResult::Error_FailedToCreateFolder;
		}
	}
	
	if (CreateProject)
	{
		CloseProject();
		LXProject* Document = new LXProject(ValidatedFolder + Name + L"." + LX_PROJECT_EXT);
		Document->InitializeNewProject(Name);
		SetDocument(Document);
		result = ECreateProjectResult::Success;
	}
					
	return result;
}

bool LXCore::LoadProject(const LXString& ProjectName)
{
	LXFilepath Filepath = ProjectName;
	if (!Filepath.IsFileExist())
	{
		Filepath = GetSettings().GetProjectsFolder() + ProjectName + "/" + ProjectName + L"." LX_PROJECT_EXT;
	}
	if (Filepath.IsFileExist())
	{
		return LoadFile(Filepath);
	}
	else
	{
		LogW(Core, L"Project file %s does not exist", Filepath.GetBuffer());
		return false;
	}
}

bool LXCore::LoadFile(const LXString& Filename)
{
	// Close the current project
	CloseProject();
	LXProject* Project = m_documentManager->LoadFile(Filename);
	SetDocument(Project);
	return Project ? true : false;
}

void LXCore::CloseProject()
{
	LXController* Controller = GetController();
	Controller->Purge();
	LXProject* Project = m_documentManager->GetDocument();
	SetDocument(nullptr);
	LX_SAFE_DELETE(Project);
	GetEventManager()->BroadCastEvent(EEventType::ProjectClosed);
}

void LXCore::SetPlayMode(bool bPlay)
{
	m_bPlay = bPlay;
	if (GetProject())
		GetProject()->GetAnimationManager().Play(m_bPlay);
}

void LXCore::SetRenderer(LXRenderer* Renderer)
{
	_Renderer = Renderer;
	_Controller->SetRenderer(Renderer);
}

LXRenderer* LXCore::GetRenderer() const
{
	return _Renderer;
}

void LXCore::SetDocument(LXProject* Document)
{
	CHK(!LoadingThread);

	if (_Renderer && RenderThread)
	{
		// Waiting until the frame's end
		_Renderer->GetEndEvent()->Wait();
		_Renderer->GetEndEvent()->Reset();
	}

	if (LXViewport* Viewport = GetViewportManager().GetViewport())
		Viewport->SetDocument(Document);

	if (_Renderer)
	{
		_Renderer->SetDocument(Document);
		
		if (RenderThread)
		{
			_Renderer->GetBeginEvent()->SetEvent();
		}
	}
}

void LXCore::Run()
{
	LX_PERFOSCOPE(MainThread_Run);

	Time.Update();
	
	// Broadcast events posted outside the MainThread
	GetEventManager()->BroadCastEvents();
	
	//
	// MainThread Actors Update
	//

// 	if (GetScene())
// 	{
// 		GetScene()->Update2(Time.DeltaTime());
// 	}
		
	if (_Renderer && RenderThread)
	{
		// Wait for the RenderThread frame end.
		// Do not remove the brackets, needed to measure the wait time.
		{
			LX_PERFOSCOPE(MainThread_WaitTime);
			_Renderer->GetEndEvent()->Wait();
		}
		_Renderer->GetEndEvent()->Reset();
	}
	
	//
	// --- Here, RenderThread is waiting ---
	//

	//
	// Synchronize the data between MainThread and RenderThread
	//
	
	// Do not remove the brackets, needed to measure time.
	{
		LX_PERFOSCOPE(Thread_Synchronisation);
		GetController()->Run();
	}
		
	//
	// Restart RenderThread
	//

	if (_Renderer && RenderThread)
	{
		_Renderer->GetBeginEvent()->SetEvent();
	}

	// If RenderThread is not used, call the Render function now.
	if (_Renderer && !LXRenderer::gUseRenderThread)
	{
		_Renderer->Render_MainThread();
	}
}

LXMaterial * LXCore::GetDefaultMaterial() const
{
	if (GetProject())
		return GetProject()->GetAssetManager().GetDefaultMaterial();
	else
		return nullptr;
}

void LXCore::ResetShaders()
{
	if (_Renderer)
		_Renderer->ResetShaders();
}

//------------------------------------------------------------------------------------------------------

LXCore& GetCore()
{
	return LXCore::GetCore();
}

LXProject* GetProject()
{
	return GetCore().GetProject();
}

LXScene* GetScene()
{
	return GetProject() ? GetProject()->GetScene() : nullptr;
}

LXAssetManager* GetAssetManager()
{
	return GetProject() ? &GetProject()->GetAssetManager() : nullptr;
}

LXSelectionManager* GetSelectionManager()
{
	return GetProject() ? &GetProject()->GetSelectionManager() : nullptr;
}

LXPrimitiveFactory* GetPrimitiveFactory()
{
	return GetProject() ? &GetProject()->GetPrimitiveFactory() : nullptr;
}

LXActorFactory* GetActorFactory()
{
	return GetCore().GetActorFactory();
}

LXStatManager* GetStatManager()
{
	return GetCore().GetStatManager();
}

LXEventManager* GetEventManager()
{
	static LXEventManager EventManager;
	return &EventManager;
}

LXController* GetController()
{
	return GetCore().GetController();
}

LXRenderer* GetRenderer()
{
	return GetCore().GetRenderer();
}
