//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXTraverser.h"
#include "LXActor.h"
#include "LXActor.h"
#include "LXActorMesh.h"

//
// Get all Actors
//

class LXENGINE_API LXSelectionTraverser : public LXTraverser
{

public:

	LXSelectionTraverser(void);
	virtual ~LXSelectionTraverser(void);

	virtual void OnActor(LXActor* actor);
	void SetSetActors(SetActors* pSetActors) { m_pSetActors = pSetActors; }

private:

	SetActors*		m_pSetActors;

};

//
// Get all SmartObject
//

class LXENGINE_API LXSelectionTraverser2 : public LXTraverser
{

public:

	LXSelectionTraverser2(void);
	virtual ~LXSelectionTraverser2(void);

	virtual void	OnActor		 ( LXActor* pGroup ) override;

	
	void			SetSetGroups ( SetSmartObjects* pSetGroups) { m_pSetGroups = pSetGroups; }

private:

	SetSmartObjects*		m_pSetGroups;

};

//
// Get all Meshes
//

class LXENGINE_API LXSelectionTraverser3 : public LXTraverser
{

public:

	LXSelectionTraverser3(void);
	virtual ~LXSelectionTraverser3(void);

	virtual void	OnMesh		 ( LXActorMesh* pMesh ) override;

	
	void			SetSetMeshs ( SetMeshs* pSetMeshs) { m_pSetMeshs = pSetMeshs; }

private:

	SetMeshs*		m_pSetMeshs;

};

//
// Get visible Meshes
//

class LXENGINE_API LXSelectionTraverser4 : public LXTraverser
{

public:

	LXSelectionTraverser4(void);
	virtual ~LXSelectionTraverser4(void);

	virtual void	OnActor		 ( LXActor* pGroup ) override;
	virtual void	OnMesh		 ( LXActorMesh* pMesh ) override;


	void			SetSetMeshs	 ( SetActors* pSetMeshs) { m_pSetMeshs = pSetMeshs; }

private:

	SetActors*		m_pSetMeshs;

};


//
// Traverser with Lambda callback
//

class LXENGINE_API LXSelectionTraverserCB : public LXTraverser
{

public:

	LXSelectionTraverserCB(void);
	virtual ~LXSelectionTraverserCB(void);

	void OnGroupCallBack(std::function<void(LXActor*)> func) { m_onGroupCallBack = func; }
	void OnMeshCallBack(std::function<void(LXActorMesh*)> func) { m_onMeshCallBack = func; }

	virtual void	OnActor		 ( LXActor* pGroup ) override;
	virtual void	OnMesh		 ( LXActorMesh* pMesh ) override;
	
protected:

	std::function<void(LXActor*)>		m_onGroupCallBack;  
	std::function<void(LXActorMesh*)>	m_onMeshCallBack;  

};
