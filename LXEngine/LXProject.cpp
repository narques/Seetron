//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXActorCamera.h"
#include "LXActorMeshCube.h"
#include "LXActorMeshPlane.h"
#include "LXActorMeshSphere.h"
#include "LXAnimationManager.h"
#include "LXAssetManager.h"
#include "LXAssetMesh.h"
#include "LXCommandManager.h"
#include "LXDocumentManager.h"
#include "LXActorMeshGizmo.h"
#include "LXActorLight.h"
#include "LXMesh.h"
#include "LXMSXMLNode.h"
#include "LXPerformance.h"
#include "LXPrimitive.h"
#include "LXPrimitiveFactory.h"
#include "LXProject.h"
#include "LXQueryManager.h"
#include "LXQueryTransform.h"
#include "LXScene.h"
#include "LXSelectionManager.h"
#include "LXSelectionTraverser.h"
#include "LXSettings.h"
#include "LXSnapshotManager.h"
#include "LXStatistic.h"
#include "LXTerrain.h"
#include "LXViewStateManager.h"
#include "LXMemory.h" // --- Must be the last included --- 

namespace
{
	const vec3f kDefaultCameraPosition = vec3f(200.f, 180.f, 80.f);
	const vec3f kDefaultLightPosition = vec3f(0.0f, 0.0f, 0.0f);
}

LXProject::LXProject(const LXFilepath& filepath)
{
	if (filepath.GetExtension() == LXString(LX_PROJECT_EXT).MakeLower())
		_seetronProject = true;
	else
		_seetronProject = false;
	
	m_strFilepath = filepath;

	// Refers to DocManager: Done first because some project managers can be use the link in their constructors
	GetCore().GetDocumentManager().SetDocument(this);

	// Managers
	m_pSelectionManager = new LXSelectionManager(this);
	m_pQueryManager = new LXQueryManager(this);
	m_pViewStateManager = new LXViewStateManager(this); 
	m_pAnimationManager = new LXAnimationManager(this);
	m_pSnapshotManager = new LXSnapshotManager(this);
	m_pResourceManager = new LXAssetManager(this);
	m_pPrimitiveFactory = new LXPrimitiveFactory();
					
	// SmartObject settings
	SetName(L"Project");
	SetManageFile(true);
	
	// SmartObject ( Order is important for Saving, ex. ViewState need a Material.
	AddSmartObject(m_pViewStateManager);
	AddSmartObject(m_pSelectionManager);
	AddSmartObject(m_pSnapshotManager);
	AddSmartObject(m_pAnimationManager);

	//
	// Misc Object 
	//

	// Scene
	m_pScene = new LXScene(this);

	// Gizmo
	LXActorMeshGizmo* ActorMeshGizmo = new LXActorMeshGizmo();
	m_pScene->AddChild(ActorMeshGizmo);
	
	// To manipulate Anchor or Gizmo
	GetCore().GetCommandManager().PushQuery(new LXQueryTransform());

	// Done
	_init = true;
}

LXProject::~LXProject(void)
{
	// SmartObjects
	LX_SAFE_DELETE(m_pScene);
	
	// Managers
	LX_SAFE_DELETE(m_pSelectionManager);
	LX_SAFE_DELETE(m_pQueryManager);
	LX_SAFE_DELETE(m_pViewStateManager); 
	LX_SAFE_DELETE(m_pAnimationManager);
	LX_SAFE_DELETE(m_pSnapshotManager);
	LX_SAFE_DELETE(m_pResourceManager);
	LX_SAFE_DELETE(m_pPrimitiveFactory);
	
	LogI(Project, L"Project deleted");

	// Unregister from the manager
	GetCore().GetDocumentManager().SetDocument(NULL);
}

bool LXProject::InitializeNewProject( const LXString& strName )
{
	//
	// Default Camera
	//

	LXActorCamera* ActorCamera = new LXActorCamera(this);
	ActorCamera->SetName(L"Camera");
	ActorCamera->SetPosition(kDefaultCameraPosition);
	m_pScene->AddChild(ActorCamera);
		
	//
	// Default Lighting
	//

	LXActorLight* pLight = new LXActorLight(this);
	pLight->SetName(L"Light");
	pLight->SetType(ELightType::Directional);
	pLight->SetPosition(kDefaultLightPosition);
	m_pScene->AddChild(pLight);
	
	//
	// Default object
	//

	LXActorMesh* Mesh = new LXActorMeshCube(this);
	Mesh->SetName(L"Cube");
	m_pScene->AddChild(Mesh);

	OnFilesLoaded(true);
	return true;
}

