//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXManager.h"
#include "LXCommandModifyProperty.h"
#include "LXCommandDeleteActor.h"

class LXQuery;
class LXViewState;
class LXMesh;

class LXCORE_API LXCommandManager : public LXObject
{

public:

	LXCommandManager();
	virtual ~LXCommandManager(void);

	
	//
	// Misc
	//
	
	bool	IsEmpty				( ) { return _listCommands.size()?false:true; }
	void	UndoLastCommand		( );
	const ListCommands& GetCommands( ) const { return _listCommands; }

	//
	// Commands
	//

	bool    SaveFile			( LXSmartObject* smartObject);
	void	SaveSelection		( const LXString& name );
			
	void	ClearSelection		( );
	void	SetSelection		( LXSmartObject* smartObject );
	void	AddToSelection		( LXSmartObject* smartObject);
	void	AddToSelection2		( LXSmartObject* smartObject, uint64 flags = 0);
	void	AddToSelection2		( const SetSmartObjects& setPropServers, uint64 flags = 0);
	void	RemoveToSelection	( LXSmartObject* smartObject );
	void	SelectAll			( );
	void	Select				( );
	void	ActivateView		( LXViewState* viewState );
	
	//
	// Query
	//

	void	PushQuery			( LXQuery* query);

	//
	// Property Commands
	//

	template <class T>
	void	ChangeProperties    ( const ListProperties& listProperties, const T& newValue , bool supportUndo = true);
	template <class T>		
	void	ChangeProperty		( LXProperty* property, const T& newValue, bool supportUndo = true)  {   ChangeProperty(dynamic_cast<LXPropertyT<T>*>(property), newValue, supportUndo);  }
	template <class T>
	void	ChangeProperty		( LXPropertyT<T>* property, const T& newValue, bool supportUndo = true);

	template <class T>
	void	PreviewChangeProperties(const ListProperties& listProperties, const T& newValue) { ChangeProperties(listProperties, newValue, false); }
	template <class T>
	void	PreviewChangeProperty(LXProperty* property, const T& newValue) { PreviewChangeProperty(dynamic_cast<LXPropertyT<T>*>(property), newValue); }
	template <class T>
	void	PreviewChangeProperty(LXPropertyT<T>* property, const T& newValue) { ChangeProperty(property, newValue, false); }

	//
	// "Undoable" Commands
	//

	void	HideActors			( const SetActors& setActors );
	void	ShowActors			( const SetActors& setActors );
	void	DeleteActors		( const SetActors& setActors );	
	void	DeleteMaterials		( const SetMaterials& setMaterials );
	void	DeleteKeys			( const SetKeys& setKeys );
	bool	SetParent			( LXActor* parent, LXActor* child);
	bool	SetParent			( LXMesh* parent, LXMesh* child );


private:
	
	void	PushCommand			( LXCommand* command );

private:

	ListCommands				_listCommands;
	SetActors					_setActorsToCopy;
};

