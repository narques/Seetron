//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXAsset.h" 

class LXShader : public LXAsset
{

public:

	LXShader();
	virtual ~LXShader();

	// Overridden from LXAsset
	bool Load() override;
	bool Save() override;
	LXString GetFileExtension() override { return LX_SHADER_EXT; }
	void SaveDefault();
};