bool LXProject::LoadFile( const LXFilepath& strFilepath )
{
	if (!strFilepath.IsFileExist())
		return false;

	m_strFilepath = strFilepath;

	LXPerformance perf;

	LXString strExtension = strFilepath.GetExtension();

	bool bRet = false;

	if ( strExtension == LXString(LX_PROJECT_EXT).MakeLower())
	{
		bRet = Load(strFilepath, true, true);
	}
	else
	{
		//
		// Not a project file. Try to open it in a volatile viewer usage.
		//

		if (LXAssetMesh* AssetMesh = dynamic_cast<LXAssetMesh*>(GetAssetManager().Import(strFilepath, L"", true)))
		{
			// Set the project as Volatile and Viewer
			SetPersistent(false);

			// Default Camera
			LXActorCamera* ActorCamera = new LXActorCamera(this);
			ActorCamera->SetName(L"Camera");
			m_pScene->AddChild(ActorCamera);

			// Default Lighting
			LXActorLight* pLight = new LXActorLight(this);
			pLight->SetName(L"Light");
			pLight->SetType(ELightType::Directional);
			pLight->SetPosition(kDefaultLightPosition);
			m_pScene->AddChild(pLight);
			
			// ActorMesh 
			LXActorMesh* ActorMesh = new LXActorMesh();
			ActorMesh->SetMesh(AssetMesh->GetMesh());
			ActorMesh->SetAssetMesh(AssetMesh);
			GetScene()->AddChild(ActorMesh);

			// Clamp the min Actor size to 5m
			const LXBBox& bbox = ActorMesh->GetBBoxWorld();
			float Size = bbox.GetMaxSize();
			if (Size < 500.0f)
			{
				Size = 500.f / Size;
				ActorMesh->SetScale(vec3f(Size));
			}

			GetCore().GetCommandManager().SceneChanged();

			bRet = true;
		}
	}

	LogI(Project, L"File(s) loaded in %f ms", perf.GetTime());

	return bRet;
}

void LXProject::OnFilesLoaded( bool Success )
{
	// MainView
	m_pMainView = m_pViewStateManager->GetViewState(L"MainView");
	if (!m_pMainView)
	{
		m_pMainView = m_pViewStateManager->CreateViewState(this);
		m_pMainView->SetName(L"MainView");
		if (!GetPersistent())
			m_pMainView->ZoomOnBBoxAnimated(m_pScene->GetBBoxWorld());
	}
		
	// Default active view is MainView
	m_pActiveView = m_pMainView;
}

bool LXProject::OnSaveChild(const TSaveContext& saveContext) const
{
	//
	// Order is important !
	// Ex : ViewState needs a Material pointer, Animation needs some property pointers,...
	//
	
	m_pViewStateManager->Save(saveContext);
	//GetSelectionManager().Save(saveContext);
	//GetSnapshotManager().Save(saveContext);
	m_pScene->Save(saveContext);
	//GetAnimationManager().Save(saveContext);
	return true;
}

bool LXProject::OnLoadChild ( const TLoadContext& loadContext )
{ 
	const LXString& name = loadContext.node.name();
	
	if (name == L"LXScene")
	{
		m_pScene->Load(loadContext);
	}
	else
	{
		return false;
	}
	
	return true;
}

LXActor* LXProject::CreateGroup( )
{
	return new LXActor(this);
}

LXActorMesh*  LXProject::CreateMesh( )
{
	return new LXActorMesh(this);
}

LXPrimitive* LXProject::CreatePrimitive( )
{
	return new LXPrimitive;
}

LXActorCamera* LXProject::GetCamera() const
{
	return m_pScene ? m_pScene->GetCamera() : nullptr;
}

// TODO: Move to a SceneManager
void LXProject::GetMeshs( SetMeshs& setMeshs ) 
{
	LXSelectionTraverser3 t;
	t.SetScene(m_pScene);
	t.SetSetMeshs(&setMeshs);
	t.Apply();
}

// TODO: Move to a SceneManager
void LXProject::GetVisibleMeshes( SetActors& setMeshs )
{
	LXPerformance perf;
	LXSelectionTraverser4 t;
	t.SetScene(m_pScene);
	t.SetSetMeshs(&setMeshs);
	t.Apply();
	LX_COUNT("GetVisibleMeshes.LXSelectionTraverser4", perf.GetTime());
}

// TODO: Move to a SceneManager
void LXProject::GetHiddenMeshes( SetActors& setMeshs )
{
	LXSelectionTraverserCB t;
	t.SetScene(m_pScene);
	t.OnMeshCallBack([&setMeshs](LXActorMesh* pMesh)
	{
		if (!pMesh->IsVisible())
			setMeshs.insert(pMesh);
	});
	t.Apply();
}

void LXProject::GetMeshs(ArrayMeshs& meshs)
{
	LXSelectionTraverserCB t;
	t.SetScene(m_pScene);
	t.OnMeshCallBack([&meshs](LXActorMesh* pMesh)
	{
		meshs.push_back(pMesh);
	});
	t.Apply();
}

