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
class LXENGINE_API LXOverride : public virtual LXSmartObject
{

public:

	LXOverride(){}
	virtual ~LXOverride(){}

	virtual LXProperty* GetProperty(const LXPropertyID& PID) override;
	virtual LXProperty* GetProperty(const LXString& propertyName) const override;

	// [Create] and set a generic property override
	template<typename P>
	bool SetProperty(const LXString& propertyName, const P& propertyValue)
	{
		// Ensure the property exists on parent
		const LXSmartObject* material = Parent.get();
		LXProperty* property = material->GetProperty(propertyName);

		if (const LXPropertyT<P>* propertyTyped = dynamic_cast<const LXPropertyT<P>*>(property))
		{
			LXPropertyT<P>* propertyOverride = dynamic_cast<LXPropertyT<P>*>(LXSmartObject::GetProperty(propertyName));
 
			if (!propertyOverride)
			{
				propertyOverride = CreateUserProperty<P>(propertyName, propertyTyped->GetValue());
			}
 
			propertyOverride->SetValue(propertyValue, false);
			return true;
		}
 
		return false;
	}

public:

	std::shared_ptr<T> Parent;

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


