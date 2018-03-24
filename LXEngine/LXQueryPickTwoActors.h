//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once
#include "lxquery.h"

class LXQueryPickTwoActors : public LXQuery
{

public:
  
  LXQueryPickTwoActors(LXQueryManager* pQueryManager);
  virtual ~LXQueryPickTwoActors(void);

  virtual void OnSecondNode(LXActor* pFirstNode, LXActor* pSecondNode ) = 0;

  // Overridden from LXQuery
  virtual void OnPickNode(LXActor* pNode);

private:

  LXActor* m_pFirstNode;

};
