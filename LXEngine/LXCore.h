//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXDocumentBase.h"
#include "LXTime.h"

class LXActorFactory;
class LXAssetManager;
class LXCommandManager;
class LXController;
class LXCounter;
class LXDocumentManager;
class LXEventManager;
class LXImporter;
class LXPrimitiveFactory;
class LXProject;
class LXPropertyManager;
class LXRenderer;
class LXScene;
class LXScriptEngine;
class LXSelectionManager;
class LXStatManager;
class LXThread;
class LXViewport;
class LXViewportManager;

typedef vector<LXCounter*> ArrayCounters;

enum class ECreateProjectResult
{
	Success,
	Error_FolderNoEmpty,
	Error_FailedToCreateFolder,
	Error_Unknown
};

enum class ESelectionMode
{
	SelectionModeActor,
	SelectionModePrimitive,
	SelectionModeMaterial
};

class LXCORE_API LXCore : public LXDocumentBase // In order to load/save setting TODO LXSmartObject[File|Data|...]
{

private:

	LXCore(void);   
	virtual ~LXCore();

public:
	
	static LXCore&		GetCore();
	
	// Properties Get/Set
	uint				GetUndoStackSize() const { return m_nUndo; }

	// Plugins
	LXImporter*			GetPlugin(LXProject* pDocument, const LXString& strExtension);

	// Misc
	static LXFilepath	GetAppPath();
	void				BuildOpenDialogFilter(LXString& filter);
	
	// Project management 
	ECreateProjectResult	CreateNewProject(const LXString& Name, const LXString& FolderPath);
	bool					LoadProject(const LXString& ProjectName);
	bool					LoadFile(const LXString& Filename);
	void					CloseProject();
	
	// Shortcuts
	LXProject*			GetProject() const;
	LXViewport*			GetViewport() const;

	// Managers
	LXDocumentManager&  GetDocumentManager(){ return *m_documentManager; }
	LXViewportManager&  GetViewportManager(){ return *m_viewportManager; }
	LXCommandManager&	GetCommandManager(){ return *m_commandManager; }
	LXPropertyManager&  GetPropertyManager(){ return *m_propertyManager; }
	LXScriptEngine&		GetScriptEngine(){ return *m_pScriptEngine;  }
	LXActorFactory*		GetActorFactory() { return _ActorFactory.get();  }
	LXController*		GetController() { return _Controller.get(); }
	LXStatManager*		GetStatManager() { return StatManager; }

	// Animation
	void				SetPlayMode(bool bPlay);
	bool				GetPlayMode() const { return m_bPlay; }

		


	// Rendering
	void				SetRenderer(LXRenderer* Renderer);
	LXRenderer*			GetRenderer() const;

	
	// Misc
	#ifdef LX_COUNTERS
	ArrayCounters&      GetCounters()  { return m_arrayCounters; }
	#endif

	// 
	void				Run();

	LXMaterial*			GetDefaultMaterial() const ;
	void				ResetShaders();
	
private:

	void				SetDocument(LXProject* Document);
	void				EnumPlugins();
	void                RegisterPlugin(const LXFilepath& strFilepath);
	void				DefineProperties();

private:

	//
	// Properties
	//

	// Importer option
	GetSetDef(bool, m_bImportMaterials, ImportMaterials, true);
	GetSetDef(bool, m_bImportTextures, ImportTextures, true);
	
	// Misc
	GetSet(ESelectionMode, m_eSelectionMode, SelectionMode);
	GetSetDef(bool, _bAutoKey, AutoKey, false);

public:

	LXTime				Time;
	__int64				Frame = -1;

private:

	uint				 m_nUndo;

	LXDocumentManager*	 m_documentManager;
	LXViewportManager*	 m_viewportManager;
	LXCommandManager*	 m_commandManager;
	LXPropertyManager*	 m_propertyManager;
	LXScriptEngine*		 m_pScriptEngine;
	LXStatManager*		 StatManager;
	
	std::unique_ptr<LXActorFactory> _ActorFactory;
	std::unique_ptr<LXController> _Controller;

	LXRenderer*		 _Renderer;

	//
	// Plugins management 
	//

	typedef map<LXString, LXFilepath> MapExtension2Plugin;
	MapExtension2Plugin	 m_mapExtension2Plugin;

	#ifdef LX_COUNTERS
	ArrayCounters		m_arrayCounters;
	#endif

	// Animation
	bool m_bPlay = false;
};

// Global functions

LXCORE_API LXCore& GetCore();
LXCORE_API LXProject* GetProject();
LXCORE_API LXScene* GetScene();
LXCORE_API LXAssetManager* GetAssetManager();
LXCORE_API LXSelectionManager* GetSelectionManager();
LXCORE_API LXActorFactory* GetActorFactory();
LXCORE_API LXPrimitiveFactory* GetPrimitiveFactory();
LXCORE_API LXController* GetController();
LXCORE_API LXRenderer* GetRenderer();
LXCORE_API LXStatManager* GetStatManager();
LXCORE_API LXEventManager* GetEventManager();
