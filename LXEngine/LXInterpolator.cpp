//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXAsset.h"
#include "LXFilepath.h"
#include "LXInterpolator.h"
#include "LXLogger.h"
#include "LXMath.h"
#include "LXMatrix.h"
#include "LXProperty.h"
#include "LXVariant.h"
#include "LXVec3.h"
#include "LXVec4.h"

LXInterpolator::LXInterpolator()
{
}

LXInterpolator::~LXInterpolator()
{
}

template <class T>
void LXInterpolator::InterpolateLinear(const T& v0, const T& v1, double t, T& result)
{
	CHK(t >= 0. && t <= 1.);
	result = v0 + (v1 - v0) * t;
}

template <class T>
void LXInterpolator::InterpolateSmooth(const T& v0, const T& v1, double t, T& result)
{
	CHK(t >= 0.0f && t <= 1.);
	t = (1. - cos(LX_PI * t)) * 0.5;
	result = v0 + (v1 - v0) * t;
}

template <class T>
void LXInterpolator::InterpolateCubic(const T& v0, const T& v1, const T& v2, const T& v3, double t, T& result)
{
	T a0, a1, a2, a3;
	double mu2 = t*t;
	a0 = v3 - v2 - v0 + v1;
	a1 = v0 - v1 - a0;
	a2 = v2 - v0;
	a3 = v1;
	result = a0 * t * mu2 + a1 * mu2 + a2 * t + a3;
}

// Explicit method Instantiation
template LXENGINE_API void LXInterpolator::InterpolateLinear(const float& v0, const float& v1, double t, float& result);
template LXENGINE_API void LXInterpolator::InterpolateSmooth(const float& v0, const float& v1, double t, float& result);
template LXENGINE_API void LXInterpolator::InterpolateCubic(const float& v0, const float& v1, const float& v2, const float& v3, double t, float& result);

template LXENGINE_API void LXInterpolator::InterpolateLinear(const double& v0, const double& v1, double t, double& result);
template LXENGINE_API void LXInterpolator::InterpolateSmooth(const double& v0, const double& v1, double t, double& result);
template LXENGINE_API void LXInterpolator::InterpolateCubic(const double& v0, const double& v1, const double& v2, const double& v3, double t, double& result);

template LXENGINE_API void LXInterpolator::InterpolateLinear(const uint& v0, const uint& v1, double t, uint& result);
template LXENGINE_API void LXInterpolator::InterpolateSmooth(const uint& v0, const uint& v1, double t, uint& result);
template LXENGINE_API void LXInterpolator::InterpolateCubic(const uint& v0, const uint& v1, const uint& v2, const uint& v3, double t, uint& result);

template LXENGINE_API void LXInterpolator::InterpolateLinear(const int& v0, const int& v1, double t, int& result);
template LXENGINE_API void LXInterpolator::InterpolateSmooth(const int& v0, const int& v1, double t, int& result);
template LXENGINE_API void LXInterpolator::InterpolateCubic(const int& v0, const int& v1, const int& v2, const int& v3, double t, int& result);

template LXENGINE_API void LXInterpolator::InterpolateLinear(const vec2f& v0, const vec2f& v1, double t, vec2f& result);
template LXENGINE_API void LXInterpolator::InterpolateSmooth(const vec2f& v0, const vec2f& v1, double t, vec2f& result);
template LXENGINE_API void LXInterpolator::InterpolateCubic(const vec2f& v0, const vec2f& v1, const vec2f& v2, const vec2f& v3, double t, vec2f& result);

template LXENGINE_API void LXInterpolator::InterpolateLinear(const vec3f& v0, const vec3f& v1, double t, vec3f& result);
template LXENGINE_API void LXInterpolator::InterpolateSmooth(const vec3f& v0, const vec3f& v1, double t, vec3f& result);
template LXENGINE_API void LXInterpolator::InterpolateCubic(const vec3f& v0, const vec3f& v1, const vec3f& v2, const vec3f& v3, double t, vec3f& result);

