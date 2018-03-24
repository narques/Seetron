//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

class LXRenderCommand
{

public:

	LXRenderCommand();
	virtual ~LXRenderCommand();

	virtual void Execute(class LXRenderCommandList*) = 0;
};

