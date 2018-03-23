//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXSmartObject.h"
#include "LXKey.h"
#include "LXProperty.h"

class LXActorMeshPath;

class LXCORE_API LXTrack : public LXSmartObject
{

public:

	enum LXInterpolation // DataModel
	{
		Interpolation_None,
		Interpolation_Linear,
		Interpolation_Cosine,
		Interpolation_Cubic
	};

	LXTrack();
	virtual ~LXTrack();

	// Overridden from LXSmartObject
	virtual bool OnSaveChild(const TSaveContext& saveContext) override;
	virtual bool OnLoadChild(const TLoadContext& loadContext) override;
	virtual void GetChildren(ListSmartObjects& list)override;

	void Update(double time);
	
	virtual void InterpolateLinear(const LXKey& key0, const LXKey& key1, double time) = 0;
	virtual void InterpolateCosine(const LXKey& key0, const LXKey& key1, double time) = 0;
	virtual void InterpolateCubic(const LXKey& key0, const LXKey& key1, const LXKey& key2, const LXKey& key3, double t) = 0;
	virtual LXProperty* GetProperty() = 0;
	virtual	const LXString GetTypeString() const = 0;
	
	void Capture(LXProperty* pProperty, int time);
	void Capture(LXProperty* pProperty, LXVariant* pVariant, int time);
	bool HasKey(int time);

	const MapKeys& GetKeys() const { return	_keys; }

	void MoveKey(LXKey* pKey, int time);
	void Remove(LXKey* pKey);
	void Add(LXKey* pKey);
	
private:

	void DefineProperties();
	void UpdateLinear(double time);
	void UpdateCosine(double time);
	void UpdateCubic(double time);

protected:

	MapKeys		_keys;
	LXString	_objectUID;
	LXString	_propertyName;
	LXInterpolation _eInterpolation = Interpolation_Cubic;
};

template <class T>
class  LXCORE_API LXTrackT : public LXTrack
{

public:

	LXTrackT(){ }
	LXTrackT(LXPropertyT<T>* property):_pProperty(property)
	{ 
		_propertyName = _pProperty->GetName(); 
		_objectUID = *_pProperty->GetLXObject()->GetUID(true); 
	};
	virtual ~LXTrackT() { };
	virtual void InterpolateLinear(const LXKey& key0, const LXKey& key1, double time) override;
	virtual void InterpolateCosine(const LXKey& key0, const LXKey& key1, double time) override;
	virtual void InterpolateCubic(const LXKey& key0, const LXKey& key1, const LXKey& key2, const LXKey& key3, double t) override;
	virtual LXProperty* GetProperty() override;
	virtual	const LXString GetTypeString() const override;

protected:

	LXPropertyT<T>* _pProperty = nullptr;
	LXActorMeshPath* _meshPath = nullptr; // Optional mesh to illustrate and manipulate the keys

};