template LXENGINE_API void LXInterpolator::InterpolateLinear(const vec4f& v0, const vec4f& v1, double t, vec4f& result);
template LXENGINE_API void LXInterpolator::InterpolateSmooth(const vec4f& v0, const vec4f& v1, double t, vec4f& result);
template LXENGINE_API void LXInterpolator::InterpolateCubic(const vec4f& v0, const vec4f& v1, const vec4f& v2, const vec4f& v3, double t, vec4f& result);

template LXENGINE_API void LXInterpolator::InterpolateLinear(const LXColor4f& v0, const LXColor4f& v1, double t, LXColor4f& result);
template LXENGINE_API void LXInterpolator::InterpolateSmooth(const LXColor4f& v0, const LXColor4f& v1, double t, LXColor4f& result);
template LXENGINE_API void LXInterpolator::InterpolateCubic(const LXColor4f& v0, const LXColor4f& v1, const LXColor4f& v2, const LXColor4f& v3, double t, LXColor4f& result);

//------------------------------------------------------------------------------------------------------
// Specializations
//------------------------------------------------------------------------------------------------------

template<>
void LXInterpolator::InterpolateLinear<bool>(const bool& v0, const bool& v1, double t, bool& result)
{
	t < 1.0 ? result = v0 : result = v1;
}

template<>
void LXInterpolator::InterpolateSmooth<bool>(const bool& v0, const bool& v1, double t, bool& result)
{
	t < 1.0 ? result = v0 : result = v1;
}

template<>
void LXInterpolator::InterpolateCubic<bool>(const bool& v0, const bool& v1, const bool& v2, const bool& v3, double t, bool& result)
{
	t < 1.0 ? result = v1 : result = v2;
}

//------------------------------------------------------------------------------------------------------

template<>
void LXInterpolator::InterpolateLinear<LXMatrix>(const LXMatrix& v0, const LXMatrix& v1, double t, LXMatrix& result)
{
	CHK(0);
}

template<>
void LXInterpolator::InterpolateSmooth<LXMatrix>(const LXMatrix& v0, const LXMatrix& v1, double t, LXMatrix& result)
{
	CHK(0);
}

template<>
void LXInterpolator::InterpolateCubic<LXMatrix>(const LXMatrix& v0, const LXMatrix& v1, const LXMatrix& v2, const LXMatrix& v3, double t, LXMatrix& result)
{
	CHK(0);
}

//------------------------------------------------------------------------------------------------------

template<>
void LXInterpolator::InterpolateLinear<LXString>(const LXString& v0, const LXString& v1, double t, LXString& result)
{
	CHK(0);
}

template<>
void LXInterpolator::InterpolateSmooth<LXString>(const LXString& v0, const LXString& v1, double t, LXString& result)
{
	CHK(0);
}

template<>
void LXInterpolator::InterpolateCubic<LXString>(const LXString& v0, const LXString& v1, const LXString& v2, const LXString& v3, double t, LXString& result)
{
	CHK(0);
}

//------------------------------------------------------------------------------------------------------

template<>
void LXInterpolator::InterpolateLinear<LXFilepath>(const LXFilepath& v0, const LXFilepath& v1, double t, LXFilepath& result)
{
	CHK(0);
}

template<>
void LXInterpolator::InterpolateSmooth<LXFilepath>(const LXFilepath& v0, const LXFilepath& v1, double t, LXFilepath& result)
{
	CHK(0);
}

template<>
void LXInterpolator::InterpolateCubic<LXFilepath>(const LXFilepath& v0, const LXFilepath& v1, const LXFilepath& v2, const LXFilepath& v3, double t, LXFilepath& result)
{
	CHK(0);
}

//------------------------------------------------------------------------------------------------------

template<>
void LXInterpolator::InterpolateLinear<LXAssetPtr>(const LXAssetPtr& v0, const LXAssetPtr& v1, double t, LXAssetPtr& result)
{
	CHK(0);
}

template<>
void LXInterpolator::InterpolateSmooth<LXAssetPtr>(const LXAssetPtr& v0, const LXAssetPtr& v1, double t, LXAssetPtr& result)
{
	CHK(0);
}

template<>
void LXInterpolator::InterpolateCubic<LXAssetPtr>(const LXAssetPtr& v0, const LXAssetPtr& v1, const LXAssetPtr& v2, const LXAssetPtr& v3, double t, LXAssetPtr& result)
{
	CHK(0);
}

