//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXSmartObject.h"
#include "LXTrack.h"
#include "LXProperty.h"

typedef map<LXProperty*, LXTrack*> MapPropertyTrack;

class LXCORE_API LXAnimation : public LXSmartObject
{

public:

	LXAnimation();
	virtual ~LXAnimation();

	// Overridden from LXSmartObject
	virtual bool OnSaveChild(const TSaveContext& saveContext) const override;
	virtual bool OnLoadChild(const TLoadContext& loadContext) override;
	virtual void GetChildren(ListSmartObjects&) override;

	//------------------------------------------------------------------------------------------------------
	// Remove the given key 
	//------------------------------------------------------------------------------------------------------
	void Remove(LXKey* pKey);

	//------------------------------------------------------------------------------------------------------
	// Add an existing key to his corresponding track
	//------------------------------------------------------------------------------------------------------
	void Add(LXKey* pKey);

	//------------------------------------------------------------------------------------------------------
	// Create (or update) a key with the specified property and value
	//------------------------------------------------------------------------------------------------------
	template <class T>
	void AddKey(LXProperty* pProp, const T& newValue, DWORD dwTime);

	void SetDuration(double dDuration)  { _dDuration = dDuration; }
	double GetDuration()const{ return _dDuration; }

	void SetPosition(double dPosition)  { _dPosition = dPosition; }
	double GetPosition()const{ return _dPosition; }
	
	const MapPropertyTrack& GetTracks() const { return _tracks; }
		
	bool IsFinished() const { return _bFinished; }
	
	//------------------------------------------------------------------------------------------------------
	// Update the scene last frame time
	// Animation position is automatically incremented
	//------------------------------------------------------------------------------------------------------
	void Update(double dFrameTime);

	//------------------------------------------------------------------------------------------------------
	// Update the scene with the current animation position
	//------------------------------------------------------------------------------------------------------
	void Update();

private:

	void DefineProperties();
	
protected:

	double _dPosition = 0.;
	double _dDuration = 0.;
	bool _bFinished = false;
	MapPropertyTrack _tracks;
};

typedef list<LXAnimation*> ListAnimations;
typedef vector<LXAnimation*> ArrayAnimations;