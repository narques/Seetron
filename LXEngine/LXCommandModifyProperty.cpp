//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXProperty.h"
#include "LXCommandModifyProperty.h"
#include "LXSmartObject.h"
#include "LXMath.h"
#include "LXMemory.h" // --- Must be the last included ---

// Explicit class Instantiation
template class LXCORE_API LXCommandPropertyT<float>;
template class LXCORE_API LXCommandPropertyT<double>;
template class LXCORE_API LXCommandPropertyT<uint>;
template class LXCORE_API LXCommandPropertyT<int>;
template class LXCORE_API LXCommandPropertyT<LXString>;
template class LXCORE_API LXCommandPropertyT<LXFilepath>;
template class LXCORE_API LXCommandPropertyT<bool>;
template class LXCORE_API LXCommandPropertyT<vec2f>;
template class LXCORE_API LXCommandPropertyT<vec3f>;
template class LXCORE_API LXCommandPropertyT<vec4f>;
template class LXCORE_API LXCommandPropertyT<LXColor4f>;
template class LXCORE_API LXCommandPropertyT<LXMatrix>;
template class LXCORE_API LXCommandPropertyT<LXAsset*>;

// Explicit class Instantiation
template class LXCORE_API LXCommandPropertiesT<float>;
template class LXCORE_API LXCommandPropertiesT<double>;
template class LXCORE_API LXCommandPropertiesT<uint>;
template class LXCORE_API LXCommandPropertiesT<int>;
template class LXCORE_API LXCommandPropertiesT<LXString>;
template class LXCORE_API LXCommandPropertiesT<LXFilepath>;
template class LXCORE_API LXCommandPropertiesT<bool>;
template class LXCORE_API LXCommandPropertiesT<vec2f>;
template class LXCORE_API LXCommandPropertiesT<vec3f>;
template class LXCORE_API LXCommandPropertiesT<vec4f>;
template class LXCORE_API LXCommandPropertiesT<LXColor4f>;
template class LXCORE_API LXCommandPropertiesT<LXMatrix>;
template class LXCORE_API LXCommandPropertiesT<LXAsset*>;

LXCommandProperty::LXCommandProperty(void)
{
}

LXCommandProperty::~LXCommandProperty(void)
{
}

template<class T>
bool LXCommandPropertyT<T>::Do( )
{
	m_pProperty->SetValue(m_newValue);
	return true;
}

template<class T>
bool LXCommandPropertyT<T>::Undo( )
{
	m_pPropertyOriginal->SetValue(m_oldValue); 
	return true;
}

template<class T>
const LXProperty* LXCommandPropertyT<T>::GetPropertyOriginal( ) const 
{ 
	return m_pPropertyOriginal; 
}

template<class T>
const LXProperty* LXCommandPropertyT<T>::GetProperty( ) const 
{ 
	return m_pProperty; 
}

template<class T>
bool LXCommandPropertiesT<T>::Do( )
{
	for (auto It = m_listProperties.begin(); It != m_listProperties.end(); It++)
	{
		LXPropertyT<T>* pProperty = dynamic_cast<LXPropertyT<T>*>(*It);
		CHK(pProperty);
		if (pProperty)
		{
			pProperty->SetValue(m_newValue);
		}
	}
	return true;
}

template<class T>
bool LXCommandPropertiesT<T>::Undo( )
{
	CHK(m_oldValues.size() == m_listProperties.size());
	int i=0;
	auto ItValue = m_oldValues.begin();
	for (auto It = m_listProperties.begin(); It != m_listProperties.end(); It++)
	{
		LXPropertyT<T>* pProperty = dynamic_cast<LXPropertyT<T>*>(*It);
		CHK(pProperty);
		if (pProperty)
		{
			pProperty->SetValue(*ItValue);
			ItValue++;
		}
		i++;
	}
	return true;
}

