//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXSmartObject.h"
#include "LXFilepath.h"

enum class EResourceOwner
{
	LXResourceOwner_Engine,
	LXResourceOwner_Project,
};

class LXCORE_API LXAsset : public virtual LXSmartObject
{

public:

	enum class EResourceState
	{
		LXResourceState_NotAFile,
		LXResourceState_Unloaded,
		LXResourceState_Loaded
	};
	
	LXAsset();
	virtual ~LXAsset();

	virtual bool Load() = 0;
	virtual bool Reload() { return false; }

	virtual bool Save();

	bool CanBeSaved();
	bool CanBeReloaded();
	
	LXString GetRelativeFilename() const;
	virtual LXString GetFileExtension() = 0;

	virtual ListProperties GetProperties() const override;
		
protected:

	void DefineProperties() ;
	GetSet(LXFilepath, _filepath, Filepath);

	
	
public:

	EResourceState State = EResourceState::LXResourceState_Unloaded;
	EResourceOwner Owner = EResourceOwner::LXResourceOwner_Engine;

};

typedef list<LXAsset*> ListResources;