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
#include "LXLogger.h"
#include "LXNode.h"

LXGraph::LXGraph()
{
	// --------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Graph");
	// --------------------------------------------------------------------------------------------------------------
	DefineProperty("Nodes", (ListSharedObjects*)&Nodes);
	DefineProperty("Connections", (ListSmartObjects*)&Connections);
}

LXGraph::~LXGraph()
{
	for (LXConnection* connection : Connections)
	{
		delete connection;
	}
}

void LXGraph::Clear()
{
	for (LXConnection* connection : Connections)
		delete connection;
	Nodes.clear();
	Connections.clear();
	_main = nullptr;
	OnGraphChanged.Invoke();
}

void LXGraph::AddNode(std::shared_ptr<LXNode>& node)
{
	if (node->Main && _main)
	{
		LogW(LXGraph, L"Graph already has a main function");
		return;
	}

	Nodes.push_back(node);
	node->Graph = this;
	if (node->Main)
	{
		_main = node;
	}
	OnGraphChanged.Invoke();
}

void LXGraph::RemoveNode(std::shared_ptr<LXNode>& node)
{
	// Delete the Connections.
	for (LXConnector* connector : node->Inputs)
	{
		while(connector->Connections.size())
		{
			LXConnection* connection = *connector->Connections.begin();
			Connections.remove(connection);
			connection->Detach(connector);
			delete connection;
		}
		connector->Connections.clear();
	}

	for (LXConnector* connector : node->Outputs)
	{
		while (connector->Connections.size())
		{
			LXConnection* connection = *connector->Connections.begin();
			Connections.remove(connection);
			connection->Detach(connector);
			delete connection;
		}
		connector->Connections.clear();
	}
	
	// Delete the node
	Nodes.remove(node);
	OnGraphChanged.Invoke();
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
	OnGraphChanged.Invoke();
	return connection;
}

void LXGraph::DeleteConnection(LXConnection* connection)
{
	CHK(std::find(Connections.begin(), Connections.end(), connection) != Connections.end());
	connection->Detach(nullptr);
	Connections.remove(connection);
	delete connection;
	OnGraphChanged.Invoke();
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
	OnGraphChanged.Invoke();
}

const LXNode* LXGraph::GetMain() const
{
	return _main.get();
}

void LXGraph::OnLoaded()
{
	// Remove broken and duplicated connections
	for (std::list<LXConnection*>::iterator it = Connections.begin(); it != Connections.end();)
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
	for (const std::shared_ptr<LXNode>& node : Nodes)
	{
		if (node->Main)
		{
			CHK(_main == nullptr);
			_main = node;
		}
	}
	OnGraphChanged.Invoke();
}

