//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXConnection.h"
#include "LXConnector.h"
#include "LXMemory.h" // --- Must be the last included ---

LXConnection::LXConnection()
{
	DefineProperties();
}

LXConnection::~LXConnection()
{
	if (Source.get())
		Source->Connections.remove(this);
	if (Destination.get())
		Destination->Connections.remove(this);
}

void LXConnection::Detach(LXConnector* owner)
{
	if (owner != Source)
	{
		Source->Connections.remove(this);
		Source = nullptr;
	}

	if (owner != Destination)
	{
		Destination->Connections.remove(this);
		Destination = nullptr;
	}
}

LXConnection::LXConnection(LXConnector* source, LXConnector* destination)
{
	Source = source;
	Destination = destination;
	
	Source->Connections.push_back(this);
	Destination->Connections.push_back(this);

	DefineProperties();
}

void LXConnection::DefineProperties()
{
	DefineProperty(L"Source", (LXReference<LXSmartObject>*)&Source);
	DefineProperty(L"Destination", (LXReference<LXSmartObject>*)&Destination);
}

void LXConnection::OnLoaded()
{
	if (Source.get() && 
		Destination.get() && 
		Destination->Connections.size() == 0)
	{
		Source->Connections.push_back(this);
		Destination->Connections.push_back(this);
	}
	else
	{
		// Broken or duplicated 
		CHK(0);
		LogE(LXConnection, L"Loaded a broken connection.");
	}
}
