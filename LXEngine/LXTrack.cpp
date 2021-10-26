//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXTrack.h"
#include "LXProperty.h"
#include "LXInterpolator.h"
#include "LXProject.h"
#include "LXLogger.h"
#include "LXMatrix.h"
#include "LXMSXMLNode.h"

LXTrack::LXTrack()
{
	SetName(L"Track");
	DefineProperties();
}

LXTrack::~LXTrack()
{
	for (auto &it : _keys)
		delete it.second;
}

bool LXTrack::OnSaveChild(const TSaveContext& saveContext) const
{
	for (auto &it : _keys)
	{
		int time = it.first;
		LXKey* key = it.second;
		key->Save(saveContext);
	}
	return true;
}

bool LXTrack::OnLoadChild(const TLoadContext& loadContext)
{
	if (loadContext.node.name() == L"LXKey")
	{
		LXProperty* property = GetProperty();
		CHK(property);
		LXVariant* pVariant = property?property->CreateVariant():NULL;
		LXKey* pKey = new LXKey(this, pVariant);
		pKey->Load(loadContext);
		int time = pKey->GetTime();
		_keys[time] = pKey;
		return true;
	}
	else
	{
		auto str = loadContext.node.name();
		return false;
	}
}

void LXTrack::Update(double time)
{
	if (_keys.size() == 0)
		return;

	if (_eInterpolation == Interpolation_Linear)
		UpdateLinear(time);
	else if ((_keys.size() <= 2) || (_eInterpolation == Interpolation_Cosine))
		UpdateCosine(time);
	else if (_eInterpolation == Interpolation_Cubic)
		UpdateCubic(time);
	else
		CHK(0);
}

void LXTrack::UpdateLinear(double time)
{
	MapKeys::iterator It0 = _keys.upper_bound((int)time);
	MapKeys::iterator It1 = _keys.upper_bound((int)time);

	if (It0 != _keys.begin())
		It0--;

	double t;

	if (It1 == _keys.end())
	{
		It1 = It0;
		t = 1.;
	}
	else
	{
		double stepDuration = It1->first - It0->first;
		double localTime = time - It0->first;
		if (stepDuration)
			t = localTime / stepDuration;
		else
			t = 1.;

		if (t < 0.)
			t = 0.;
		if (t > 1.)
			t = 1.;
	}

	InterpolateLinear(*It0->second, *It1->second, t);
}

void LXTrack::UpdateCosine(double time)
{
	MapKeys::iterator It0 = _keys.upper_bound((int)time);
	MapKeys::iterator It1 = _keys.upper_bound((int)time);
	
	if (It0 != _keys.begin())
		It0--;

	double t;

	if (It1 == _keys.end())
	{
		It1 = It0;
		t = 1.;
	}
	else
	{
		double stepDuration = It1->first - It0->first;
		double localTime = time - It0->first;
		if (stepDuration)
			t = localTime / stepDuration;
		else
			t = 1.;
		
		if (t < 0.)
			t = 0.;
		if (t > 1.)
			t = 1.;
	}

	InterpolateCosine(*It0->second, *It1->second, t);
}

void LXTrack::UpdateCubic(double time)
{
	MapKeys::iterator It0 = _keys.upper_bound((int)time);
	MapKeys::iterator It1 = _keys.upper_bound((int)time);
	MapKeys::iterator It2 = _keys.upper_bound((int)time);
	MapKeys::iterator It3 = _keys.upper_bound((int)time);
	
	if (It0 != _keys.begin())
	{
		It0--;
		if (It0 != _keys.begin())
			It0--;
	}

	if (It1 != _keys.begin())
		It1--;

	if (It3 != _keys.end())
	{
		It3++;
		if (It3 == _keys.end())
			It3 = It2;
	}

	double t;

	if (It2 == _keys.end())
	{
		It3 = It2 = It1;
		t = 1.;
	}
	else
	{
		double stepDuration = It2->first - It1->first;
		double localTime = time - It1->first;
		if (stepDuration)
			t = localTime / stepDuration;
		else
			t = 1.;

		if (t < 0.)
			t = 0.;
		if (t > 1.)
			t = 1.;
	}

	CHK(It0->first <= It1->first);
	CHK(It1->first <= It2->first);
	CHK(It2->first <= It3->first);

	InterpolateCubic(*It0->second, *It1->second, *It2->second, *It3->second, t);
}

void LXTrack::Capture(LXProperty* pProperty, int time)
{
	LXVariant* pVariant = pProperty->CreateVariant();
	Capture(pProperty, pVariant, time);
}

void LXTrack::Capture(LXProperty* pProperty, LXVariant* pVariant, int time)
{
	auto it = _keys.find(time);
	if (it != _keys.end())
	{
		// Update the key
		_keys[time]->SetVariant(pVariant);
	}
	else
	{
		// Create a new key
		LXKey* key = new LXKey(this, pVariant);
		key->SetTime(time);
		_keys[time] = key;
	}
}

bool LXTrack::HasKey(int position)
{
	if (_keys.find(position) != _keys.end())
		return true;
	else
		return false;
}

