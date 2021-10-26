//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXBbox.h"
#include "LXMath.h"

LXBBox::LXBBox(void)
{
	Reset();
}

LXBBox::LXBBox(const LXBBox& bbox)
{
	m_ptCenter = bbox.m_ptCenter;
	m_ptMax = bbox.m_ptMax;
	m_ptMin = bbox.m_ptMin;
	m_fDiag = bbox.m_fDiag;
	m_bValid = bbox.m_bValid;
}

LXBBox::~LXBBox(void)
{
}

LXBBox& LXBBox::operator=(const LXBBox& m)
{
	m_fDiag = m.m_fDiag;
	m_bValid = m.m_bValid;
	m_ptCenter = m.m_ptCenter;
	m_ptMax = m.m_ptMax;
	m_ptMin = m.m_ptMin;
	return *this;
}

void LXBBox::Reset()
{
	m_ptMin.x = m_ptMin.y = m_ptMin.z = FLT_MAX;
	m_ptMax.x = m_ptMax.y = m_ptMax.z = -FLT_MAX;
	m_ptCenter.x = m_ptCenter.y = m_ptCenter.z = 0.0f;
	m_fDiag = 0.0f;
	m_bValid = false;
}

bool LXBBox::TryToValidate()
{
	if (m_ptMin.x == FLT_MAX)
		return false;
	if (m_ptMin.y == FLT_MAX)
		return false;
	if (m_ptMin.z == FLT_MAX)
		return false;
	if (m_ptMax.x == -FLT_MAX)
		return false;
	if (m_ptMax.y == -FLT_MAX)
		return false;
	if (m_ptMax.z == -FLT_MAX)
		return false;

	CHK(m_ptMax.x >= m_ptMin.x);
	CHK(m_ptMax.y >= m_ptMin.y);
	CHK(m_ptMax.z >= m_ptMin.z);

	return true;
}

void LXBBox::Add(const LXBBox& box)
{
	if (box.m_bValid)
	{
		Add(box.m_ptMax);
		Add(box.m_ptMin);
	}
}

void LXBBox::Add(const vec3f& point)
{
	CHK(point.x != FLT_MAX);
	CHK(point.x != -FLT_MAX);
	CHK(point.y != FLT_MAX);
	CHK(point.y != -FLT_MAX);
	CHK(point.z != FLT_MAX);
	CHK(point.z != -FLT_MAX);

	m_ptMax.SetMax(m_ptMax, point);
	m_ptMin.SetMin(m_ptMin, point);

	m_bValid = TryToValidate();

	if (m_bValid)
	{
		ComputeCenter();
		ComputeDiagonal();
	}
}

void LXBBox::ExtendZ(float z)
{
	m_ptMax.z += z;

	m_bValid = TryToValidate();

	if (m_bValid)
	{
		ComputeCenter();
		ComputeDiagonal();
	}
}

void LXBBox::ComputeCenter()
{
	CHK(m_bValid);
	m_ptCenter = (m_ptMax + m_ptMin) * 0.5f;
}

void LXBBox::ComputeDiagonal()
{
	CHK(m_bValid);
	m_fDiag = m_ptMin.Distance(m_ptMax);
	m_bValid = m_fDiag > LX_SMALL_NUMBER6;
}

void LXBBox::GetPoints( vec3f* pPoints ) const
{
	CHK(m_bValid);

	pPoints[0].Set(m_ptMin.x, m_ptMin.y, m_ptMin.z);
	pPoints[1].Set(m_ptMax.x, m_ptMin.y, m_ptMin.z);
	pPoints[2].Set(m_ptMin.x, m_ptMax.y, m_ptMin.z);
	pPoints[3].Set(m_ptMax.x, m_ptMax.y, m_ptMin.z);

	pPoints[4].Set(m_ptMin.x, m_ptMin.y, m_ptMax.z);
	pPoints[5].Set(m_ptMax.x, m_ptMin.y, m_ptMax.z);
	pPoints[6].Set(m_ptMin.x, m_ptMax.y, m_ptMax.z);
	pPoints[7].Set(m_ptMax.x, m_ptMax.y, m_ptMax.z);
}

bool LXBBox::IsPoint() const
{
	CHK(m_bValid);
	return m_ptMin == m_ptMax;
}

bool LXBBox::IsLine() const
{
	CHK(m_bValid);
	return (m_ptMin.z - m_ptMax.z == 0) || (m_ptMin.y - m_ptMax.y == 0) || (m_ptMin.x - m_ptMax.x == 0);
}
