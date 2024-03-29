//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXObject.h"

#if LX_TRACE_OBJECTS

#include "LXMutex.h"
#include "LXLogger.h"

namespace
{
	SetObjects& GetObjects()
	{
		static SetObjects setObjects;
		return setObjects;
	}

	LXMutex& GetMutex()
	{
		static LXMutex Mutex;
		return Mutex;
	}

	void AddInstance(LXObject* Object)
	{
		GetMutex().Lock();
		GetObjects().insert(Object);
		GetMutex().Unlock();
	}

	void RemoveInstance(LXObject* pObject)
	{
		GetMutex().Lock();
		std::set<LXObject*>::iterator It = GetObjects().find(pObject);
		if (It != GetObjects().end())
			GetObjects().erase(pObject);
		else
			CHK(0);
		GetMutex().Unlock();
	}
}

#endif // LX_TRACE_OBJECTS

LXObject::LXObject(void)
{
#if LX_TRACE_OBJECTS
	AddInstance(this);
#endif
}

LXObject::~LXObject(void)
{
#if LX_TRACE_OBJECTS
	RemoveInstance(this);
#endif
}

LXString LXObject::GetObjectName() const
{
	wchar_t dest[256];
	size_t numOfCharConverted;
	mbstowcs_s(&numOfCharConverted, dest, typeid(*this).name(), 256);
	LXString strName(dest);
	strName.TrimLeft(L"class ");
	return strName;
}

void LXObject::TraceAll()
{
#if LX_TRACE_OBJECTS
	MapObjects mapObjects;
	MapObjects::iterator It2;
	std::set<LXObject*>::iterator It;

	SetObjects& setObjects = GetObjects();

	for(It=setObjects.begin(); It!=setObjects.end(); It++)
	{
		LXObject* pObject = *It;
		if (pObject)
		{
			It2 = mapObjects.find(pObject->GetObjectName());
			if (It2 != mapObjects.end())
				It2->second++;
			else
				mapObjects[(*It)->GetObjectName()] = 1;
		}
	}
	for(It2=mapObjects.begin(); It2!=mapObjects.end(); It2++)
	{
		LogD(LXObject, L"%s %i", It2->first.GetBuffer(), It2->second);
	}
	mapObjects.clear();
#endif	
}
