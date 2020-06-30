//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

class LXMaterial;
class LXMaterialBase;

class LXMaterialUtility
{
public:
	static const LXMaterial* GetMaterialFromBase(const LXMaterialBase* materialBase);
};

