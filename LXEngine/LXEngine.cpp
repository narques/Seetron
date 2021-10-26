//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXEngine.h"
#include "LXActorFactory.h"
#include "LXAnimationManager.h"
#include "LXAssetManager.h"
#include "LXCommandManager.h"
#include "LXConsoleManager.h"
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
#include "LXThreadManager.h"
#include "LXViewport.h"
#include "LXViewportManager.h"
#include "LXWindow.h"

//------------------------------------------------------------------------------------------------------
// Console commands
//------------------------------------------------------------------------------------------------------

LXConsoleCommandNoArg CCListAllCommands(L"ListAllCommands", []()
{
	for (const LXConsoleCommand* ConsoleCommand : GetConsoleManager().ListCommands)
	{
		LXString Msg = L"- " + ConsoleCommand->Name;
		LogI(Engine,L"%s", Msg.GetBuffer());
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
		LogI(Engine, L"%s", It.FileName.c_str());
	}
});

LXConsoleCommand2S CCCreateNewProject(L"CreateNewProject", [](const LXString& ProjectName, const LXString& FolderPath)
{
	GetEngine().CreateNewProject(ProjectName, FolderPath);
});

LXConsoleCommandNoArg CCSaveProject(L"SaveProject", []()
{
	if (GetEngine().GetProject() == nullptr)
	{
		LogW(Engine, L"No project");
		return;
	}

	if (GetEngine().GetProject()->SaveFile() == true)
		LogI(Engine, L"Saved project %s", GetEngine().GetProject()->GetFilepath().GetBuffer())
	else
		LogE(Engine, L"Failed to save project %s", GetEngine().GetProject()->GetFilepath().GetBuffer())
});

LXConsoleCommand2S CCLoadProject(L"LoadProject", [](const LXString& ProjectName, const LXString& FolderPath)
{
	GetEngine().LoadProject(ProjectName);
});

//------------------------------------------------------------------------------------------------------

namespace
{
	LXEngine* gEngine = nullptr;
}

__int64 LXEngine::FrameNumber = -1;

LXEngine::LXEngine()
{
#ifdef LX_DEBUGFLAG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
}

/*virtual*/
LXEngine::~LXEngine()
{
	delete m_commandManager;  
	delete m_documentManager;
	delete m_viewportManager;
	delete m_propertyManager;
	delete StatManager;

	LXConsoleManager::DeleteSingleton();
		
	LogI(Engine,L"------ Engine deleted ------");
}

void LXEngine::Init()
{
	MainThread = GetCurrentThreadId();

	_settings = std::make_unique<LXSettings>();

	LogI(Engine, L"------ Seetron Engine ------");

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
	_mainTasks = std::make_unique<LXTaskManager>();
	_syncTasks = std::make_unique<LXTaskManager>();

	// Plugins
#ifdef LX_EDITOR
	EnumPlugins();
#endif

	DefineProperties();
}

/*virtual*/
void LXEngine::DefineProperties( ) 
{
	//--------------------------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Misc");
	//--------------------------------------------------------------------------------------------------------------------------------

	// Selection Mode
	{
		LXPropertyEnum* p = DefinePropertyEnum( L"SelectionMode", LXPropertyID::ENGINE_SELECTIONMODE, (uint*)&m_eSelectionMode );
		p->AddChoice(L"Mesh", (uint)ESelectionMode::SelectionModeActor);
		p->AddChoice(L"Primitive", (uint)ESelectionMode::SelectionModePrimitive);
		p->AddChoice(L"Material", (uint)ESelectionMode::SelectionModeMaterial);
		p->SetPersistent(false);
	}

	// Undo
	{
		LXPropertyUint* pPropUint = DefinePropertyUint( L"UndoStackSize", LXPropertyID::ENGINE_UNDOSTACKSIZE, &m_nUndo);
		pPropUint->SetMinMax(0, 256);
	}

	//--------------------------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Import");
	//--------------------------------------------------------------------------------------------------------------------------------

	DefinePropertyBool(L"ImportMaterials", LXPropertyID::ENGINE_IMPORTMATERIALS, &m_bImportMaterials);
	DefinePropertyBool(L"ImportTextures", LXPropertyID::ENGINE_IMPORTTEXTURES, &m_bImportTextures);
}

LXEngine& LXEngine::GetEngine()
{
	CHK(gEngine != nullptr);
	return *gEngine;
}

LXEngine*	LXEngine::CreateEngine()
{
	CHK(gEngine == nullptr);
	gEngine = new LXEngine();
	gEngine->Init();
	return gEngine;
}

void LXEngine::BeginShutdown()
{
	CHK(!_shutdowning);
	LogI(Engine, L"BeginShutdown");
	CloseProject();
	_shutdowning = true;
}

void LXEngine::EndShutdow()
{
	LogI(Engine, L"EndShutdown");
	_shutdowning = false;
	_shutdown = true;
	EngineShutdown.Invoke();
}

void LXEngine::Destroy()
{
	CHK(_shutdown && !_shutdowning);
	LogI(Engine, L"Destroy");
	gEngine = nullptr;
	delete this;
	LXObject::TraceAll();
	LXLogger::DeleteSingleton();
}

void LXEngine::EnumPlugins()
{
	LXString str = _settings->GetPluginsFolder() + "LXImporter*.dll";
	LXDirectory dir(str);
	
	const ListFileInfos& list = dir.GetListFileNames();

	for (auto& It: list)
		RegisterPlugin(It.FileName.c_str());
}

