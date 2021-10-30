//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXCore/LXObject.h"
#include "LXMath.h"

class LXCamera : public LXObject
{
public:

	void SetPosition(const vec3f& position) { _position = position; _validMatrixView = false; }
	void SetDirection(const vec3f& direction) { _direction = direction;  _validMatrixView = false;}
	void SetFov(float angle) { _angle = angle;  _validMatrixView = false; }
	void SetAspectRatio(float aspect) { _aspect = aspect;  _validMatrixView = false; }

	bool IsOrtho() const { return _orthographic; }
	const vec3f& GetPosition() const { return _position; }
	const vec3f& GetDirection() const { return _direction;}
	float GetFov() const { return _angle; }
	float GetHeight() const { return _height; }
	float GetAspectRatio() const { return _aspect; }
	float GetNear() const { return 10.f; }
	float GetFar() const { return KM2CM(10.f); }

	const LXMatrix& GetMatrixView();

private:

	void BuildMatrixView();
	
private:

	LXMatrix _matrixView;
	bool _validMatrixView = false;
	bool _orthographic = false;

	vec3f _position = vec3f(0.f, 0.f, 0.f);
	vec3f _direction = vec3f(1.f, 0.f, 0.f);
	float _angle = 55.0f;
	float _height = 1000.f;
	float _aspect = 1.f;
};

