//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXCommandManager.h"
#include "LXCore.h"
#include "LXProject.h"
#include "LXPropertyManager.h"
#include "LXSelectionManager.h"
#include "LXQueryManager.h"
#include "LXViewportManager.h"
#include "LXAnchor.h"
#include "LXSettings.h"
#include "LXKey.h"
#include "LXAnimationManager.h"
#include "LXCommandModifyHierarchy.h"
#include "LXMemory.h" // --- Must be the last included ---

LXCommandManager::LXCommandManager()
{
}

LXCommandManager::~LXCommandManager(void)
{
	for (ListCommands::iterator It = _listCommands.begin(); It!=_listCommands.end(); It++)
	{
		LXCommand* pCommand = *It;
		delete pCommand;
	}
	_listCommands.clear();

	CHK(!_pPreviewCmd);
}

//
// Commands
//

bool LXCommandManager::SaveFile( LXSmartObject* pSmartObject )
{
	CHK(pSmartObject);
	if (!pSmartObject)
		return false;

	bool bRet = false;

	LXDocumentBase* pSB = dynamic_cast<LXDocumentBase*>(pSmartObject);
	CHK(pSB);
	if (pSB)
		 bRet = pSB->SaveFile();
	
	for (ListObservers::iterator It = m_listObservers.begin(); It!=m_listObservers.end(); It++)
	{
		LXObserverCommandManager* pObserver = static_cast<LXObserverCommandManager*>(*It);
		pObserver->OnFileSaved(pSmartObject);
	}

	return bRet;
}

void LXCommandManager::ClearSelection( )
{
	if (GetCore().GetProject())
		GetCore().GetProject()->GetSelectionManager().ClearSelection();
}

void LXCommandManager::AddToSelection( LXSmartObject* pSmartObject )
{
	if (GetCore().GetProject())
		GetCore().GetProject()->GetSelectionManager().AddToSelection(pSmartObject);
}

void LXCommandManager::RemoveToSelection( LXSmartObject* pSmartObject)
{
	if (GetCore().GetProject())
		GetCore().GetProject()->GetSelectionManager().RemoveToSelection(pSmartObject);
}

void LXCommandManager::AddToSelection2( LXSmartObject* pActor, uint64 nFlags)
{
	if (!GetCore().GetProject())
		return;

	GetCore().GetProject()->GetSelectionManager().Submit(pActor, nFlags);
	
	const SetSmartObjects& setSmartObjects = GetCore().GetProject()->GetSelectionManager().GetSelection();

	if (m_bNotifyListerners)
	{
		for (ListObservers::iterator It = m_listObservers.begin(); It!=m_listObservers.end(); It++)
			static_cast<LXObserverCommandManager*>(*It)->OnChangeSelection(setSmartObjects);
	}
}

void LXCommandManager::AddToSelection2( const SetSmartObjects& setPropServers, uint64 nFlags)
{
	if(!GetCore().GetProject())
		return;

	GetCore().GetProject()->GetSelectionManager().Submit(setPropServers, nFlags);
	
	const SetSmartObjects& setSmartObjects = GetCore().GetProject()->GetSelectionManager().GetSelection();
	
	if (m_bNotifyListerners)
	{
		for (ListObservers::iterator It = m_listObservers.begin(); It!=m_listObservers.end(); It++)
			static_cast<LXObserverCommandManager*>(*It)->OnChangeSelection(setSmartObjects);
	}
}

void LXCommandManager::SelectAll()
{
	if (!GetCore().GetProject())
		return;

	SetSmartObjects setSmartObjects;
	GetCore().GetProject()->GetGroups(setSmartObjects);
		
	AddToSelection2(setSmartObjects, 0);
}

void LXCommandManager::Select() 
{
	if (!GetCore().GetProject())
		return;
	
	SetSmartObjects setSmartObjects;

	PairPropetyIDVariant item;
	item.first = LXPropertyID::MESH_LAYER;
	item.second = new LXVariantT<int>(1);

	GetCore().GetProject()->GetGroups(setSmartObjects, item);

	AddToSelection2(setSmartObjects, 0);
}

//
// Do/Undo Commands
//

void LXCommandManager::PushCommand(LXCommand* pCommand)
{
	CHK (pCommand);
	if (!pCommand)
		return;

	_listCommands.push_back(pCommand);
	
	if (_listCommands.size() > GetCore().GetUndoStackSize())
	{
		delete *_listCommands.begin();
		_listCommands.pop_front();
	}

	InvokeOnCommand(pCommand);
}

