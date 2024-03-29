//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXSelectionmanager.h"
#include "LXEventManager.h"
#include "LXActorMesh.h"
#include "LXSettings.h"
#include "LXActor.h"
#include "LXEngine.h"
#include "LXKey.h"
#include "LXMSXMLNode.h"
#include "LXMaterial.h"
#include "LXEngine.h"
#include "LXProject.h"
#include "LXScene.h"

LXSelectionManager::LXSelectionManager(const LXProject* pDocument):
m_eSelectionLevel(LX_SELECTION_LEVEL_ENTITY)
{
}

LXSelectionManager::~LXSelectionManager(void)
{
}

bool LXSelectionManager::OnSaveChild(const TSaveContext& saveContext) const
{
	for (auto It = _listSelections.begin(); It!= _listSelections.end(); It++)
		(*It)->Save(saveContext);

	return true;
}

bool LXSelectionManager::OnLoadChild ( const TLoadContext& loadContext )
{
	if (loadContext.node.name() == L"LXSelection")
	{
		LXSelection* pSelection = new LXSelection();
		pSelection->Load(loadContext);
		_listSelections.push_back(pSelection);
	}
	else
		CHK(0);

	return true;
}

void LXSelectionManager::GetObjects( ListObjects& listObject )
{
	for (auto It = _listSelections.begin(); It != _listSelections.end(); It++)
	{
		listObject.push_back(*It);
	}
}

void LXSelectionManager::ClearSelection( )
{
	_setPropServers.clear();
	InvokeEvents();
}

void LXSelectionManager::SetSelection(LXSmartObject* pSmartObject)
{
	_setPropServers.clear();
	_setPropServers.insert(pSmartObject);
	InvokeEvents();
}

void LXSelectionManager::AddToSelection( LXSmartObject* pSmartObject )
{
	_setPropServers.insert( pSmartObject );
	InvokeEvents();
}

void LXSelectionManager::RemoveToSelection(LXSmartObject* pSmartObject)
{
	_setPropServers.erase(pSmartObject);
	InvokeEvents();
}

void LXSelectionManager::Submit( LXSmartObject* pActor, uint64 nFlags)
{
	if (nFlags & MK_CONTROL)
	{
		if (pActor)
		{
			_setPropServers.insert(pActor);  
		}
	}
	else if (nFlags == MK_SHIFT)
	{
		if (pActor)
		{
			_setPropServers.erase(pActor); 
		}
	}
	else
	{
		_setPropServers.clear(); 
		if (pActor)
		{
			_setPropServers.insert(pActor);
		}
	}

	InvokeEvents();

}

void LXSelectionManager::Submit(const SetSmartObjects& setPropServers, uint64 nFlags)
{
	if (nFlags & MK_CONTROL)
	{
		_setPropServers.insert(setPropServers.begin(), setPropServers.end());
	}
	else if (nFlags & MK_SHIFT)
	{
		_setPropServers.erase(setPropServers.begin(), setPropServers.end());
	}
	else
	{
		_setPropServers.clear();
		_setPropServers = setPropServers;
	}

	InvokeEvents();
}

void LXSelectionManager::InvokeEvents()
{
	GetEventManager()->BroadCastEvent(EEventType::SelectionChanged);
	OnSelectionChanged.Invoke();
}

void LXSelectionManager::GetProperties( ListProperties& listProperties) const
{
	for (SetSmartObjects::const_iterator It = _setPropServers.begin(); It!=_setPropServers.end(); It++)
	{
		LXSmartObject* pSmartObject = *It;
		if (pSmartObject)
		{
			const ListProperties& lp = pSmartObject->GetProperties();

			for (ListProperties::const_iterator ItProp = lp.begin(); ItProp!= lp.end(); ItProp++)
			{
				LXProperty* pProperty = *ItProp;
				listProperties.push_back(pProperty);
			}

			// Collects additional properties for UI purpose.
			pSmartObject->GetChildProperties(listProperties);
		}
	}
}

void LXSelectionManager::GetSetActors( SetActors& setActors )
{
	for (SetSmartObjects::const_iterator It = _setPropServers.begin(); It!=_setPropServers.end(); It++)
	{
		if (LXActor* pActor = dynamic_cast<LXActor*>(*It))
			setActors.insert(pActor);
	}
}

void LXSelectionManager::GetUserActors( SetActors& setActors )
{
	for (SetSmartObjects::const_iterator It = _setPropServers.begin(); It!=_setPropServers.end(); It++)
	{
		if (LXActor* pActor = dynamic_cast<LXActor*>(*It))
			if (!pActor->IsSystem())
				setActors.insert(pActor);
	}
}

void LXSelectionManager::GetSetMeshs( SetMeshs& setMeshes )
{
	for (SetSmartObjects::const_iterator It = _setPropServers.begin(); It!=_setPropServers.end(); It++)
	{
		if (LXActorMesh* pMesh = dynamic_cast<LXActorMesh*>(*It))
			setMeshes.insert(pMesh);
	}
}

void LXSelectionManager::GetSetMaterials( SetMaterials& setMaterials )
{
	for (SetSmartObjects::const_iterator It = _setPropServers.begin(); It!=_setPropServers.end(); It++)
	{
		if (LXMaterial* pMaterial = dynamic_cast<LXMaterial*>(*It))
			setMaterials.insert(pMaterial);
	}
}

void LXSelectionManager::GetUserMaterials( SetMaterials& setMaterials )
{
	for (SetSmartObjects::const_iterator It = _setPropServers.begin(); It!=_setPropServers.end(); It++)
	{
		if (LXMaterial* pMaterial = dynamic_cast<LXMaterial*>(*It))
			if (!pMaterial->IsSystem())
				setMaterials.insert(pMaterial);
	}
}

void LXSelectionManager::GetUserKeys(SetKeys& setKeys)
{
	for (SetSmartObjects::const_iterator It = _setPropServers.begin(); It != _setPropServers.end(); It++)
	{
		if (LXKey* pKey = dynamic_cast<LXKey*>(*It))
			if (!pKey->IsSystem())
				setKeys.insert(pKey);
	}
}

bool LXSelectionManager::IsSelected( LXActor* pActor )
{
	if (!pActor)
		return false;

	if (_setPropServers.find(pActor) != _setPropServers.end())
		return true;
	else
		return false;
}

LXBBox LXSelectionManager::GetBBoxWorld()
{
	LXBBox BBox;

	for (LXSmartObject* SmartObject : _setPropServers)
	{
		if (LXActor* Actor = dynamic_cast<LXActor*>(SmartObject))
		{
			BBox.Add(Actor->GetBBoxWorld());
		}
	}
	
	if (!BBox.IsValid())
	{
		BBox = GetEngine().GetProject()->GetScene()->GetBBoxWorld();
	}
	return BBox;

}

void LXSelectionManager::SaveSelection(const LXString& name)
{
	if (_setPropServers.size())
	{
		LXSelection* pSelection = new LXSelection();
		pSelection->SetName(name);
		pSelection->Set(_setPropServers);
		_listSelections.push_back(pSelection);
	}
}

