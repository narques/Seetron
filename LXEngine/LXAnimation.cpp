//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXMatrix.h"
#include "LXAnimation.h"
#include "LXStatistic.h"
#include "LXMSXMLNode.h"

LXAnimation::LXAnimation()
{
	SetName(L"Animation");
	DefineProperties();
}

LXAnimation::~LXAnimation()
{
	for (auto& It : _tracks)
	{
		delete It.second;
	}
}

bool LXAnimation::OnSaveChild(const TSaveContext& saveContext) const
{
	for (auto& it : _tracks)
	{
		LXTrack* track = it.second;
		LXString name(L"LXTrack");
		LXString attribute = track->GetTypeString();
		// We need to set the name "manually",
		// as LXTrack is a template, so the class name 
		// is something like <LXTrackT<vec3f>>
		track->Save(saveContext, &name, &attribute); 
	}
	return true;
}

bool LXAnimation::OnLoadChild(const TLoadContext& loadContext)
{
	if (loadContext.node.name() == L"LXTrack")
	{
		LXString dataType = loadContext.node.attr(L"Type");

		LXTrack* pTrack = NULL;
		
		if (dataType == "float")			
			pTrack = new LXTrackT<float>();
		else if (dataType == "double")
			pTrack = new LXTrackT<double>();
		else if (dataType == "vec3f")
			pTrack = new LXTrackT<vec3f>();
		else
		{
			CHK(0);
			return false;
		}

		LXString name(L"LXTrack");
		pTrack->Load(loadContext, &name);
		LXProperty* property = pTrack->GetProperty();
		CHK(property);
		if (property)
			_tracks[property] = pTrack;

		return true;
	}
	else
	{
		auto str = loadContext.node.name();
		return false;
	}
}

void LXAnimation::Update(double frameTime)
{
	_dPosition += frameTime;

	LX_COUNT(L"Animation.Position", _dPosition);

	for (auto It =_tracks.begin(); It != _tracks.end() ; It++)
	{	
		It->second->Update(_dPosition);
	}

	if (_dPosition >= _dDuration)
	{
		_dPosition = 0;
		_bFinished = true;
	}
 }

void LXAnimation::Update()
{
	for (auto It = _tracks.begin(); It != _tracks.end(); It++)
	{
		It->second->Update(_dPosition);
	}
}

template <class T>
void LXAnimation::AddKey(LXProperty* pProp, const T& newValue, DWORD dwTime)
{
	CHK(pProp->IsAnimatable());
	if (!pProp->IsAnimatable())
		return;

	LXTrackT<T>* pTrack = NULL;

	CHK(dynamic_cast<LXPropertyT<T>*>(pProp));
	LXPropertyT<T>* pPropTyped = (LXPropertyT<T>*)pProp;

	auto it = _tracks.find(pPropTyped);
	if (it != _tracks.end())
	{
		pTrack = dynamic_cast<LXTrackT<T>*>(it->second); // TODO dynamic_cast not necessary
		CHK(pTrack);
	}
	else
	{
		pTrack = new LXTrackT<T>(pPropTyped);
		_tracks[pPropTyped] = pTrack;
	}

	if (!pTrack)
		return;

	if (!pTrack->HasKey(0))
		pTrack->Capture(pPropTyped, 0);

	LXVariant* pVariant = new LXVariantT<T>(newValue);
	pTrack->Capture(pPropTyped, pVariant, dwTime);

	if (dwTime > _dDuration)
		_dDuration = dwTime;
}

void LXAnimation::DefineProperties()
{
	auto propertyDuration = DefineProperty("Duration", LXPropertyID::AnimationDuration, &_dDuration);
	propertyDuration->SetMin(0.);
}

bool LXAnimation::Load()
{
	if (State == EResourceState::LXResourceState_Loaded)
		return true;

	bool Result = false;
	Result = LoadWithMSXML(_filepath);

	if (Result)
	{
		State = EResourceState::LXResourceState_Loaded;
	}

	return false;
}

void LXAnimation::Remove(LXKey* pKey)
{
	CHK(pKey);
	if (!pKey)
		return;

	LXTrack* pTrack = pKey->GetTrack();
	CHK(pTrack);

	if (pTrack)
		pTrack->Remove(pKey);
}

void LXAnimation::Add(LXKey* pKey)
{
	CHK(pKey);
	if (!pKey)
		return;

	LXTrack* pTrack = pKey->GetTrack();
	CHK(pTrack);

	if (pTrack)
		pTrack->Add(pKey);
}

// Explicit method Instantiation
template LXENGINE_API void LXAnimation::AddKey(LXProperty* pProp, const bool& newValue, DWORD dwTime);
template LXENGINE_API void LXAnimation::AddKey(LXProperty* pProp, const float& newValue, DWORD dwTime);
template LXENGINE_API void LXAnimation::AddKey(LXProperty* pProp, const double& newValue, DWORD dwTime);
template LXENGINE_API void LXAnimation::AddKey(LXProperty* pProp, const uint& newValue, DWORD dwTime);
template LXENGINE_API void LXAnimation::AddKey(LXProperty* pProp, const int& newValue, DWORD dwTime);
template LXENGINE_API void LXAnimation::AddKey(LXProperty* pProp, const LXString& newValue, DWORD dwTime);
template LXENGINE_API void LXAnimation::AddKey(LXProperty* pProp, const LXFilepath& newValue, DWORD dwTime);
template LXENGINE_API void LXAnimation::AddKey(LXProperty* pProp, const vec2f& newValue, DWORD dwTime);
template LXENGINE_API void LXAnimation::AddKey(LXProperty* pProp, const vec3f& newValue, DWORD dwTime);
template LXENGINE_API void LXAnimation::AddKey(LXProperty* pProp, const vec4f& newValue, DWORD dwTime);
template LXENGINE_API void LXAnimation::AddKey(LXProperty* pProp, const LXColor4f& newValue, DWORD dwTime);
template LXENGINE_API void LXAnimation::AddKey(LXProperty* pProp, const LXMatrix& newValue, DWORD dwTime);
