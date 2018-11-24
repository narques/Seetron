//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXTransformation.h"
#include "LXSmartObject.h"
#include "LXMath.h"
#include "LXMemory.h" // --- Must be the last included ---

LXTransformation::LXTransformation()
{
	_Translation.Set(0.f, 0.f, 0.f);
	_Rotation.Set(0.f, 0.f, 0.f);
	_Scale.Set(1.f, 1.f, 1.f);
}

LXTransformation::~LXTransformation()
{
}

void LXTransformation::DefineProperties(LXSmartObject* SmartObject)
{
	// Position
	LXPropertyVec3f* pPropPosition = SmartObject->DefineProperty(L"Position", LXPropertyID::POSITION, &_Translation);
	pPropPosition->SetAnimatable(true);
	pPropPosition->SetLambdaOnChange([this](LXProperty*)
	{
		InvalidateMatrixLocal();
	});

	// Rotation
	LXPropertyVec3f* pPropRotation = SmartObject->DefineProperty(L"Rotation", LXPropertyID::ROTATION, &_Rotation);
	pPropRotation->SetLambdaOnChange([this](LXProperty*)
	{
		InvalidateMatrixLocal();
	});

	// Scale
	LXPropertyVec3f* pPropScale = SmartObject->DefineProperty(L"Scale", LXPropertyID::SCALE, &_Scale);
	pPropScale->SetLambdaOnChange([this](LXProperty*)
	{
		InvalidateMatrixLocal();
	});
}

void LXTransformation::OnChange(std::function<void()> eval)
{
	_FuncOnChange = eval;
}

void LXTransformation::InvalidateMatrixLocal()
{
	_bValidMatrix = false;
	if(_FuncOnChange)
		_FuncOnChange();
}

const LXMatrix& LXTransformation::GetMatrix()
{
	if (!_bValidMatrix)
		BuildMatrix();

	return _Matrix;
}

void LXTransformation::SetTranslation(const vec3f& v)
{
	if (!IsNearlyEqual(v, _Translation))
	{
		_Translation = v;
		InvalidateMatrixLocal();
	}
}

void LXTransformation::SetRotation(const vec3f& v)
{
	CHK(IsValid(v));

	if (!IsNearlyEqual(v, _Rotation))
	{
		_Rotation = v;
		InvalidateMatrixLocal();
	}
}

void LXTransformation::SetQuaternion(const vec4f& v)
{
	vec3f Euler;

	// roll (x-axis rotation)
	float sinr = 2.0f * (v.w * v.x + v.y * v.z);
	float cosr = 1.0f - 2.0f * (v.x * v.x + v.y * v.y);
	Euler.x = atan2(sinr, cosr);

	// pitch (y-axis rotation)
	float sinp = 2.0f * (v.w * v.y - v.z * v.x);
	if (fabs(sinp) >= 1.0f)
		Euler.y = copysign(LX_PI_2, sinp); // use 90 degrees if out of range
	else
		Euler.y = asin(sinp);

	// yaw (z-axis rotation)
	float siny = 2.0f * (v.w * v.z + v.x * v.y);
	float cosy = 1.0f - 2.0f * (v.y * v.y + v.z * v.z);
	Euler.z = atan2(siny, cosy);
	
	SetRotation(Euler);
}

void LXTransformation::SetScale(const vec3f& v)
{
	if (!IsNearlyEqual(v, _Scale))
	{
		_Scale = v;
		InvalidateMatrixLocal();
	}
}

void LXTransformation::BuildMatrix()
{
	LXMatrix MatrixTranslation, MatrixRotation, MatrixScale;
	MatrixTranslation.SetOrigin(_Translation);
	MatrixRotation.SetRotation(_Rotation);
	MatrixScale.SetScale(_Scale);
	_Matrix = MatrixTranslation * MatrixRotation * MatrixScale;
	_bValidMatrix = true;
}
