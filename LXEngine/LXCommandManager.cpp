//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXCommandManager.h"
#include "LXEngine.h"
#include "LXProject.h"
#include "LXSelectionManager.h"
#include "LXQueryManager.h"
#include "LXAnimationManager.h"
#include "LXCommandModifyHierarchy.h"
#include "LXLogger.h"

LXCommandManager::LXCommandManager()
{
}

LXCommandManager::~LXCommandManager(void)
{
	for (LXCommand* command : _listCommands)
	{
		delete command;
	}
}

//
// Commands
//

bool LXCommandManager::SaveFile( LXSmartObject* smartObject )
{
	CHK(smartObject);
	if (!smartObject)
		return false;

	bool bRet = false;

	LXDocumentBase* documentBase = dynamic_cast<LXDocumentBase*>(smartObject);
	CHK(documentBase);
	if (documentBase)
		 bRet = documentBase->SaveFile();
	
	return bRet;
}

void LXCommandManager::ClearSelection( )
{
	if (GetEngine().GetProject())
		GetEngine().GetProject()->GetSelectionManager().ClearSelection();
}

void LXCommandManager::SetSelection(LXSmartObject* smartObject)
{
	if (GetEngine().GetProject())
		GetEngine().GetProject()->GetSelectionManager().SetSelection(smartObject);
}

void LXCommandManager::AddToSelection( LXSmartObject* smartObject )
{
	if (GetEngine().GetProject())
		GetEngine().GetProject()->GetSelectionManager().AddToSelection(smartObject);
}

void LXCommandManager::RemoveToSelection( LXSmartObject* smartObject)
{
	if (GetEngine().GetProject())
		GetEngine().GetProject()->GetSelectionManager().RemoveToSelection(smartObject);
}

void LXCommandManager::AddToSelection2( LXSmartObject* pActor, uint64 nFlags)
{
	if (GetEngine().GetProject())
		GetEngine().GetProject()->GetSelectionManager().Submit(pActor, nFlags);
}

void LXCommandManager::AddToSelection2( const SetSmartObjects& setPropServers, uint64 nFlags)
{
	if(GetEngine().GetProject())
		GetEngine().GetProject()->GetSelectionManager().Submit(setPropServers, nFlags);
}

void LXCommandManager::SelectAll()
{
	if (!GetEngine().GetProject())
		return;

	SetSmartObjects setSmartObjects;
	GetEngine().GetProject()->GetGroups(setSmartObjects);
		
	AddToSelection2(setSmartObjects, 0);
}

void LXCommandManager::Select() 
{
	if (!GetEngine().GetProject())
		return;
	
	SetSmartObjects setSmartObjects;

	PairPropetyIDVariant item;
	item.first = LXPropertyID::MESH_LAYER;
	item.second = new LXVariantT<int>(1);

	GetEngine().GetProject()->GetGroups(setSmartObjects, item);

	AddToSelection2(setSmartObjects, 0);
}

//
// Do/Undo Commands
//

void LXCommandManager::PushCommand(LXCommand* command)
{
	CHK (command);
	if (!command)
		return;

	LogD(CommandManager, L"Command pushed : %s", command->GetDescription().GetBuffer());

	_listCommands.push_back(command);
	
	if (_listCommands.size() > GetEngine().GetUndoStackSize())
	{
		delete *_listCommands.begin();
		_listCommands.pop_front();
	}
}

void LXCommandManager::UndoLastCommand()
{
	if (!_listCommands.size())
		return;

	LXCommand* command = _listCommands.back();
	command->Undo();
	_listCommands.pop_back();

	if (LXCommandProperty* pCmdProp = dynamic_cast<LXCommandProperty*>(command))
	{

	}
	else if (LXCommandProperties* pCmdProp = dynamic_cast<LXCommandProperties*>(command))
	{

	}
	else if (LXCommandDeleteActor* pCmdDeleteActors = dynamic_cast<LXCommandDeleteActor*>(command))
	{
		pCmdDeleteActors->ClearActors();
	}
	else if (LXCommandDeleteKey* pCmdDeleteKeys = dynamic_cast<LXCommandDeleteKey*>(command))
	{
		pCmdDeleteKeys->ClearKeys();
	}
	else
		CHK(0); 
	
	delete command;
}