void LXCommandManager::UndoLastCommand()
{
	if (!_listCommands.size())
		return;

	LXCommand* pCmd = _listCommands.back();
	pCmd->Undo();
	_listCommands.pop_back();

	if (LXCommandProperty* pCmdProp = dynamic_cast<LXCommandProperty*>(pCmd))
	{
		const LXProperty* pProperty = pCmdProp->GetPropertyOriginal();
		{
			for (ListObservers::iterator It = m_listObservers.begin(); It!=m_listObservers.end(); It++)
				static_cast<LXObserverCommandManager*>(*It)->OnChangeProperty(pProperty);
		}
	}
	else if (LXCommandProperties* pCmdProp = dynamic_cast<LXCommandProperties*>(pCmd))
	{
		for(auto ItProp = pCmdProp->GetProperties().begin(); ItProp != pCmdProp->GetProperties().end(); ItProp++)
		{
			for (ListObservers::iterator It = m_listObservers.begin(); It!=m_listObservers.end(); It++)
				static_cast<LXObserverCommandManager*>(*It)->OnChangeProperty(*ItProp);
		}
	}
	else if (LXCommandDeleteActor* pCmdDeleteActors = dynamic_cast<LXCommandDeleteActor*>(pCmd))
	{
		if (m_bNotifyListerners)
		{
			const SetActors& setActors = pCmdDeleteActors->GetActors();
			for (ListObservers::iterator It = m_listObservers.begin(); It != m_listObservers.end(); It++)
				static_cast<LXObserverCommandManager*>(*It)->OnAddActors(setActors);
		}
		pCmdDeleteActors->ClearActors();
	}
	else if (LXCommandDeleteKey* pCmdDeleteKeys = dynamic_cast<LXCommandDeleteKey*>(pCmd))
	{
		if (m_bNotifyListerners)
		{
			const SetKeys& setKeys = pCmdDeleteKeys->GetKeys();
			for (ListObservers::iterator It = m_listObservers.begin(); It != m_listObservers.end(); It++)
				static_cast<LXObserverCommandManager*>(*It)->OnAddKeys(setKeys);
		}
		pCmdDeleteKeys->ClearKeys();
	}
	else
		CHK(0); 
	

	InvokeOnCommand(pCmd);

	delete pCmd;
}

void LXCommandManager::HideActors(const SetActors& setActors)
{
	if (!setActors.size())
		return;

	ListProperties listProperties;
	LXPropertyManager::GetProperties(setActors, LXPropertyID::VISIBILITY, listProperties);

	ChangeProperties(listProperties, false);

	if (GetCore().GetProject())
	{
		GetCore().GetProject()->GetSelectionManager().ClearSelection();
	}
}

void LXCommandManager::ShowActors(const SetActors& setActors)
{
	if (!setActors.size())
		return;

	ListProperties listProperties;
	LXPropertyManager::GetProperties(setActors, LXPropertyID::VISIBILITY, listProperties);

	ChangeProperties(listProperties, true);
}


void LXCommandManager::DeleteActors(const SetActors& setActors)
{
	if (!setActors.size())
		return;

	LXCommandDeleteActor* pCmd = new LXCommandDeleteActor(setActors);
	pCmd->Do();
	pCmd->SetDescription(LXString::Number((int)setActors.size()) +  L" actor(s) deleted");

	// Remove objects from selection
	for(LXSmartObject* pObject:setActors)
		GetCore().GetProject()->GetSelectionManager().RemoveToSelection(pObject);

	if (m_bNotifyListerners)
	{
		for (ListObservers::iterator It = m_listObservers.begin(); It != m_listObservers.end(); It++)
		{
			static_cast<LXObserverCommandManager*>(*It)->OnDeleteActors(pCmd->GetActors());
		}
	}

	PushCommand(pCmd);
}

void LXCommandManager::DeleteMaterials( const SetMaterials& setMaterials )
{
	if (!setMaterials.size())
		return;
		
	LXCommandDeleteMaterial* pCmd = new LXCommandDeleteMaterial(setMaterials);
	pCmd->Do();
	pCmd->SetDescription(LXString::Number((int)setMaterials.size()) +  L" material(s) deleted");

	// Remove objects from selection
	for (LXSmartObject* pObject : setMaterials)
		GetCore().GetProject()->GetSelectionManager().RemoveToSelection(pObject);
		
	if (m_bNotifyListerners)
	{
		for (ListObservers::iterator It = m_listObservers.begin(); It!=m_listObservers.end(); It++)
			static_cast<LXObserverCommandManager*>(*It)->OnDeleteMaterials(pCmd->GetMaterials());
	}

	PushCommand(pCmd);
}

