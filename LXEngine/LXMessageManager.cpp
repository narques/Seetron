//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXMessageManager.h"
#include "LXMutex.h"

LXMessageManager::LXMessageManager()
{
	_mutex = std::make_unique<LXMutex>();
}

LXMessageManager::~LXMessageManager()
{

}

const LXChannel* LXMessageManager::Connect(const LXObject* sender, const LXString& messageID, const LXObject* receiver, std::function<void()> callback)
{
	LXChannel* channel = new LXChannel{ sender, messageID, receiver, callback };
	_channels.push_back(channel);
	return channel;
}

void LXMessageManager::Disconnect(const LXChannel* channel)
{
	_channels.remove(channel);
	delete channel;
}

void LXMessageManager::Post(const LXChannel* channel)
{
	_mutex->Lock();
	_sent.push_back(channel);
	_mutex->Unlock();
}

void LXMessageManager::Run()
{
	_mutex->Lock();
	for (const LXChannel* channel : _sent)
	{
		const LXObject* sender = channel->Sender;
		const LXString& messageID = channel->MessageID;
		const LXObject* receiver = channel->Receiver;
		channel->Callback();
	}
	_sent.clear();
	_mutex->Unlock();
}

