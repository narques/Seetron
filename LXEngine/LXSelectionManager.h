//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXActor.h"
#include "LXActorMesh.h"
#include "LXManager.h"
#include "LXBBox.h"
#include "LXSelection.h"

enum LXSelectionLevel
{
	LX_SELECTION_LEVEL_POINT,
	LX_SELECTION_LEVEL_TRIANGLE,
	LX_SELECTION_LEVEL_DRAWABLE,
	LX_SELECTION_LEVEL_GROUP,		// GROUP, MESH ...
	LX_SELECTION_LEVEL_ENTITY
};

class LXCORE_API LXSelectionManager : public LXManager2
{

	friend class LXCommandManager;
	
public:

	LXSelectionManager(const LXProject* pDocument);
	virtual ~LXSelectionManager(void);

	// Overridden from LXSmartObject
	virtual bool	OnSaveChild						( const TSaveContext& saveContext  ) const override;
	virtual bool	OnLoadChild						( const TLoadContext& loadContext ) override;
	
	// Overridden from LXManager2
	virtual void				GetObjects			( ListObjects& listObject ) override;

	const SetSmartObjects&		GetSelection		( ) const { return _setPropServers; }
	void						GetProperties		( ListProperties& listProperties ) const;
	void						GetSetActors		( SetActors& setActors );
	void						GetUserActors		( SetActors& setActors );
	void						GetSetMeshs			( SetMeshs& setMeshs );
	void						GetSetMaterials		( SetMaterials& setMaterials );
	void						GetUserMaterials	( SetMaterials& setMaterials );
	void						GetUserKeys			( SetKeys& setKeys );
	bool						IsSelected			( LXActor* pActor );
	
	// Returns the selection world bounds;
	LXBBox						GetBBoxWorld		( );
			
private:
	
	void						ClearSelection		( );
	void						AddToSelection		( LXSmartObject* pSmartObject );
	void						RemoveToSelection	( LXSmartObject* pSmartObject );
	void						Submit				( LXSmartObject* pActor, uint64 nFlags );
	void						Submit				( const SetSmartObjects& setActors, uint64 nFlags );
	void						SaveSelection		( const LXString& name );
	
private:

	ListSelections				_listSelections;
	SetSmartObjects				_setPropServers;
	GetSet(LXSelectionLevel, m_eSelectionLevel, SelectionLevel);
	
};