void LXCommandManager::DeleteKeys(const SetKeys& setKeys)
{
	if (!setKeys.size())
		return;

	LXCommandDeleteKey* pCmd = new LXCommandDeleteKey(setKeys);
	pCmd->Do();
	pCmd->SetDescription(LXString::Number((int)setKeys.size()) + L" key(s) deleted");

	// Remove objects from selection
	for (LXSmartObject* pObject : setKeys)
		GetCore().GetProject()->GetSelectionManager().RemoveToSelection(pObject);
	
	if (m_bNotifyListerners)
	{
		for (ListObservers::iterator It = m_listObservers.begin(); It != m_listObservers.end(); It++)
			static_cast<LXObserverCommandManager*>(*It)->OnDeleteKeys(pCmd->GetKeys());
	}

	PushCommand(pCmd);
}

bool LXCommandManager::SetParent(LXActor* Parent, LXActor* Child)
{
	LXCommandModifyActorHierarchy* Cmd = new LXCommandModifyActorHierarchy(Parent, Child);
	auto ret = Cmd->Do();
	if (ret)
		PushCommand(Cmd);
	else
		delete Cmd;
	return ret;
}

bool LXCommandManager::SetParent(LXMesh* Parent, LXMesh* Child)
{
	LXCommandModifyMeshHierarchy* Cmd = new LXCommandModifyMeshHierarchy(Parent, Child);
	auto ret = Cmd->Do();
	if (ret)
		PushCommand(Cmd);
	else
		delete Cmd;
	return ret;
}

void LXCommandManager::SceneChanged()
{
	if (m_bNotifyListerners)
	{
		for (ListObservers::iterator It = m_listObservers.begin(); It != m_listObservers.end(); It++)
			static_cast<LXObserverCommandManager*>(*It)->OnSceneChanged();
	}
}

void LXCommandManager::ChangeHighlight(LXActor* pActor)
{
	if (m_bNotifyListerners)
	{
		for (ListObservers::iterator It = m_listObservers.begin(); It!=m_listObservers.end(); It++)
			static_cast<LXObserverCommandManager*>(*It)->OnChangeHighlight(pActor);
	}
}

// Explicit method Instantiation

template LXCORE_API void LXCommandManager::ChangeProperties(const ListProperties& listProperties, const float&);
template LXCORE_API void LXCommandManager::ChangeProperties(const ListProperties& listProperties, const double&);
template LXCORE_API void LXCommandManager::ChangeProperties(const ListProperties& listProperties, const uint&);
template LXCORE_API void LXCommandManager::ChangeProperties(const ListProperties& listProperties, const int&);
template LXCORE_API void LXCommandManager::ChangeProperties(const ListProperties& listProperties, const LXString&);
template LXCORE_API void LXCommandManager::ChangeProperties(const ListProperties& listProperties, const LXFilepath&);
template LXCORE_API void LXCommandManager::ChangeProperties(const ListProperties& listProperties, const bool&);
template LXCORE_API void LXCommandManager::ChangeProperties(const ListProperties& listProperties, const vec3f&);
template LXCORE_API void LXCommandManager::ChangeProperties(const ListProperties& listProperties, const vec4f&);
template LXCORE_API void LXCommandManager::ChangeProperties(const ListProperties& listProperties, const LXColor4f&);
template LXCORE_API void LXCommandManager::ChangeProperties(const ListProperties& listProperties, const LXMaterialNodePtr&);
template LXCORE_API void LXCommandManager::ChangeProperties(const ListProperties& listProperties, const LXMatrix&);
template LXCORE_API void LXCommandManager::ChangeProperties(const ListProperties& listProperties, const LXAssetPtr&);

template LXCORE_API void LXCommandManager::PreviewChangeProperties(const ListProperties& listProperties, const float&);
template LXCORE_API void LXCommandManager::PreviewChangeProperties(const ListProperties& listProperties, const double&);
template LXCORE_API void LXCommandManager::PreviewChangeProperties(const ListProperties& listProperties, const int&);
template LXCORE_API void LXCommandManager::PreviewChangeProperties(const ListProperties& listProperties, const uint&);
template LXCORE_API void LXCommandManager::PreviewChangeProperties(const ListProperties& listProperties, const LXColor4f&);