void LXEngine::RegisterPlugin(const LXFilepath& strFilepath)
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
						LogI(Engine, L"Loaded %s", strFilepath.GetBuffer());
					}
				}
				else
					LogW(Engine,L"No extensions");
			}
		}
		else
		{
			LogW(Engine,L"Entry point not found : GetImporter");
		}
		
		library.Release();
	}
	else
	{
		LogW(Engine, L"Failed to load %s", strFilepath.GetBuffer());
	}
}

LXImporter* LXEngine::GetPlugin(LXProject* Project, const LXString& strExtension)
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
		LogW(Engine, L"No plugin for extension %s", strExtension.GetBuffer());
	}
	return NULL;
}

/*static*/
LXFilepath LXEngine::GetAppPath( )
{
	LXFilepath str = LXPlatform::GetApplicationFileName().c_str();
	str.ReplaceAll(L"\\", L"/");
	return str.GetFilepath();
}

LXProject*	LXEngine::GetProject( ) const
{ 
	return m_documentManager->GetDocument(); 
}

LXViewport*	LXEngine::GetViewport( ) const
{ 
	return m_viewportManager->GetViewport(); 
}

void LXEngine::BuildOpenDialogFilter(LXString& filter)
{
	filter = "Seetron Projects (*.seproj);;";
	for (auto el : m_mapExtension2Plugin)
	{
		LXString ext = el.first.GetBuffer();
		filter += L"(*." +  ext + L");;";
	}
	filter += "All Files (*.*)";
}

ECreateProjectResult LXEngine::CreateNewProject(const LXString& Name, const LXString& FolderPath )
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
			LogE(Engine,L"Folder %s is not empty", ValidatedFolder.GetBuffer());
			result = ECreateProjectResult::Error_FolderNoEmpty;
		}
	}
	else
	{
		if (CreateDirectory(ValidatedFolder, NULL) == TRUE)
		{
			LogI(Engine,L"Created folder %s", ValidatedFolder.GetBuffer());
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
			case ERROR_ALREADY_EXISTS: LogE(Engine, L"Failed to create folder %s (ERROR_ALREADY_EXISTS)", ValidatedFolder.GetBuffer()); break;
			case ERROR_PATH_NOT_FOUND: LogE(Engine, L"Failed to create folder %s (ERROR_PATH_NOT_FOUND)", ValidatedFolder.GetBuffer()); break;
			default: LogE(Engine,L"Failed to create folder %s", ValidatedFolder.GetBuffer());
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

bool LXEngine::LoadProject(const LXString& ProjectName)
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
		LogW(Engine, L"Project file %s does not exist", Filepath.GetBuffer());
		return false;
	}
}

bool LXEngine::LoadFile(const LXString& Filename)
{
	// Close the current project
	CloseProject();
	LXProject* Project = m_documentManager->LoadFile(Filename);
	SetDocument(Project);
	return Project ? true : false;
}

void LXEngine::CloseProject()
{
	LXProject* Project = m_documentManager->GetDocument();
	SetDocument(nullptr);
	LX_SAFE_DELETE(Project);
	GetEventManager()->BroadCastEvent(EEventType::ProjectClosed);
}

void LXEngine::SetPlayMode(bool bPlay)
{
	m_bPlay = bPlay;
	if (GetProject())
		GetProject()->GetAnimationManager().Play(m_bPlay);
}

void LXEngine::SetRenderer(LXRenderer* Renderer)
{
	_Renderer = Renderer;
}

LXRenderer* LXEngine::GetRenderer() const
{
	return _Renderer;
}

void LXEngine::EnqueueTask(LXTask* task)
{
	CHK(!IsMainThread());
	_mainTasks->EnqueueTask(task);
}

void LXEngine::EnqueueInvokeDelegate(LXDelegate<>* delegate)
{
	LXTask* task = new LXTaskCallBack([delegate]()
	{
		delegate->Invoke();
	});
	_mainTasks->EnqueueTask(task);
}

void LXEngine::EnqueueInvokeDelegate(const LXDelegate<>* delegate)
{
	LXTask* task = new LXTaskCallBack([delegate]()
	{
		delegate->Invoke();
	});
	_mainTasks->EnqueueTask(task);
}

void LXEngine::AddObjectForDestruction(LXObject* object)
{
	LXTask* task = new LXTaskCallBack([object]()
	{
		delete object;
	});
	_syncTasks->EnqueueTask(task);
}

void LXEngine::SetDocument(LXProject* Document)
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

	GetEngine().GetDocumentManager().SetDocument(Document);

	if (_Renderer)
	{
		_Renderer->SetDocument(Document);
		
		if (RenderThread)
		{
			_Renderer->GetBeginEvent()->SetEvent();
		}
	}
}

void LXEngine::Run()
{
	if (_shutdown)
		return;

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

	// Dispatch Engine Messages
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

LXMaterial* LXEngine::GetDefaultMaterial() const
{
	if (GetProject())
		return GetProject()->GetAssetManager().GetDefaultMaterial().get();
	else
		return nullptr;
}

LXTexture* LXEngine::GetDefaultTexture() const
{ 
	if (GetProject())
		return GetProject()->GetAssetManager().GetDefaultTexture().get();
	else
		return nullptr;
}

void LXEngine::ResetShaders()
{
	if (_Renderer)
		_Renderer->ResetShaders();
}

//------------------------------------------------------------------------------------------------------

LXEngine& GetEngine()
{
	return LXEngine::GetEngine();
}

LXProject* GetProject()
{
	return GetEngine().GetProject();
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
	return GetEngine().GetActorFactory();
}

LXStatManager* GetStatManager()
{
	return gEngine ? gEngine->GetStatManager() : nullptr;
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

LXRenderer* GetRenderer()
{
	return GetEngine().GetRenderer();
}

LXMeshFactory* GetMeshFactory()
{
	return GetProject() ? &GetProject()->GetMeshFactory() : nullptr;
}