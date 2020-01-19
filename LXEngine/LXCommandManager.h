//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXManager.h"
#include "LXCommandModifyProperty.h"
#include "LXCommandDeleteActor.h"
#include "LXObserver.h"
#include "LXObservable.h"

class LXQuery;
class LXViewState;
class LXMesh;

class LXCORE_API LXCommandManager : public LXObservable
{

public:

	LXCommandManager();
	virtual ~LXCommandManager(void);

	//
	// Overridden from LXObservable
	//

	void	AddObserver			( LXObserverCommandManager* pObserver ) { LXObservable::AddObserver(pObserver); }
	void	RemoveObserver		( LXObserverCommandManager* pObserver ) { LXObservable::RemoveObserver(pObserver); }

	//
	// Callbacks
	//

	void OnCommand(std::function<void(LXObject*)> Func)
	{
		ListFuncOnCommand.push_back(Func);
	}
	void InvokeOnCommand(LXObject* var)
	{
		for (auto Func : ListFuncOnCommand)
		{
			Func(var);
		}
	}

	list < std::function<void(LXObject*)>> ListFuncOnCommand;
	
	//
	// Misc
	//
	
	bool	IsEmpty				( ) { return _listCommands.size()?false:true; }
	void	UndoLastCommand		( );
	const ListCommands& GetCommands( ) const { return _listCommands; }

	//
	// Commands
	//

	bool    SaveFile			( LXSmartObject* pServer );
	void	SaveSelection		( const LXString& name );
	void	ChangeHighlight		( LXActor* pActor );
		
	void	ClearSelection		( );
	void	SetSelection		( LXSmartObject* pSmartObject );
	void	AddToSelection		( LXSmartObject* pSmartObject );
	void	AddToSelection2		( LXSmartObject* pActor, uint64 nFlags = 0);
	void	AddToSelection2		( const SetSmartObjects& setPropServers, uint64 nFlags = 0);
	void	RemoveToSelection	( LXSmartObject* pSmartObject );
	void	SelectAll			( );
	void	Select				( );
	void	ActivateView		( LXViewState* pView );
	
	//
	// Query
	//

	void	PushQuery			( LXQuery* );
		
	//
	// "Undoable" Commands
	//

	template <class T>
	void	ChangeProperties    ( const ListProperties& listProperties, const T& newValue );
	template <class T>		
	void	ChangeProperty		( LXProperty* pProp, const T& newValue)  {   ChangeProperty(dynamic_cast<LXPropertyT<T>*>(pProp), newValue);  }
	template <class T>
	void	ChangeProperty		( LXPropertyT<T>* pProp, const T& newValue );
	template <class T>
	void	ChangeProperty		( LXPropertyT<T>* pProp ); // Current value is already good (Updated by PreviewChangeProperty for ex. )

	void	HideActors			( const SetActors& setActors );
	void	ShowActors			( const SetActors& setActors );
	void	DeleteActors		( const SetActors& setActors );	
	void	DeleteMaterials		( const SetMaterials& setMaterials );
	void	DeleteKeys			( const SetKeys& setKeys );
	bool	SetParent			( LXActor* Parent, LXActor* Child);
	bool	SetParent			( LXMesh* Parent, LXMesh* Child );
	
	// 
	//	invoke the listeners when the Scene changes (Actor added or deleted)
	//

	void	SceneChanged		();
	
	//
	// Misc Command Methods
	//

	template <class T>
	void	PreviewChangeProperties( const ListProperties& listProperties, const T& newValue );
	template <class T>		
	void	PreviewChangeProperty( LXProperty* pProp, const T& newValue)  {   PreviewChangeProperty(dynamic_cast<LXPropertyT<T>*>(pProp), newValue);  }
	template <class T>
	void	PreviewChangeProperty( LXPropertyT<T>* pProp, const T& newValue );

private:
	
	void	PushCommand			( LXCommand* pCommand );

private:

	ListCommands				_listCommands;
	SetActors					_setActorsToCopy;
	LXProperty*					_pPreviewProperty = nullptr;
	LXCommand*					_pPreviewCmd = nullptr;
};




