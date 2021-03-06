//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "lxquerylinkchildtofather.h"
#include "LXActor.h"
#include "lxquerymanager.h"
#include "LXProject.h"
#include "LXMemory.h" // --- Must be the last included ---

LXQueryLinkChildToFather::LXQueryLinkChildToFather(LXQueryManager* pQueryManager):
LXQueryPickTwoActors(pQueryManager)
{
}

LXQueryLinkChildToFather::~LXQueryLinkChildToFather(void)
{
}

void LXQueryLinkChildToFather::OnSecondNode( LXActor* pFirstNode, LXActor* pSecondNode )
{
	CHK(0);
	CHK(pFirstNode);
	CHK(pSecondNode);
	CHK(pFirstNode != pSecondNode);

	LXActor* pChild   = pFirstNode->GetParent();
	LXActor* pParent  = pSecondNode->GetParent();

	/*
	pChild->InvalidateBBox();
	pChild->InvalidateChildBBoxes();
	pChild->InvalidateParentBBoxes();
	pParent->InvalidateBBox();
	pParent->InvalidateChildBoxes();
	pParent->InvalidateParentBBoxes();
	*/

	pChild->GetParent()->RemoveChild(pChild);
	pParent->AddChild(pChild);
  
	//m_pQueryManager->GetDocument()->GetSceneManager().ComputeBBoxes();
}