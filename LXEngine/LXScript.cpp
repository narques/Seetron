//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXScript.h"
#include "LXEngine.h"
#include "LXLogger.h"
#include "LXPropertyManager.h"
#include "LXActor.h"
#include "LXProject.h"
#include "LXCommandManager.h"

LXScript::LXScript()
{
}

LXScript::~LXScript()
{
}

void LXScript::Init()
{
}

void LXScript::GetProperties(LXSmartObject* p)
{
}

void LXScript::DeleteProperties(LXActor* pGroup)
{
}

void LXScript::Update(double frameTime, LXActor* pOwner)
{
}

LXString LXScript::GetCommandLabel()
{
	return L"";
}

void LXScript::DoCommand()
{
}