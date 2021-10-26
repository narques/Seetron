//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXQueryPickTwoActors.h"

LXQueryPickTwoActors::LXQueryPickTwoActors(LXQueryManager* pQueryManager):
LXQuery(pQueryManager),
m_pFirstNode(NULL)
{
}

LXQueryPickTwoActors::~LXQueryPickTwoActors(void)
{
}

void LXQueryPickTwoActors::OnPickNode ( LXActor* pNode ) 
{
  if (pNode)
  {
    if (m_pFirstNode && (m_pFirstNode != pNode))
    {
      OnSecondNode(m_pFirstNode, pNode);
      m_pFirstNode = NULL;
    }
    else
      m_pFirstNode = pNode;
  }
}