template LXCORE_API void LXCommandManager::ChangeProperty(LXPropertyT<float>*, const float&);
template LXCORE_API void LXCommandManager::ChangeProperty(LXPropertyT<double>*, const double&);
template LXCORE_API void LXCommandManager::ChangeProperty(LXPropertyT<uint>*, const uint&);
template LXCORE_API void LXCommandManager::ChangeProperty(LXPropertyT<int>*, const int&);
template LXCORE_API void LXCommandManager::ChangeProperty(LXPropertyT<LXString>*, const LXString&);
template LXCORE_API void LXCommandManager::ChangeProperty(LXPropertyT<LXFilepath>*, const LXFilepath&);
template LXCORE_API void LXCommandManager::ChangeProperty(LXPropertyT<bool>*, const bool&);
template LXCORE_API void LXCommandManager::ChangeProperty(LXPropertyT<vec3f>*, const vec3f&);
template LXCORE_API void LXCommandManager::ChangeProperty(LXPropertyT<vec4f>*, const vec4f&);
template LXCORE_API void LXCommandManager::ChangeProperty(LXPropertyT<LXColor4f>*, const LXColor4f&);
template LXCORE_API void LXCommandManager::ChangeProperty(LXPropertyT<LXMatrix>*, const LXMatrix&);
template LXCORE_API void LXCommandManager::ChangeProperty(LXPropertyT<LXAssetPtr>*, const LXAssetPtr&);

template LXCORE_API void LXCommandManager::ChangeProperty(LXPropertyT<LXMatrix>*);
template LXCORE_API void LXCommandManager::ChangeProperty(LXPropertyT<vec3f>*);

template LXCORE_API void LXCommandManager::PreviewChangeProperty(LXPropertyT<float>*, const float&);
template LXCORE_API void LXCommandManager::PreviewChangeProperty(LXPropertyT<double>*, const double&);
template LXCORE_API void LXCommandManager::PreviewChangeProperty(LXPropertyT<uint>*, const uint&);
template LXCORE_API void LXCommandManager::PreviewChangeProperty(LXPropertyT<int>*, const int&);
template LXCORE_API void LXCommandManager::PreviewChangeProperty(LXPropertyT<LXString>*, const LXString&);
template LXCORE_API void LXCommandManager::PreviewChangeProperty(LXPropertyT<LXFilepath>*, const LXFilepath&);
template LXCORE_API void LXCommandManager::PreviewChangeProperty(LXPropertyT<bool>*, const bool&);
template LXCORE_API void LXCommandManager::PreviewChangeProperty(LXPropertyT<LXColor4f>*, const LXColor4f&);
template LXCORE_API void LXCommandManager::PreviewChangeProperty(LXPropertyT<LXMatrix>*, const LXMatrix&);
template LXCORE_API void LXCommandManager::PreviewChangeProperty(LXPropertyT<vec3f>*, const vec3f&);

template <class T>
void	LXCommandManager::ChangeProperties( const ListProperties& listProperties, const T& newValue )
{
	CHK(listProperties.size() > 0);
	if (listProperties.size()==0)
		return;

	LXCommand* pCmd = NULL;
	
	if (_pPreviewCmd)
	{
		// Use "preview" command as the final command
		if (dynamic_cast<LXCommandPropertiesT<T>*>(_pPreviewCmd))
		{
			pCmd = _pPreviewCmd;
			_pPreviewCmd = NULL;
		}
		else
		{
			CHK(0); 
			delete _pPreviewCmd;
			_pPreviewCmd = NULL;
		}
		
	}

	if (!pCmd)
		pCmd = new LXCommandPropertiesT<T>(listProperties, newValue);

	pCmd->Do();
	pCmd->SetDescription(listProperties.size() + L" properties changed");
	PushCommand(pCmd);

	// Animation
	SetKeys setKeys;
	if (GetCore().GetAutoKey())
	{
		LXAnimation* pAnimation = GetCore().GetProject()->GetAnimationManager().GetAnimation();
		double time = pAnimation->GetPosition();
		for (LXProperty* property : listProperties)
		{
			if (property->IsAnimatable())
				pAnimation->AddKey(property, newValue, (DWORD)time);
		}
	}
	
	// Callback
	for (ListObservers::iterator It = m_listObservers.begin(); It!=m_listObservers.end(); It++)
	{
		for (auto ItProp = listProperties.begin(); ItProp != listProperties.end(); ItProp++)
		{
			static_cast<LXObserverCommandManager*>(*It)->OnChangeProperty(*ItProp);
			if (GetCore().GetAutoKey())
				static_cast<LXObserverCommandManager*>(*It)->OnAddKeys(setKeys);
		}
	}
}

