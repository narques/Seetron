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
#include "LXGraph.h"
#include "LXNode.h"
#include "LXMemory.h" // --- Must be the last included ---

LXGraph::LXGraph()
{
	// --------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Graph");
	// --------------------------------------------------------------------------------------------------------------
	DefineProperty("Nodes", (ListSmartObjects*)&Nodes);
	DefineProperty("Connections", (ListSmartObjects*)&Connections);
}

LXGraph::~LXGraph()
{
}

void LXGraph::Clear()
{
	Nodes.clear();
	Connections.clear();
	_main = nullptr;
}

void LXGraph::AddNode(LXNode* node)
{
	Nodes.push_back(node);
	node->Graph = this;
	if (node->Main)
	{
		CHK(_main == nullptr);
		_main = node;
	}
}

void LXGraph::DeleteNode(LXNode* node)
{
	// Delete the Connections.
	for (LXConnector* connector : node->Inputs)
	{
		for (LXConnection* connection : connector->Connections)
		{
			Connections.remove(connection);
			connection->Detach(connector);
			delete connection;
		}
		connector->Connections.clear();
	}

	for (LXConnector* connector : node->Outputs)
	{
		for (LXConnection* connection : connector->Connections)
		{
			Connections.remove(connection);
			connection->Detach(connector);
			delete connection;
		}
		connector->Connections.clear();
	}
	
	// Delete the node
	Nodes.remove(node);
	delete node;
}

LXConnection* LXGraph::CreateConnection(LXConnector* source, LXConnector* destination)
{
	// Delete previous connection to the destination.
	if (destination->Connections.size() > 0)
	{
		while (destination->Connections.size() > 0)
		{
			LXConnection* oldConnection = destination->Connections.back();
			DeleteConnection(oldConnection);
		}
	}
	
	LXConnection* connection = new LXConnection(source, destination);
	Connections.push_back(connection);

	return connection;
}

void LXGraph::DeleteConnection(LXConnection* connection)
{
	CHK(std::find(Connections.begin(), Connections.end(), connection) != Connections.end());
	Connections.remove(connection);
	connection->Detach(nullptr);
	delete connection;
	OnConnectionDeleted.Invoke();
}

void LXGraph::DeleteConnector(LXConnector* connector)
{
	while (connector->Connections.size())
	{
		LXConnection* connection = connector->Connections.back();
		DeleteConnection(connection);
		connector->Connections.pop_back();
	}

	delete connector;
	OnConnectorDeleted.Invoke();
}

const LXNode* LXGraph::GetMain() const
{
	return _main;
}

void LXGraph::OnLoaded()
{
	// Remove broken and duplicated connections
	for (list<LXConnection*>::iterator it = Connections.begin(); it != Connections.end();)
	{
		if ((*it)->Source == nullptr || (*it)->Destination == nullptr)
		{
			delete *it;
			it = Connections.erase(it);
		}
		else if ((*it)->Destination->Connections.size() > 1)
		{
			delete *it;
			it = Connections.erase(it);
		}
		else
		{
			it++;
		}
	}

	// Search for the main node
	for (LXNode* node : Nodes)
	{
		if (node->Main)
		{
			CHK(_main == nullptr);
			_main = node;
		}
	}
}

