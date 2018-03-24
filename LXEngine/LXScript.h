//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXAsset.h"
#include "LXSmartObject.h"

class LXActor;

class LXCORE_API LXScript : public LXAsset
{

public:

	enum LXScripType
	{
		Unknown,
		SceneObjectUpdate,  // Entry point is Update()
		ApplicationCommand	// Entry point is Command()
	};

	LXScript();
	virtual ~LXScript();
	
	void Init();
	void Update(double frameTime, LXActor* pOwner);
	LXString GetCommandLabel();
	void DoCommand();
	void GetProperties(LXSmartObject* p);
	void DeleteProperties(LXActor* pGroup);

protected:

	GetSetDef(LXScripType, _type, Type, Unknown);

private:

	vector<LXPropertyID> _arrayProperties;
	set<LXPropertyID> _setProperties;
	
};
