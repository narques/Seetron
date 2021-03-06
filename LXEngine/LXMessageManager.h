//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"

struct LXChannel
{
	const LXObject* Sender;
	const LXString MessageID;
	const LXObject* Receiver;
	function<void()> Callback;
};

class LXCORE_API LXMessageManager : public LXObject
{
public:

	LXMessageManager();
	virtual ~LXMessageManager();
		
	void Run();
	
	const LXChannel* Connect(const LXObject* sender, const LXString& messageID, const LXObject* destination, std::function<void()> callback);
	void Disconnect(const LXChannel* channel);
	void Post(const LXChannel* channel);

private:

	unique_ptr<LXMutex> _mutex;
	list<const LXChannel*> _channels;
	list<const LXChannel*> _sent;
};


