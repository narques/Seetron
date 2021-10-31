//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXCommand.h"
#include "LXPropertyTemplate.h"
#include "LXMatrix.h"

//--------------------------------------------------------------------------

class LXENGINE_API LXCommandProperty : public LXCommand
{

public:

	LXCommandProperty(void);
	virtual ~LXCommandProperty(void);

	virtual const LXProperty*	GetProperty( ) const = 0;
};

//--------------------------------------------------------------------------

template<class T> 
class LXENGINE_API LXCommandPropertyT : public LXCommandProperty
{

public:

	LXCommandPropertyT(LXPropertyT<T>* pProperty, const T& newValue)
	{
		if (!pProperty)
			  return;
			m_newValue = newValue;
			m_oldValue = pProperty->GetValue();
			m_pProperty = pProperty;
	}
	
	virtual ~LXCommandPropertyT(void)
	{
	}
	
	// Overridden from LXCommand
	virtual	bool Do( )override;
	virtual bool Undo( )override;
		
	// Overridden from LXCommandProperty
	virtual const LXProperty*	GetProperty( ) const;

	void SetNewValue(const T& newValue) { m_newValue = newValue; }

protected:

	LXPropertyT<T>*		m_pProperty;			 // Targeted property
	T					m_newValue;				 // New value
	T                   m_oldValue;				 // Old value
};

//--------------------------------------------------------------------------

class LXENGINE_API LXCommandProperties : public LXCommand
{

public:

	LXCommandProperties(void){ }
	virtual ~LXCommandProperties(void) { }

	const ListProperties& GetProperties( ) const { return m_listProperties; }

protected:

	ListProperties  m_listProperties;
};

//--------------------------------------------------------------------------

template<class T> 
class LXENGINE_API LXCommandPropertiesT : public LXCommandProperties
{
public:

	LXCommandPropertiesT(const ListProperties& listProperties, const T& newValue)
	{
		m_listProperties = listProperties;
		m_newValue = newValue;
		// Save the previous values
		for (auto It = listProperties.begin(); It != listProperties.end(); It++)
		{
			LXPropertyT<T>* pProperty = dynamic_cast<LXPropertyT<T>*>(*It);
			CHK(pProperty);
			if (pProperty)
				m_oldValues.push_back(pProperty->GetValue());
		}
	}

	virtual ~LXCommandPropertiesT() { }

	// Overridden from LXCommand
	virtual	bool Do      ( )override;
	virtual bool Undo	 ( )override;
	
	void SetNewValue(const T& newValue) { m_newValue = newValue; }
		
protected:
	
	T               m_newValue;
	std::list<T>			m_oldValues;

};
