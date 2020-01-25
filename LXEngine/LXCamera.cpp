//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXCamera.h"

const LXMatrix& LXCamera::GetMatrixView()
{
	if (!_validMatrixView)
	{
		BuildMatrixView();
	}

	return _matrixView;
}

void LXCamera::BuildMatrixView()
{
	_matrixView.SetIdentity();

	vec3f vz = _direction;
	vec3f vy = vec3f(0.f, 0.f, 1.0f);
	vec3f vx = CrossProduct(vy, vz);

	vx.Normalize();
	if (!IsValid(vx))
	{
		vy = vec3f(0.f, 1.f, 0.0f);
		vx = CrossProduct(vy, vz);
		vx.Normalize();
	}

	vy.CrossProduct(vz, vx);
	vy.Normalize();

	LXMatrix matCamera;
	matCamera.SetXYZ(vx, vy, vz);
	_matrixView = Inverse(matCamera);

	vec3f vo = _position;

	LXMatrix matPosition;
	matPosition.SetOrigin(-vo.x, -vo.y, -vo.z);

	_matrixView = _matrixView * matPosition;

	_validMatrixView = true;
}