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

LXConnection::LXConnection()
{
	DefineProperties();
}

LXConnection::LXConnection(LXConnector* source, LXConnector* destination)
{
	LogD(LXConnection, L"Raw ptr to shared_ptr :(");
	Source = shared_ptr<LXConnector>(source);
	Destination = shared_ptr<LXConnector>(destination);

	// Input is unique, remove existing
	if (Destination->Connections.size() > 0)
	{
		for(LXConnection* connection : destination->Connections)
		{
			CHK(0); // TODO : message to delete the corresponding connection views
			connection->Source->Connections.remove(connection);
			//canvas.Children.Remove(connection._connectionView._path);
		}

		destination->Connections.clear();
	}
	
	Source->Connections.push_back(this);
	Destination->Connections.push_back(this);

	DefineProperties();
}

void LXConnection::DefineProperties()
{
	DefineProperty(L"Source", (shared_ptr<LXSmartObject>*)&Source);
	DefineProperty(L"Destination", (shared_ptr<LXSmartObject>*)&Destination);
}

LXConnection::~LXConnection()
{
}
