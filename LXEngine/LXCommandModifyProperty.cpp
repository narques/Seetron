//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXProperty.h"
#include "LXCommandModifyProperty.h"
#include "LXSmartObject.h"
#include "LXMath.h"

// Explicit class Instantiation
template class LXENGINE_API LXCommandPropertyT<float>;
template class LXENGINE_API LXCommandPropertyT<double>;
template class LXENGINE_API LXCommandPropertyT<uint>;
template class LXENGINE_API LXCommandPropertyT<int>;
template class LXENGINE_API LXCommandPropertyT<LXString>;
template class LXENGINE_API LXCommandPropertyT<LXFilepath>;
template class LXENGINE_API LXCommandPropertyT<bool>;
template class LXENGINE_API LXCommandPropertyT<vec2f>;
template class LXENGINE_API LXCommandPropertyT<vec3f>;
template class LXENGINE_API LXCommandPropertyT<vec4f>;
template class LXENGINE_API LXCommandPropertyT<LXColor4f>;
template class LXENGINE_API LXCommandPropertyT<LXMatrix>;

// Explicit class Instantiation
template class LXENGINE_API LXCommandPropertiesT<float>;
template class LXENGINE_API LXCommandPropertiesT<double>;
template class LXENGINE_API LXCommandPropertiesT<uint>;
template class LXENGINE_API LXCommandPropertiesT<int>;
template class LXENGINE_API LXCommandPropertiesT<LXString>;
template class LXENGINE_API LXCommandPropertiesT<LXFilepath>;
template class LXENGINE_API LXCommandPropertiesT<bool>;
template class LXENGINE_API LXCommandPropertiesT<vec2f>;
template class LXENGINE_API LXCommandPropertiesT<vec3f>;
template class LXENGINE_API LXCommandPropertiesT<vec4f>;
template class LXENGINE_API LXCommandPropertiesT<LXColor4f>;
template class LXENGINE_API LXCommandPropertiesT<LXMatrix>;

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
	m_pProperty->SetValue(m_oldValue);
	return true;
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

