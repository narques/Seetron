//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXCore.h"
#include "LXActorFactory.h"
#include "LXAnimationManager.h"
#include "LXAssetManager.h"
#include "LXCommandManager.h"
#include "LXConsoleManager.h"
#include "LXController.h"
#include "LXDirectory.h"
#include "LXDocumentManager.h"
#include "LXEventManager.h"
#include "LXImporter.h"
#include "LXLibrary.h"
#include "LXLogger.h"
#include "LXMeshFactory.h"
#include "LXMessageManager.h"
#include "LXPerformance.h"
#include "LXPlatform.h"
#include "LXPrimitiveFactory.h"
#include "LXProject.h"
#include "LXPropertyManager.h"
#include "LXRenderer.h"
#include "LXScene.h"
#include "LXSettings.h"
#include "LXSettings.h"
#include "LXTrackBallCameraManipulator.h"
#include "LXStatManager.h"
#include "LXStatistic.h"
#include "LXSyncEvent.h"
#include "LXThread.h"
#include "LXViewport.h"
#include "LXViewportManager.h"
#include "LXWindow.h"
#include "LXMemory.h" // --- Must be the last included ---

//------------------------------------------------------------------------------------------------------
// Console commands
//------------------------------------------------------------------------------------------------------

LXConsoleCommandNoArg CCListAllCommands(L"ListAllCommands", []()
{
	for (const LXConsoleCommand* ConsoleCommand : GetConsoleManager().ListCommands)
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

namespace
{
	LXCore* gCore = nullptr;
}

__int64 LXCore::FrameNumber = -1;

LXCore::LXCore()
{
#ifdef LX_DEBUGFLAG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
}

/*virtual*/
LXCore::~LXCore()
{
	delete m_commandManager;  
	delete m_documentManager;
	delete m_viewportManager;
	delete m_propertyManager;
	delete StatManager;

	LXConsoleManager::DeleteSingleton();
		
	LogI(Core,L"------ Core deleted ------");

	LXObject::TraceAll();
	LXLogger::DeleteSingleton();
}

void LXCore::Init()
{
	MainThread = GetCurrentThreadId();

	_settings = std::make_unique<LXSettings>();

	LogI(Core, L"------ Seetron Engine ------");

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
	_mainTasks = std::make_unique<LXTaskManager>();
	_syncTasks = std::make_unique<LXTaskManager>();

	// Plugins
#ifdef LX_EDITOR
	EnumPlugins();
#endif

	DefineProperties();
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
	CHK(gCore != nullptr);
	return *gCore;
}

LXCore*	LXCore::CreateCore()
{
	CHK(gCore == nullptr);
	gCore = new LXCore();
	gCore->Init();
	return gCore;
}

void LXCore::BeginShutdown()
{
	CHK(!_shutdowning);
	CloseProject();
	_shutdowning = true;
}

void LXCore::EndShutdow()
{
	_shutdowning = false;
	_shutdown = true;
	EngineShutdown.Invoke();
}

void LXCore::Destroy()
{
	CHK(_shutdown && !_shutdowning);
	gCore = nullptr;
	delete this;
}

void LXCore::EnumPlugins()
{
	LXString str = _settings->GetPluginsFolder() + "LXImporter*.dll";
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
		ValidatedFolder = _settings->GetProjectsFolder() + Name + L"/";

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

			//Copy common shader files.
			CopyFile(_settings->GetShadersFolder() + "Common.hlsl", ValidatedFolder + L"Assets/Shaders/Common.hlsl", FALSE);
			CopyFile(_settings->GetShadersFolder() + "Forward.hlsl", ValidatedFolder + L"Assets/Shaders/Forward.hlsl", FALSE);
			CopyFile(_settings->GetShadersFolder() + "LightingFunctions.hlsl", ValidatedFolder + L"Assets/Shaders/LightingFunctions.hlsl", FALSE);
			CopyFile(_settings->GetShadersFolder() + "LightingUtilities.hlsl", ValidatedFolder + L"Assets/Shaders/LightingUtilities.hlsl", FALSE);

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
		LXFilepath filapath = ValidatedFolder + Name + L"." + LX_PROJECT_EXT;
		LXProject* Document = new LXProject(filapath);
		GetEventManager()->BroadCastEvent(new LXEventResult(EEventType::ProjectCreated, true));
		Document->InitializeNewProject(Name);
		SetDocument(Document);
		result = ECreateProjectResult::Success;
		Document->SaveFile();
		GetEventManager()->PostEvent(new LXEventResult(EEventType::ProjectLoaded, true));
	}
					
	return result;
}

bool LXCore::LoadProject(const LXString& ProjectName)
{
	LXFilepath Filepath = ProjectName;
	if (!Filepath.IsFileExist())
	{
		Filepath = _settings->GetProjectsFolder() + ProjectName + "/" + ProjectName + L"." LX_PROJECT_EXT;
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

void LXCore::EnqueueTask(LXTask* task)
{
	CHK(!IsMainThread());
	_mainTasks->EnqueueTask(task);
}

void LXCore::EnqueueInvokeDelegate(LXDelegate<>* delegate)
{
	LXTask* task = new LXTaskCallBack([delegate]()
	{
		delegate->Invoke();
	});
	_mainTasks->EnqueueTask(task);
}

void LXCore::AddObjectForDestruction(LXObject* object)
{
	LXTask* task = new LXTaskCallBack([object]()
	{
		delete object;
	});
	_syncTasks->EnqueueTask(task);
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

	GetCore().GetDocumentManager().SetDocument(Document);

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
	if (_Renderer && RenderThread && FrameNumber > -1)
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
	// --- End of the frame ---
	//
	// | /
	// |/
	// |
	//
	// --- Here, RenderThread is waiting ---

	
	//
	// Synchronization
	//
	IsSyncPoint = true;
	
	FrameNumber++;
	Time.Update();
	LX_CYCLEPERFOSCOPE(MainThread_Run);


	// Do not remove the brackets, needed to measure time.
	{
		LX_PERFOSCOPE(Thread_Synchronisation);
		// Synchronize the data between MainThread and RenderThread
		GetController()->Run();
		_syncTasks->Run((float)Time.DeltaTime());
		_Renderer->Sync();
	}

	GetViewport()->GetCameraManipulator()->Update(Time.DeltaTime());

	if (_shutdowning)
	{
		if (!_syncTasks->HasTasks() &&
			!_mainTasks->HasTasks() &&
			!_Renderer->HasPendingTasks())
		{
			EndShutdow();
		}
	}

	IsSyncPoint = false;
	//
	// --- Begin Frame ---
	//
	// | 
	// |\
	// | \
	//
	// --- Restart the RenderThread ---
	if (_Renderer && RenderThread)
	{
		_Renderer->GetBeginEvent()->SetEvent();
	}

	// Logs
	GetLogger().Tick();

	// Dispatch Core Messages
	GetMessageManager()->Run();

	// Broadcast events posted outside the MainThread
	GetEventManager()->BroadCastEvents();

	// Tasks
	_mainTasks->Run((float)Time.DeltaTime());
	   

	// MainThread Actors Update
	if (GetScene())
	{
		GetScene()->Run(Time.DeltaTime());
	}

	// PrimitiveFactory
	if (GetPrimitiveFactory())
	{
		GetPrimitiveFactory()->Tick();
	}
	
	// If RenderThread is not used, call the Renderer
	if (_Renderer && !LXRenderer::gUseRenderThread)
	{
		_Renderer->Render_MainThread();
	}
}

LXMaterial* LXCore::GetDefaultMaterial() const
{
	if (GetProject())
		return GetProject()->GetAssetManager().GetDefaultMaterial();
	else
		return nullptr;
}

LXTexture* LXCore::GetDefaultTexture() const
{ 
	if (GetProject())
		return GetProject()->GetAssetManager().GetDefaultTexture();
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
	return gCore ? gCore->GetStatManager() : nullptr;
}

LXEventManager* GetEventManager()
{
	static LXEventManager EventManager;
	return &EventManager;
}

LXMessageManager* GetMessageManager()
{
	static LXMessageManager messageManager;
	return &messageManager;
}

LXController* GetController()
{
	return GetCore().GetController();
}

LXRenderer* GetRenderer()
{
	return GetCore().GetRenderer();
}

LXMeshFactory* GetMeshFactory()
{
	return GetProject() ? &GetProject()->GetMeshFactory() : nullptr;
}