void LXProject::GetActors( SetActors& setActors )
{
	LXSelectionTraverser t;
	t.SetScene(m_pScene);
	t.SetSetActors(&setActors);
	t.Apply();
}

// TODO: Move to a SceneManager
void LXProject::GetGroups( SetSmartObjects& SmartObject )
{
	LXSelectionTraverser2 t;
	t.SetScene(m_pScene);
	t.SetSetGroups(&SmartObject);
	t.Apply();
}

// TODO: Move to a SceneManager
void LXProject::GetGroups(SetSmartObjects& setSmartObject, const PairPropetyIDVariant& item)
{
	LXSelectionTraverserCB t;
	t.SetScene(m_pScene);
	t.OnGroupCallBack([&setSmartObject, &item](LXActor* pGroup) // Lambda
	{
		if (pGroup)
		{
			auto p = pGroup->GetProperty(item.first);
			if (p)
			{
				LXVariant* variant = p->CreateVariant();
				if (variant->IsValueEqual(item.second))
					setSmartObject.insert(pGroup);
			}
		}
	});
	t.Apply();
}

// TODO: Move to a SceneManager
void LXProject::GetGroups( const ListUID& listUID, SetSmartObjects& SmartObject )
{
	LXSelectionTraverserCB t;
	t.SetScene(m_pScene);
	t.OnGroupCallBack([&listUID, &SmartObject](LXActor* pGroup) // Lambda
	{
		if (LXString* uid = pGroup->GetUID(false))
		{
			auto It = std::find(listUID.begin(), listUID.end(), *uid);
			if ( It != listUID.end())
				SmartObject.insert(pGroup);
		}
	});
	t.Apply();
}

LXSmartObject* LXProject::GetObjectFromUID(const LXString& UID)
{
	LXSmartObject* pResult = NULL;

	// --- Search in Scene ---; 
	
	LXSelectionTraverserCB t;
	t.SetScene(m_pScene);
	t.OnGroupCallBack([&UID, &pResult](LXActor* pGroup) // Lambda
	{
		if (LXString* uid = pGroup->GetUID(false))
		{
			if (*uid == UID)
			{
				CHK(!pResult); // Serious problem : at least two objects with the same UID !
				pResult = pGroup;
			}
		}
	});
	t.Apply();

	// --- Search in ViewStateManager ---

	if (!pResult)
	{
		auto& lvs = m_pViewStateManager->GetListViewStates();
		for (auto* viewState : lvs)
		{
			if (LXString* uid = viewState->GetUID(false))
			{
				if (*uid == UID)
				{
					pResult = viewState;
					break;
				}
			}
		}
	}
	
	return pResult;
}

LXMaterial* LXProject::GetMaterial(const LXString& filename)
{
	LXFilepath assetFolder = GetAssetFolder();
	LXFilepath relativeFilepath = assetFolder.GetRelativeFilepath(filename);

	if (relativeFilepath.IsEmpty())
	{
		assetFolder = GetSettings().GetDataFolder();
		relativeFilepath = assetFolder.GetRelativeFilepath(filename);
	}

	return GetAssetManager().GetMaterial(relativeFilepath);
}

const LXGraphTemplate* LXProject::GetGraphMaterialTemplate()
{
	return m_pResourceManager->GetGraphMaterialTemplate();
}

const LXFilepath LXProject::GetFolder()
{
	CHK(!m_strFilepath.IsEmpty());
	if (!m_strFilepath.IsEmpty())
	{
		LXFilepath ret = m_strFilepath.GetFilepath();
		return ret;
	}
	else
		return L"";
}

void LXProject::CreateTerrainActor()
{
	LXTerrain* Actor = new LXTerrain(this);
	m_pScene->AddChild(Actor);
}

void LXProject::CreateCubeActor()
{
	LXActorMesh* Actor = new LXActorMeshCube(this);
	m_pScene->AddChild(Actor);
}
			
void LXProject::CreatePlaneActor()
{
	LXActorMesh* Actor = new LXActorMeshPlane(this);
	m_pScene->AddChild(Actor);
}

void LXProject::CreateSphereActor()
{
	LXActorMesh* Actor = new LXActorMeshSphere(this);
	m_pScene->AddChild(Actor);
}

LXFilepath LXProject::GetAssetFolder() const
{
	LXFilepath AssetFolder;
	
	if (_seetronProject)
	{
		AssetFolder = m_strFilepath.GetFilepath() + L"Assets/";
		if (!AssetFolder.IsFolderExist())
		{
			CHK(0);
			return L"";
		}
	}
	else
	{
		// If the project is "volatile", acting as fbx viewer for example,
		// use the Engine asset folder.
		AssetFolder = GetSettings().GetDataFolder();
	}

	return AssetFolder;
}