void LXTrack::DefineProperties()
{
	// ID of the targeted object
	{
		auto p = DefinePropertyString("ObjectID", LXPropertyID::TRACK_OBJECT_UID, &_objectUID);
		p->SetReadOnly(true);
	}

	// PropertyName of the targeted object
	{
		auto p = DefinePropertyString("Property", LXPropertyID::TRACK_PROPERTY, &_propertyName);
		p->SetReadOnly(true);
	}

	// Interpolation
	{
		auto prop = DefinePropertyEnum("Interpolation", LXPropertyID::TRACK_INTERPOLATION, (uint*)&_eInterpolation);
		prop->AddChoice(L"None", Interpolation_None);
		prop->AddChoice(L"Linear", Interpolation_Linear);
		prop->AddChoice(L"Cosine", Interpolation_Cosine);
		prop->AddChoice(L"Cubic", Interpolation_Cubic);
	}
}

void LXTrack::MoveKey(LXKey* pKey, int time)
{
	int oldTime = pKey->GetTime();

	if (oldTime == time)
		return;

	_keys.erase(oldTime);
	_keys[time] = pKey;
	pKey->SetTime(time);
}

void LXTrack::Remove(LXKey* pKey)
{
	CHK(_keys.find(pKey->GetTime()) != _keys.end());
	_keys.erase(pKey->GetTime());
}

void LXTrack::Add(LXKey* pKey)
{
	CHK(_keys.find(pKey->GetTime()) == _keys.end());
	_keys[pKey->GetTime()] = pKey;
}

template <class T>
void LXTrackT<T>::InterpolateLinear(const LXKey& key0, const LXKey& key1, double t)
{
	CHK(_pProperty);
	CHK(t >= 0. && t <= 1.);

	LXVariantT<T>* pv0 = (LXVariantT<T>*)key0.GetVariant();
	LXVariantT<T>* pv1 = (LXVariantT<T>*)key1.GetVariant();

	T res;
	LXInterpolator::InterpolateLinear<T>(pv0->GetValue(), pv1->GetValue(), t, res);
	_pProperty->SetValue(res);
}

template <class T>
void LXTrackT<T>::InterpolateCosine(const LXKey& key0, const LXKey& key1, double t)
{
	CHK(_pProperty);
	CHK(t >= 0. && t <= 1.);

	LXVariantT<T>* pv0 = (LXVariantT<T>*)key0.GetVariant();
	LXVariantT<T>* pv1 = (LXVariantT<T>*)key1.GetVariant();

	T res;
	LXInterpolator::InterpolateSmooth<T>(pv0->GetValue(), pv1->GetValue(), t, res);
	_pProperty->SetValue(res);
}

template <class T>
void LXTrackT<T>::InterpolateCubic(const LXKey& key0, const LXKey& key1, const LXKey& key2, const LXKey& key3, double t)
{
	CHK(_pProperty);
	CHK(t >= 0. && t <= 1.);

	LXVariantT<T>* pv0 = (LXVariantT<T>*)key0.GetVariant();
	LXVariantT<T>* pv1 = (LXVariantT<T>*)key1.GetVariant();
	LXVariantT<T>* pv2 = (LXVariantT<T>*)key2.GetVariant();
	LXVariantT<T>* pv3 = (LXVariantT<T>*)key3.GetVariant();

	T res;
	LXInterpolator::InterpolateCubic<T>(pv0->GetValue(), pv1->GetValue(), pv2->GetValue(), pv3->GetValue(), t, res);
	_pProperty->SetValue(res);
}

template <class T>
LXProperty* LXTrackT<T>::GetProperty()
{ 
	if (!_pProperty)
	{
		LXSmartObject* pSmartObject = GetEngine().GetProject()->GetObjectFromUID(_objectUID);
		CHK(pSmartObject);
		if (pSmartObject)
			_pProperty = dynamic_cast<LXPropertyT<T>*>(pSmartObject->GetProperty(_propertyName));
	}
	return _pProperty; 
}

// Specialization
template<> const LXString LXTrackT<bool>::GetTypeString() const { return L" Type=\"bool\""; }
template<> const LXString LXTrackT<float>::GetTypeString() const { return L" Type=\"float\""; }
template<> const LXString LXTrackT<double>::GetTypeString() const { return L" Type=\"double\""; }
template<> const LXString LXTrackT<uint>::GetTypeString() const { return L" Type=\"uint\""; }
template<> const LXString LXTrackT<int>::GetTypeString() const { return L" Type=\"int\""; }
template<> const LXString LXTrackT<vec2f>::GetTypeString() const { return L" Type=\"vec2f\""; }
template<> const LXString LXTrackT<vec3f>::GetTypeString() const { return L" Type=\"vec3f\""; }
template<> const LXString LXTrackT<vec4f>::GetTypeString() const { return L" Type=\"vec4f\""; }
template<> const LXString LXTrackT<LXColor4f>::GetTypeString() const { return L" Type=\"LXColor4f\""; }
template<> const LXString LXTrackT<LXMatrix>::GetTypeString() const { return L" Type=\"matrix\""; }
template<> const LXString LXTrackT<LXString>::GetTypeString() const { return L" Type=\"string\""; }
template<> const LXString LXTrackT<LXFilepath>::GetTypeString() const { return L" Type=\"filepath\""; }
template<> const LXString LXTrackT<LXAssetPtr>::GetTypeString() const { return L" Type=\"asset\""; }

//Explicit class Instantiation
template class LXTrackT < bool >;
template class LXTrackT < float > ;
template class LXTrackT < double >;
template class LXTrackT < uint >;
template class LXTrackT < int >;
template class LXTrackT < vec2f >;
template class LXTrackT < vec3f >;
template class LXTrackT < vec4f >;
template class LXTrackT < LXColor4f >;
template class LXTrackT < LXMatrix >;
template class LXTrackT < LXString >;
template class LXTrackT < LXFilepath >;
template class LXTrackT < LXAssetPtr >;
