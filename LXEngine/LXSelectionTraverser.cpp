//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXSelectionTraverser.h"
#include "LXMemory.h" // --- Must be the last included --- 

LXSelectionTraverser::LXSelectionTraverser(void):
m_pSetActors(NULL)
{
}

LXSelectionTraverser::~LXSelectionTraverser(void)
{
}

void LXSelectionTraverser::OnMesh( LXActorMesh* pMesh )
{
	CHK(m_pSetActors);
	if (m_pSetActors)
		m_pSetActors->insert((LXActor*)pMesh);

	LXTraverser::OnMesh(pMesh);
}

LXSelectionTraverser2::LXSelectionTraverser2(void):
m_pSetGroups(NULL)
{
}

LXSelectionTraverser2::~LXSelectionTraverser2(void)
{
}

void LXSelectionTraverser2::OnActor( LXActor* pGroup )
{
	CHK(m_pSetGroups);
	if (m_pSetGroups)
		m_pSetGroups->insert(pGroup);

	LXTraverser::OnActor(pGroup);
}

LXSelectionTraverser3::LXSelectionTraverser3(void):
m_pSetMeshs(NULL)
{
}

LXSelectionTraverser3::~LXSelectionTraverser3(void)
{
}

void LXSelectionTraverser3::OnMesh( LXActorMesh* pMesh )
{
	CHK(m_pSetMeshs);
	if (m_pSetMeshs)
		m_pSetMeshs->insert(pMesh);

	LXTraverser::OnMesh(pMesh);
}

LXSelectionTraverser4::LXSelectionTraverser4(void):
	m_pSetMeshs(NULL)
{
}

LXSelectionTraverser4::~LXSelectionTraverser4(void)
{
}

void LXSelectionTraverser4::OnActor( LXActor* pGroup )
{
	if (pGroup->IsVisible())
		LXTraverser::OnActor(pGroup);
}

void LXSelectionTraverser4::OnMesh( LXActorMesh* pMesh )
{
	CHK(m_pSetMeshs);
	if (m_pSetMeshs && pMesh->IsVisible())
		m_pSetMeshs->insert(pMesh);
}


LXSelectionTraverserCB::LXSelectionTraverserCB(void)
{

}

LXSelectionTraverserCB::~LXSelectionTraverserCB(void)
{

}

void LXSelectionTraverserCB::OnActor( LXActor* pGroup )
{
	if (m_onGroupCallBack)
		m_onGroupCallBack(pGroup);

	LXTraverser::OnActor(pGroup);
}

void LXSelectionTraverserCB::OnMesh( LXActorMesh* pMesh )
{
	if (m_onMeshCallBack)
		m_onMeshCallBack(pMesh);
}