template <class T>
void LXCommandManager::PreviewChangeProperties( const ListProperties& listProperties, const T& newValue )
{
	CHK(listProperties.size() > 0);
	if (listProperties.size()==0)
		return;
	
	if(!_pPreviewCmd)
		_pPreviewCmd = new LXCommandPropertiesT<T>(listProperties, newValue); 
	else
	{
		LXCommandPropertiesT<T>* p = dynamic_cast<LXCommandPropertiesT<T>*>(_pPreviewCmd);
		CHK(p); 
		if (!p)
		{
			CHK(0);
			delete _pPreviewCmd;
			_pPreviewCmd = NULL;
			return; 
		}
		p->SetNewValue(newValue);
	}

	_pPreviewCmd->Do();
	
	for (ListObservers::iterator It = m_listObservers.begin(); It!=m_listObservers.end(); It++)
	{
		for(auto ItProp = listProperties.begin(); ItProp != listProperties.end(); ItProp++)
			static_cast<LXObserverCommandManager*>(*It)->OnChangeProperty(*ItProp);
	}
}

template <class T>
void	LXCommandManager::ChangeProperty( LXPropertyT<T>* pProp, const T& newValue )
{
	CHK(pProp);
	if (!pProp)
		return;
	
	LXCommand* pCmd = NULL;
	if (_pPreviewCmd)
	{
		if (!dynamic_cast<LXCommandPropertyT<T>*>(_pPreviewCmd))
		{
			CHK(0);
			delete _pPreviewCmd;
			_pPreviewCmd = NULL;
		}
		else
			pCmd = _pPreviewCmd;

		_pPreviewCmd = NULL;
	}
	
	if (!pCmd)
		pCmd = new LXCommandPropertyT<T>(pProp, newValue);
	

	pCmd->Do();
	pCmd->SetDescription(L"Property " +  pProp->GetLXObject()->GetName() + "." + pProp->GetLabel() + L" changed");
	PushCommand(pCmd);

	for (ListObservers::iterator It = m_listObservers.begin(); It!=m_listObservers.end(); It++)
	{
		static_cast<LXObserverCommandManager*>(*It)->OnChangeProperty(pProp);
	}
}

template <class T>
void	LXCommandManager::ChangeProperty( LXPropertyT<T>* pProp )
{
	ChangeProperty(pProp, pProp->GetValue());
}

template <class T>
void LXCommandManager::PreviewChangeProperty( LXPropertyT<T>* pProp, const T& newValue )
{
	CHK(pProp);
	if (!pProp)
		return;

	if (pProp->GetValue() == newValue) 
		return; 

	if(_pPreviewProperty != pProp || !_pPreviewCmd)
	{
		if (_pPreviewCmd)
			delete _pPreviewCmd;
		_pPreviewCmd = new LXCommandPropertyT<T>(pProp, newValue); 
		_pPreviewProperty = pProp;
	}
	else
	{
		LXCommandPropertyT<T>* p = dynamic_cast<LXCommandPropertyT<T>*>(_pPreviewCmd);
		if (!p)
		{
			CHK(0);
			delete _pPreviewCmd;
			return; 
		}
		p->SetNewValue(newValue);
	}

	_pPreviewCmd->Do();
				
	for (ListObservers::iterator It = m_listObservers.begin(); It!=m_listObservers.end(); It++)
	{
		static_cast<LXObserverCommandManager*>(*It)->OnChangeProperty(pProp);
	}

#if LX_ANCHOR

	// Anchor location propagation
	if (pProp->GetXMLName() == LXPropertyID::TRANSFORMATION)
	if (LXAnchor* pAnchor = dynamic_cast<LXAnchor*>(pProp->GetLXObject())) // TODO : GetCID() & 
	{
		if (LXActor* pActor = pAnchor->GetOwner())
			pActor->OnAnchorMove(pAnchor);
	}

#endif
}

void LXCommandManager::PushQuery( LXQuery* pQuery)
{
	CHK(pQuery);
	if (!pQuery)
		return;

	LXProject* pDocument = GetCore().GetProject();
	CHK(pDocument);
	if (!pDocument)
		return;

	pDocument->GetQueryManager().PushQuery(pQuery);
}

void LXCommandManager::ActivateView( LXViewState* pView )
{
	LXProject* pDocument = GetCore().GetProject();
	CHK(pDocument);
	if (!pDocument)
		return;
	
	pDocument->SetActiveView(pView);
}

void LXCommandManager::SaveSelection( const LXString& name )
{
	if (GetCore().GetProject())
		GetCore().GetProject()->GetSelectionManager().SaveSelection(name);
}
