//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXProperty.h"

LXString LXPropertyInfo::_CurrentGroup = L"MISC";

LXProperty* LXProperty::Create(const LXString& name, void* var) const
{
	// Must be implemented in child classes
	CHK(0);
	return nullptr;
}

LXProperty::LXProperty(EPropertyType type):
_Owner(nullptr),
_Type(type)
{
	_PropInfo = new LXPropertyInfo();
}

LXProperty::~LXProperty(void)
{
	delete _PropInfo;
}

void LXProperty::SetObject( LXSmartObject* pObject )
{
	CHK(!_Owner);
	_Owner = pObject;
}

LXSmartObject* LXProperty::GetLXObject( ) const
{
	CHK(_Owner);
	return _Owner;
}

