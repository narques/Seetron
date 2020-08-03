//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXDocumentBase.h"
#include "LXCore.h"

class LXActor;
class LXActorCamera;
class LXActorLight;
class LXActorMesh;
class LXActorSceneCapture;
class LXAnimationManager;
class LXAssetManager;
class LXGraphTemplate;
class LXMeshFactory;
class LXPrimitive;
class LXQueryManager;
class LXScene;
class LXSelectionManager;
class LXSnapshotManager;
class LXTextureManager;
class LXViewState;
class LXViewStateManager;

typedef list<LXString> ListUID;
typedef map<LXFilepath, LXActor*> MapFilepathGroup;
typedef vector<LXActorLight*> ArrayLights;
typedef set<LXActorMesh*> SetMeshs;
typedef vector<LXActorMesh*> ArrayMeshs;

enum class ELoadingStatus
{
	NotLoaded,
	Loading,
	Loaded
};

class LXCORE_API LXProject : public LXSmartObjectContainer, public LXDocumentBase
{

public:

	LXProject(const LXFilepath& Filepath);
	virtual ~LXProject(void);

private:

	LXProject(const LXProject&);
	LXProject& operator=(const LXProject&);
	
public:

	//
	// Overridden from LXSmartObject
	//

	virtual bool				OnSaveChild				( const TSaveContext& saveContext ) const override;
	virtual bool				OnLoadChild				( const TLoadContext& loadContext ) override;
	
	//
	// Misc
	//

	// InitializeNewProject: Initializes a new empty project, creates the minimal required objects (Camera and Light).
	bool						InitializeNewProject	( const LXString& strName );
	bool						LoadFile				( const LXFilepath& strFilename );
	bool						IsLoading				( ) const { return m_eLoadingStatus == ELoadingStatus::Loading; }
	bool						IsLoaded				( ) const { return m_eLoadingStatus == ELoadingStatus::Loaded; }
	void						SetLoading				( ) { m_eLoadingStatus = ELoadingStatus::Loading; }
	void						SetNotLoaded			( ) { m_eLoadingStatus = ELoadingStatus::NotLoaded; }
	void						SetLoaded				( ) { m_eLoadingStatus = ELoadingStatus::Loaded; }
	void						OnFilesLoaded			( bool Success );
	
	//
	// Managers
	//

	LXSelectionManager&			GetSelectionManager		( )  { CHK(m_pSelectionManager); return *m_pSelectionManager; }
	LXQueryManager&				GetQueryManager			( )  { CHK(m_pQueryManager); return *m_pQueryManager; }
	LXViewStateManager&			GetViewStateManager		( )  { CHK(m_pViewStateManager);  return *m_pViewStateManager; }
	LXAnimationManager&			GetAnimationManager		( )  { CHK(m_pAnimationManager); return *m_pAnimationManager; }
	LXSnapshotManager&			GetSnapshotManager		( )  { CHK(m_pSnapshotManager); return *m_pSnapshotManager; }
	LXAssetManager&				GetAssetManager			( )  { CHK(m_pResourceManager); return *m_pResourceManager; }
	LXPrimitiveFactory&			GetPrimitiveFactory		( )  { CHK(m_pPrimitiveFactory); return *m_pPrimitiveFactory; }
	LXMeshFactory&				GetMeshFactory			( )  { CHK(_meshFactory); return *_meshFactory; }
	
	//
	// Properties
	//

	vec3f						GetUpAxis				( ) { return LX_VEC3F_Z; };

	//
	// SceneGraph & states
	//

	LXActor*					CreateGroup				( );
	LXActorMesh*				CreateMesh				( );
	LXPrimitive*				CreatePrimitive			( );

	LXScene*					GetScene				( ) { return m_pScene; }
	LXActorCamera*				GetCamera				( ) const;
	
	LXActorSceneCapture*		GetSceneCapture			( ) { return m_pSceneCapture;}
	void						SetSceneCapture			( LXActorSceneCapture* ActorSceneCapture ) { m_pSceneCapture = ActorSceneCapture; }
	
	void						CreateTerrainActor		( );
	void						CreateCubeActor			( );
	void						CreatePlaneActor		( );
	void						CreateSphereActor		( );
	LXFilepath					GetAssetFolder			( ) const;
	
	ArrayLights&				GetLights				( ) { return m_lights; }			
	void						GetMeshs				( SetMeshs& setMeshs );
	void						GetMeshs				( ArrayMeshs& meshs );
	void						GetVisibleMeshes		( SetActors& setMeshs );
	void						GetHiddenMeshes			( SetActors& setMeshs );
	void						GetActors				( SetActors& setActors );
	void						GetGroups				( SetSmartObjects& SmartObject );
	void						GetGroups				( SetSmartObjects& SmartObject, const PairPropetyIDVariant& item );
	void						GetGroups				( const ListUID& listUID, SetSmartObjects& SmartObject );
	LXSmartObject*				GetObjectFromUID		( const LXString& UID );
	
	LXViewState*				GetMainView				( ) const { return m_pMainView; }
	LXViewState*				GetActiveView			( ) const { return m_pActiveView; }
	void						SetActiveView			( LXViewState* pViewState ) { m_pActiveView = pViewState; }

	//
	// TREE
	//

	vec3f*						GetNewObjectPosition	( ) const { return m_pNewObjectPosition; }
	void						SetNewObjectPosition	( vec3f* p) { m_pNewObjectPosition = p; }

	//
	// Assets
	//

		LXMaterial*					GetMaterial(const LXString& filename);
		const LXGraphTemplate*		GetGraphMaterialTemplate( );

	//
	// Tools
	//

	const LXFilepath			GetFolder				( );

	bool GetSeetronProject() const { return _seetronProject; };
	bool IsInitialized() const { return _init; }

public:

	// Delegates & Events
	LXDelegate<> ProjectLoaded;

	bool DepthOfField = false;
	bool SSAO = true;

private:

	//
	// Managers
	//

	// Serializable 
	LXViewStateManager*				m_pViewStateManager; 
	LXSelectionManager*				m_pSelectionManager;
	LXSnapshotManager*				m_pSnapshotManager;
	LXAssetManager*					m_pResourceManager;
		
	// Volatile 
	LXQueryManager*					m_pQueryManager;
	LXAnimationManager*				m_pAnimationManager;
	LXPrimitiveFactory*				m_pPrimitiveFactory;
	LXMeshFactory*					_meshFactory;
		
	//
	// SceneGraph & states
	//

	LXScene*						m_pScene = nullptr;			
	LXActorSceneCapture*			m_pSceneCapture = nullptr;	// Shortcut ( Owned by Scene )
	ArrayLights						m_lights;

	//
	// Misc
	//

	ELoadingStatus					m_eLoadingStatus = ELoadingStatus::NotLoaded;
	LXViewState*					m_pActiveView = nullptr;
	LXViewState*					m_pMainView = nullptr;	// Shortcut	
	
	vec3f*							m_pNewObjectPosition = nullptr;  // TREE : Position of the dropped tree

	bool							_seetronProject = false;
	bool							_init = false;
};
