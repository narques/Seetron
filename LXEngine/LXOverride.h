//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------
//
// LXOverride
// 
// Provides a mechanism to override the SmartObject properties
// It uses the UserProperties to 
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXSmartObject.h"

template <typename T>
class LXCORE_API LXOverride : public virtual LXSmartObject
{

public:

	LXOverride(){}
	virtual ~LXOverride(){}

	virtual LXProperty* GetProperty(const LXPropertyID& PID) override;
	virtual LXProperty* GetProperty(const LXString& propertyName) const override;

public:

	shared_ptr<T> Parent;

private:

	ListProperties _listProperties;
};

template <typename T>
LXProperty* LXOverride<T>::GetProperty(const LXPropertyID& PID)
{
	LXProperty* property = __super::GetProperty(PID);
	return property ? property : Parent->GetProperty(PID);
}

template <typename T>
LXProperty* LXOverride<T>::GetProperty(const LXString& propertyName) const
{
	LXProperty* property = __super::GetProperty(propertyName);
	return property ? property : Parent->GetProperty(propertyName);
}


