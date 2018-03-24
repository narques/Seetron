//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXQueryPickTwoActors.h"

class LXQueryLinkChildToFather : public LXQueryPickTwoActors
{
public:

  LXQueryLinkChildToFather(LXQueryManager* pQueryManager);
  virtual ~LXQueryLinkChildToFather(void);

  // Overridden from LXQueryPickTwoActors
  
  virtual void OnSecondNode( LXActor* pFirstNode, LXActor* pSecondNode);

};
