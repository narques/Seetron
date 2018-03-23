//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once
#include "LXActorMesh.h"

class LXActorLine : public LXActorMesh
{

public:

	LXActorLine(LXProject* pDocument);
	virtual ~LXActorLine(void);

	void AddPoint						(const vec3f& p);
	void InsertPoint					(const vec3f& p, int position);
	
	// Internal
	void SetPickedID					( int i, const vec3f& vPosition );
	int GetPickedID						( ) const { return _nPickedID; };
	const vec3f& GetPickedPoint			( ) const { return _vPicked; }

#if LX_ANCHOR
	void OnAnchorMove					(LXAnchor* pAnchor) override;
#endif
	
	// Tools
	void SplitWithPickedPoint();

private:

	LXPrimitive* _primLine;

	int			 _nPickedID;
	vec3f		 _vPicked;

};

