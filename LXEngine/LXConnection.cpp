//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
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
}

void LXConnection::Detach(LXConnector* owner)
{
	if (owner != Source)
	{
		Source->Connections.remove(this);
	}

	if (owner != Destination)
	{
		Destination->Connections.remove(this);
	}
}

LXConnection::LXConnection(LXConnector* source, LXConnector* destination)
{
	Source = source;
	Destination = destination;
	
	// Input is unique, remove existing
	if (Destination->Connections.size() > 0)
	{
		for(LXConnection* connection : destination->Connections)
		{
			Detach(nullptr);
			delete connection;
		}

		destination->Connections.clear();
	}
	
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
	if (Destination->Connections.size() == 0)
	{
		Source->Connections.push_back(this);
		Destination->Connections.push_back(this);
	}
	else
	{
		// Broken or duplicated
	}
}
