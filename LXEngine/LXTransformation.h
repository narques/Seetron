//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXMatrix.h"

class LXCORE_API LXTransformation
{

public:

	LXTransformation();
	~LXTransformation();

	void DefineProperties(LXSmartObject* SmartObject);
	void OnChange(std::function<void()> eval);
	void InvalidateMatrixLocal();
	const LXMatrix& GetMatrix();
	void SetTranslation(const vec3f& v);
	void SetRotation(const vec3f& v);
	void SetQuaternion(const vec4f& v);
	void SetScale(const vec3f& v);
	const vec3f& GetTranslation() const { return _Translation; }
	const vec3f& GetRotation() const { return _Rotation; }
	const vec3f& GetScale() const { return _Scale; }

private:

	void BuildMatrix();
	
private:

	vec3f _Translation;
	vec3f _Rotation;	// In degree - /!\ Roll, Pitch, Yaw order
	vec3f _Scale;

	LXMatrix _Matrix;
	bool _bValidMatrix = false;

	std::function<void()> _FuncOnChange;
};

