//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"

struct LXMessageReceiver
{
	const LXObject* Receiver;
	std::function<void()> Callback;
};

struct LXMessagePipes
{
	map<LXString, LXMessageReceiver> messageReceivers;
};

class LXCORE_API LXMessageManager : public LXObject
{
public:

	LXMessageManager();
	virtual ~LXMessageManager();
		
	void Run();
	void Connect(const LXObject* sender, const LXString& messageID, const LXObject* destination, std::function<void()> callback);
	void Post(const LXObject* sender, const LXString& messageID);

private:

	unique_ptr<LXMutex> _mutex;
	map<const LXObject*, LXMessagePipes> _connections;
	list<pair<const LXObject*, LXString>> _messages;

};