void LXCommandManager::HideActors(const SetActors& setActors)
{
	if (!setActors.size())
		return;

	ListProperties listProperties;
	LXPropertyManager::GetProperties(setActors, LXPropertyID::VISIBILITY, listProperties);

	ChangeProperties(listProperties, false);

	if (GetEngine().GetProject())
	{
		GetEngine().GetProject()->GetSelectionManager().ClearSelection();
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

	LXCommandDeleteActor* command = new LXCommandDeleteActor(setActors);
	command->Do();
	command->SetDescription(LXString::Number((int)setActors.size()) +  L" actor(s) deleted");

	// Remove objects from selection
	for(LXSmartObject* pObject:setActors)
		GetEngine().GetProject()->GetSelectionManager().RemoveToSelection(pObject);

	PushCommand(command);
}

void LXCommandManager::DeleteMaterials( const SetMaterials& setMaterials )
{
	if (!setMaterials.size())
		return;
		
	LXCommandDeleteMaterial* command = new LXCommandDeleteMaterial(setMaterials);
	command->Do();
	command->SetDescription(LXString::Number((int)setMaterials.size()) +  L" material(s) deleted");

	// Remove objects from selection
	for (LXSmartObject* smartObject : setMaterials)
		GetEngine().GetProject()->GetSelectionManager().RemoveToSelection(smartObject);
		
	PushCommand(command);
}

void LXCommandManager::DeleteKeys(const SetKeys& setKeys)
{
	if (!setKeys.size())
		return;

	LXCommandDeleteKey* command = new LXCommandDeleteKey(setKeys);
	command->Do();
	command->SetDescription(LXString::Number((int)setKeys.size()) + L" key(s) deleted");

	// Remove objects from selection
	for (LXSmartObject* smartObject : setKeys)
		GetEngine().GetProject()->GetSelectionManager().RemoveToSelection(smartObject);
	
	PushCommand(command);
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

bool LXCommandManager::SetParent(LXMesh* parent, LXMesh* child)
{
	LXCommandModifyMeshHierarchy* command = new LXCommandModifyMeshHierarchy(parent, child);
	auto ret = command->Do();
	if (ret)
		PushCommand(command);
	else
		delete command;
	return ret;
}

// Explicit method Instantiation

template LXENGINE_API void LXCommandManager::ChangeProperties(const ListProperties& listProperties, const float&, bool supportUndo);
template LXENGINE_API void LXCommandManager::ChangeProperties(const ListProperties& listProperties, const double&, bool supportUndo);
template LXENGINE_API void LXCommandManager::ChangeProperties(const ListProperties& listProperties, const uint&, bool supportUndo);
template LXENGINE_API void LXCommandManager::ChangeProperties(const ListProperties& listProperties, const int&, bool supportUndo);
template LXENGINE_API void LXCommandManager::ChangeProperties(const ListProperties& listProperties, const LXString&, bool supportUndo);
template LXENGINE_API void LXCommandManager::ChangeProperties(const ListProperties& listProperties, const LXFilepath&, bool supportUndo);
template LXENGINE_API void LXCommandManager::ChangeProperties(const ListProperties& listProperties, const bool&, bool supportUndo);
template LXENGINE_API void LXCommandManager::ChangeProperties(const ListProperties& listProperties, const vec2f&, bool supportUndo);
template LXENGINE_API void LXCommandManager::ChangeProperties(const ListProperties& listProperties, const vec3f&, bool supportUndo);
template LXENGINE_API void LXCommandManager::ChangeProperties(const ListProperties& listProperties, const vec4f&, bool supportUndo);
template LXENGINE_API void LXCommandManager::ChangeProperties(const ListProperties& listProperties, const LXColor4f&, bool supportUndo);
template LXENGINE_API void LXCommandManager::ChangeProperties(const ListProperties& listProperties, const LXMatrix&, bool supportUndo);

template LXENGINE_API void LXCommandManager::ChangeProperty(LXPropertyT<float>*, const float&, bool supportUndo);
template LXENGINE_API void LXCommandManager::ChangeProperty(LXPropertyT<double>*, const double&, bool supportUndo);
template LXENGINE_API void LXCommandManager::ChangeProperty(LXPropertyT<uint>*, const uint&, bool supportUndo);
template LXENGINE_API void LXCommandManager::ChangeProperty(LXPropertyT<int>*, const int&, bool supportUndo);
template LXENGINE_API void LXCommandManager::ChangeProperty(LXPropertyT<LXString>*, const LXString&, bool supportUndo);
template LXENGINE_API void LXCommandManager::ChangeProperty(LXPropertyT<LXFilepath>*, const LXFilepath&, bool supportUndo);
template LXENGINE_API void LXCommandManager::ChangeProperty(LXPropertyT<bool>*, const bool&, bool supportUndo);
template LXENGINE_API void LXCommandManager::ChangeProperty(LXPropertyT<vec2f>*, const vec2f&, bool supportUndo);
template LXENGINE_API void LXCommandManager::ChangeProperty(LXPropertyT<vec3f>*, const vec3f&, bool supportUndo);
template LXENGINE_API void LXCommandManager::ChangeProperty(LXPropertyT<vec4f>*, const vec4f&, bool supportUndo);
template LXENGINE_API void LXCommandManager::ChangeProperty(LXPropertyT<LXColor4f>*, const LXColor4f&, bool supportUndo);
template LXENGINE_API void LXCommandManager::ChangeProperty(LXPropertyT<LXMatrix>*, const LXMatrix&, bool supportUndo);

template <class T>
void	LXCommandManager::ChangeProperties( const ListProperties& listProperties, const T& newValue, bool supportUndo)
{
	CHK(listProperties.size() > 0);
	if (listProperties.size()==0)
		return;

	LXCommand* command = new LXCommandPropertiesT<T>(listProperties, newValue);

	command->Do();

	if (supportUndo)
	{
		LXString message = L"Properties changed:";
		for (const LXProperty* property : listProperties)
		{
			message += L" " + property->GetName();
		}
		command->SetDescription(message);
		PushCommand(command);
	}
	else
	{
		delete command;
	}

	// Animation
	SetKeys setKeys;
	if (GetEngine().GetAutoKey())
	{
		LXAnimation* pAnimation = GetEngine().GetProject()->GetAnimationManager().GetAnimation();
		double time = pAnimation->GetPosition();
		for (LXProperty* property : listProperties)
		{
			if (property->IsAnimatable())
				pAnimation->AddKey(property, newValue, (DWORD)time);
		}
	}
}

template <class T>
void	LXCommandManager::ChangeProperty( LXPropertyT<T>* property, const T& newValue, bool supportUndo )
{
	CHK(property);
	if (!property)
		return;
	
	LXCommand* command = new LXCommandPropertyT<T>(property, newValue);
	command->Do();

	if (supportUndo)
	{
		command->SetDescription(L"Property changed: " + property->GetLXObject()->GetName() + "." + property->GetLabel());
		PushCommand(command);
	}
	else
	{
		delete command;
	}
}

void LXCommandManager::PushQuery( LXQuery* query )
{
	CHK(query);
	if (!query)
		return;

	LXProject* project = GetEngine().GetProject();
	CHK(project);
	if (!project)
		return;

	project->GetQueryManager().PushQuery(query);
}

void LXCommandManager::ActivateView( LXViewState* viewState )
{
	LXProject* project = GetEngine().GetProject();
	CHK(project);
	if (!project)
		return;
	
	project->SetActiveView(viewState);
}

void LXCommandManager::SaveSelection( const LXString& name )
{
	if (GetEngine().GetProject())
		GetEngine().GetProject()->GetSelectionManager().SaveSelection(name);
}
