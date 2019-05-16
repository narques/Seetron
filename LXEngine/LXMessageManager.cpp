//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXMessageManager.h"
#include "LXMutex.h"
#include "LXMemory.h" // --- Must be the last included ---

LXMessageManager::LXMessageManager()
{
	_mutex = make_unique<LXMutex>();
}

LXMessageManager::~LXMessageManager()
{

}

void LXMessageManager::Connect(const LXObject* sender, const LXString& messageID, const LXObject* receiver, std::function<void()> callback)
{
	LXMessagePipes& messagePipes = _connections[sender];
	LXMessageReceiver& messageReceiver = messagePipes.messageReceivers[messageID];
	CHK(messageReceiver.Receiver == nullptr)
	messageReceiver.Receiver = receiver;
	messageReceiver.Callback = callback;
}

void LXMessageManager::Post(const LXObject* sender, const LXString& messageID)
{
	_mutex->Lock();
	_messages.push_back(pair<const LXObject*, LXString>(sender, messageID));
	_mutex->Unlock();
}

void LXMessageManager::Run()
{
	_mutex->Lock();
	for (auto& it : _messages)
	{
		const LXObject* sender = it.first;
		const LXString& messageID = it.second;

		LXMessagePipes& messagePipes = _connections[sender];
		LXMessageReceiver& messageReceiver = messagePipes.messageReceivers[messageID];
		
		messageReceiver.Callback();
	}

	_messages.clear();
	_mutex->Unlock();
}
