//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXAxis.h"
#include "LXBBox.h"
#include "LXMath.h"
#include "LXMatrix.h"

bool IsValid(const LXMatrix& m)
{
	for (int i = 0; i < 16; i++)
	{
		if (_isnanf(m.GetPtr()[i]))
		{
			return false;
		}
	}
		
	return true;
}

LXMatrix::LXMatrix()
{
	SetIdentity();
}

LXMatrix::~LXMatrix()  
{ 
}

bool LXMatrix::operator!=( LXMatrix& m) const
{
	return memcmp(m_fData, m.m_fData, sizeof(m_fData)) != 0;
}

bool LXMatrix::operator!=( const LXMatrix& m) const
{
	return memcmp(m_fData, m.m_fData, sizeof(m_fData)) != 0;
}

bool LXMatrix::operator==( const LXMatrix& m) const
{
	return memcmp(m_fData, m.m_fData, sizeof(m_fData)) == 0;
}

const LXMatrix LXMatrix::MultProj(const LXMatrix& matrix) const
{
	//CHK(IsPerspectiveProjection());
	
	float res[16];

	res[0] = m_fData[0]*matrix[0]; 
	res[1] = m_fData[5]*matrix[1];
	res[2] = m_fData[10]*matrix[2];  
	res[3] = m_fData[11]*matrix[2];  

	res[4] = m_fData[0]*matrix[4];   
	res[5] = m_fData[5]*matrix[5];   
	res[6] = m_fData[10]*matrix[6];  
	res[7] = m_fData[11]*matrix[6];  

	res[8] = m_fData[0]*matrix[8];   
	res[9] =  m_fData[5]*matrix[9];	 
	res[10] = m_fData[10]*matrix[10];
	res[11] = m_fData[11]*matrix[10];

	res[12] = m_fData[0]*matrix[12];  
	res[13] = m_fData[5]*matrix[13];
	res[14] = m_fData[10]*matrix[14] + m_fData[14]; 
	res[15] = m_fData[11]*matrix[14];

	return LXMatrix(res);
}

const LXMatrix LXMatrix::MultHomogenic(const LXMatrix& matrix) const
{
	float res[16];

	res[0] = m_fData[0]*matrix[0] + m_fData[4]*matrix[1] + m_fData[8]*matrix[2];	// + m_fData[12]*matrix[3];  
	res[1] = m_fData[1]*matrix[0] + m_fData[5]*matrix[1] + m_fData[9]*matrix[2];	// + m_fData[13]*matrix[3];
	res[2] = m_fData[2]*matrix[0] + m_fData[6]*matrix[1] + m_fData[10]*matrix[2];   // + m_fData[14]*matrix[3];
	res[3] = 0.f;//m_fData[3]*matrix[0] + m_fData[7]*matrix[1] + m_fData[11]*matrix[2];   // + m_fData[15]*matrix[3];

	res[4] = m_fData[0]*matrix[4] + m_fData[4]*matrix[5] + m_fData[8]*matrix[6];   // + m_fData[12]*matrix[7];
	res[5] = m_fData[1]*matrix[4] + m_fData[5]*matrix[5] + m_fData[9]*matrix[6];   // + m_fData[13]*matrix[7];
	res[6] = m_fData[2]*matrix[4] + m_fData[6]*matrix[5] + m_fData[10]*matrix[6];  // + m_fData[14]*matrix[7];
	res[7] = 0.f;//m_fData[3]*matrix[4] + m_fData[7]*matrix[5] + m_fData[11]*matrix[6];  // + m_fData[15]*matrix[7];

	res[8] = m_fData[0]*matrix[8] + m_fData[4]*matrix[9] + m_fData[8]*matrix[10];    // + m_fData[12]*matrix[11];
	res[9] = m_fData[1]*matrix[8] + m_fData[5]*matrix[9] + m_fData[9]*matrix[10];	 // + m_fData[13]*matrix[11];
	res[10] = m_fData[2]*matrix[8] + m_fData[6]*matrix[9] + m_fData[10]*matrix[10];	 // + m_fData[14]*matrix[11];
	res[11] = 0.f;//m_fData[3]*matrix[8] + m_fData[7]*matrix[9] + m_fData[11]*matrix[10];	 // + m_fData[15]*matrix[11];

	res[12] = m_fData[0]*matrix[12] + m_fData[4]*matrix[13] + m_fData[8]*matrix[14] + m_fData[12];  // * matrix[15];
	res[13] = m_fData[1]*matrix[12] + m_fData[5]*matrix[13] + m_fData[9]*matrix[14] + m_fData[13];  // * matrix[15];
	res[14] = m_fData[2]*matrix[12] + m_fData[6]*matrix[13] + m_fData[10]*matrix[14] + m_fData[14]; // * matrix[15];
	res[15] = 1.f;//m_fData[3]*matrix[12] + m_fData[7]*matrix[13] + m_fData[11]*matrix[14] + m_fData[15]; // * matrix[15];

	return LXMatrix(res);
}

const LXMatrix LXMatrix::operator*(const LXMatrix& matrix) const
{
	if (0)
	{
		// Non Homogenic 
		LXMatrix tmp;
		 for (unsigned int i=0; i<4; i++)
		 {
			for (unsigned int j=0; j<4; j++)
			{
				tmp.m_44[i][j] = (matrix.m_44[i][0]*m_44[0][j] +  matrix.m_44[i][1]*m_44[1][j] + matrix.m_44[i][2]*m_44[2][j] + matrix.m_44[i][3]*m_44[3][j]);
			}
		 }
		 return tmp;
	}
	else
	{
		// For Homogenic

		float res[16];

		res[0] = m_fData[0]*matrix[0] + m_fData[4]*matrix[1] + m_fData[8]*matrix[2];	// + m_fData[12]*matrix[3];  
		res[1] = m_fData[1]*matrix[0] + m_fData[5]*matrix[1] + m_fData[9]*matrix[2];	// + m_fData[13]*matrix[3];
		res[2] = m_fData[2]*matrix[0] + m_fData[6]*matrix[1] + m_fData[10]*matrix[2];   // + m_fData[14]*matrix[3];
		res[3] = m_fData[3]*matrix[0] + m_fData[7]*matrix[1] + m_fData[11]*matrix[2];   // + m_fData[15]*matrix[3];

		res[4] = m_fData[0]*matrix[4] + m_fData[4]*matrix[5] + m_fData[8]*matrix[6];   // + m_fData[12]*matrix[7];
		res[5] = m_fData[1]*matrix[4] + m_fData[5]*matrix[5] + m_fData[9]*matrix[6];   // + m_fData[13]*matrix[7];
		res[6] = m_fData[2]*matrix[4] + m_fData[6]*matrix[5] + m_fData[10]*matrix[6];  // + m_fData[14]*matrix[7];
		res[7] = m_fData[3]*matrix[4] + m_fData[7]*matrix[5] + m_fData[11]*matrix[6];  // + m_fData[15]*matrix[7];

		res[8] = m_fData[0]*matrix[8] + m_fData[4]*matrix[9] + m_fData[8]*matrix[10];    // + m_fData[12]*matrix[11];
		res[9] = m_fData[1]*matrix[8] + m_fData[5]*matrix[9] + m_fData[9]*matrix[10];	 // + m_fData[13]*matrix[11];
		res[10] = m_fData[2]*matrix[8] + m_fData[6]*matrix[9] + m_fData[10]*matrix[10];	 // + m_fData[14]*matrix[11];
		res[11] = m_fData[3]*matrix[8] + m_fData[7]*matrix[9] + m_fData[11]*matrix[10];	 // + m_fData[15]*matrix[11];

		res[12] = m_fData[0]*matrix[12] + m_fData[4]*matrix[13] + m_fData[8]*matrix[14] + m_fData[12];  // * matrix[15];
		res[13] = m_fData[1]*matrix[12] + m_fData[5]*matrix[13] + m_fData[9]*matrix[14] + m_fData[13];  // * matrix[15];
		res[14] = m_fData[2]*matrix[12] + m_fData[6]*matrix[13] + m_fData[10]*matrix[14] + m_fData[14]; // * matrix[15];
		res[15] = m_fData[3]*matrix[12] + m_fData[7]*matrix[13] + m_fData[11]*matrix[14] + m_fData[15]; // * matrix[15];

		return LXMatrix(res);
	}
}

const vec3f LXMatrix::operator*	(const vec3f& v) const
{
	vec3f r;
	r.x = m_fData[0] * v.x + m_fData[4] * v.y + m_fData[8] * v.z + m_fData[12];
	r.y = m_fData[1] * v.x + m_fData[5] * v.y + m_fData[9] * v.z + m_fData[13];
	r.z = m_fData[2] * v.x + m_fData[6] * v.y + m_fData[10] * v.z + m_fData[14];
	return r;
}

const LXBBox LXMatrix::operator* (const LXBBox& box) const
{
	CHK(box.IsValid());

	LXBBox r;
	
	vec3f vPoint[8];
	box.GetPoints(vPoint);

	for (uint i = 0; i < 8; i++)
	{
		LocalToParentPoint(vPoint[i]);
		r.Add(vPoint[i]);
	}

	return r;
}

LXMatrix& LXMatrix::operator*=	(const LXMatrix& matrix )
{
	LXMatrix matRes = operator*(matrix);
	operator=(matRes);
	return *this;
}

//--------------------------------------------------------------------------------
// Tests functions
//--------------------------------------------------------------------------------

bool LXMatrix::IsIdentity() const
{
	if ((m_fData[0] == 1.0f) &&
		(m_fData[1] == 0.0f) &&
		(m_fData[2] == 0.0f) &&
		(m_fData[3] == 0.0f) &&
		(m_fData[4] == 0.0f) &&
		(m_fData[5] == 1.0f) &&
		(m_fData[6] == 0.0f) &&
		(m_fData[7] == 0.0f) &&
		(m_fData[8] == 0.0f) &&
		(m_fData[9] == 0.0f) &&
		(m_fData[10] == 1.0f) &&
		(m_fData[11] == 0.0f) &&
		(m_fData[12] == 0.0f) &&
		(m_fData[13] == 0.0f) &&
		(m_fData[14] == 0.0f) &&
		(m_fData[15] == 1.0f))
		return true;
	else
		return false;
}

bool LXMatrix::IsRightHanded() const
{
	vec3f x,y,z,r;

	x.Set(m_fData[0], m_fData[1], m_fData[2]);
	y.Set(m_fData[4], m_fData[5], m_fData[6]);
	z.Set(m_fData[8], m_fData[9], m_fData[10]);

	r.CrossProduct(y, z);

	if ((r.DotProduct(x)) >= 0.0)
		return true;
	else
		return false;
}

//--------------------------------------------------------------------------------
// Set Transformation functions
//--------------------------------------------------------------------------------

void LXMatrix::Inverse()
{

#define a m_fData[0]
#define b m_fData[4]
#define c m_fData[8]
#define d m_fData[12]
#define e m_fData[1]
#define f m_fData[5]
#define g m_fData[9]
#define h m_fData[13]
#define i m_fData[2]
#define j m_fData[6]
#define k m_fData[10]
#define l m_fData[14]

	double t4,t6,t8,t10,t12,t14,t17,t20,t23,t24,t43,t46,t47,t51,t54,t57;
	double det;

	t4 = a*f;
	t6 = a*j;
	t8 = e*b;
	t10 = e*j;
	t12 = i*b;
	t14 = i*f;
	det = (t4*k-t6*g-t8*k+t10*c+t12*g-t14*c);
	CHK(det != 0.0);

	t17 = 1.0/det;
	t20 = j*c;
	t23 = b*g;
	t24 = f*c;
	t43 = i*c;
	t46 = a*g;
	t47 = e*c;
	t51 = a*h;
	t54 = e*d;
	t57 = i*d;

	LXMatrix inv;
	inv.m_fData[0 + 4 * 0] = float((f*k-j*g)*t17);
	inv.m_fData[0 + 4 * 1] = float(-(b*k-t20)*t17);
	inv.m_fData[0 + 4 * 2] = float((t23-t24)*t17);
	inv.m_fData[0 + 4 * 3] = float(-(t23*l-b*h*k-t24*l+f*d*k+t20*h-j*d*g)*t17);
	inv.m_fData[1 + 4 * 0] = float(-(e*k-i*g)*t17);
	inv.m_fData[1 + 4 * 1] = float((a*k-t43)*t17);
	inv.m_fData[1 + 4 * 2] = float(-(t46-t47)*t17);
	inv.m_fData[1 + 4 * 3] = float((t46*l-t51*k-t47*l+t54*k+t43*h-t57*g)*t17);
	inv.m_fData[2 + 4 * 0] = float(-(-t10+t14)*t17);
	inv.m_fData[2 + 4 * 1] = float(-(t6-t12)*t17);
	inv.m_fData[2 + 4 * 2] = float((t4-t8)*t17);
	inv.m_fData[2 + 4 * 3] = float((-t4*l+t51*j+t8*l-t54*j-t12*h+t57*f)*t17);
	inv.m_fData[3 + 4 * 0] = 0.0f;
	inv.m_fData[3 + 4 * 1] = 0.0f;
	inv.m_fData[3 + 4 * 2] = 0.0f;
	inv.m_fData[3 + 4 * 3] = 1.0f;

	*this = inv;

#undef a
#undef b
#undef c
#undef d
#undef e
#undef f
#undef g
#undef h
#undef i
#undef j
#undef k
#undef l
}

void LXMatrix::Inverse2()
{
	LXMatrix inv; 
	double det;
	int i;

	inv.m_fData[0] = m_fData[5]  * m_fData[10] * m_fData[15] - 
			 m_fData[5]  * m_fData[11] * m_fData[14] - 
			 m_fData[9]  * m_fData[6]  * m_fData[15] + 
			 m_fData[9]  * m_fData[7]  * m_fData[14] +
			 m_fData[13] * m_fData[6]  * m_fData[11] - 
			 m_fData[13] * m_fData[7]  * m_fData[10];

	inv.m_fData[4] = -m_fData[4]  * m_fData[10] * m_fData[15] + 
			  m_fData[4]  * m_fData[11] * m_fData[14] + 
			  m_fData[8]  * m_fData[6]  * m_fData[15] - 
			  m_fData[8]  * m_fData[7]  * m_fData[14] - 
			  m_fData[12] * m_fData[6]  * m_fData[11] + 
			  m_fData[12] * m_fData[7]  * m_fData[10];

	inv.m_fData[8] = m_fData[4]  * m_fData[9] * m_fData[15] - 
			 m_fData[4]  * m_fData[11] * m_fData[13] - 
			 m_fData[8]  * m_fData[5] * m_fData[15] + 
			 m_fData[8]  * m_fData[7] * m_fData[13] + 
			 m_fData[12] * m_fData[5] * m_fData[11] - 
			 m_fData[12] * m_fData[7] * m_fData[9];

	inv.m_fData[12] = -m_fData[4]  * m_fData[9] * m_fData[14] + 
			   m_fData[4]  * m_fData[10] * m_fData[13] +
			   m_fData[8]  * m_fData[5] * m_fData[14] - 
			   m_fData[8]  * m_fData[6] * m_fData[13] - 
			   m_fData[12] * m_fData[5] * m_fData[10] + 
			   m_fData[12] * m_fData[6] * m_fData[9];

	inv.m_fData[1] = -m_fData[1]  * m_fData[10] * m_fData[15] + 
			  m_fData[1]  * m_fData[11] * m_fData[14] + 
			  m_fData[9]  * m_fData[2] * m_fData[15] - 
			  m_fData[9]  * m_fData[3] * m_fData[14] - 
			  m_fData[13] * m_fData[2] * m_fData[11] + 
			  m_fData[13] * m_fData[3] * m_fData[10];

	inv.m_fData[5] = m_fData[0]  * m_fData[10] * m_fData[15] - 
			 m_fData[0]  * m_fData[11] * m_fData[14] - 
			 m_fData[8]  * m_fData[2] * m_fData[15] + 
			 m_fData[8]  * m_fData[3] * m_fData[14] + 
			 m_fData[12] * m_fData[2] * m_fData[11] - 
			 m_fData[12] * m_fData[3] * m_fData[10];

	inv.m_fData[9] = -m_fData[0]  * m_fData[9] * m_fData[15] + 
			  m_fData[0]  * m_fData[11] * m_fData[13] + 
			  m_fData[8]  * m_fData[1] * m_fData[15] - 
			  m_fData[8]  * m_fData[3] * m_fData[13] - 
			  m_fData[12] * m_fData[1] * m_fData[11] + 
			  m_fData[12] * m_fData[3] * m_fData[9];

	inv.m_fData[13] = m_fData[0]  * m_fData[9] * m_fData[14] - 
			  m_fData[0]  * m_fData[10] * m_fData[13] - 
			  m_fData[8]  * m_fData[1] * m_fData[14] + 
			  m_fData[8]  * m_fData[2] * m_fData[13] + 
			  m_fData[12] * m_fData[1] * m_fData[10] - 
			  m_fData[12] * m_fData[2] * m_fData[9];

	inv.m_fData[2] = m_fData[1]  * m_fData[6] * m_fData[15] - 
			 m_fData[1]  * m_fData[7] * m_fData[14] - 
			 m_fData[5]  * m_fData[2] * m_fData[15] + 
			 m_fData[5]  * m_fData[3] * m_fData[14] + 
			 m_fData[13] * m_fData[2] * m_fData[7] - 
			 m_fData[13] * m_fData[3] * m_fData[6];

	inv.m_fData[6] = -m_fData[0]  * m_fData[6] * m_fData[15] + 
			  m_fData[0]  * m_fData[7] * m_fData[14] + 
			  m_fData[4]  * m_fData[2] * m_fData[15] - 
			  m_fData[4]  * m_fData[3] * m_fData[14] - 
			  m_fData[12] * m_fData[2] * m_fData[7] + 
			  m_fData[12] * m_fData[3] * m_fData[6];

	inv.m_fData[10] = m_fData[0]  * m_fData[5] * m_fData[15] - 
			  m_fData[0]  * m_fData[7] * m_fData[13] - 
			  m_fData[4]  * m_fData[1] * m_fData[15] + 
			  m_fData[4]  * m_fData[3] * m_fData[13] + 
			  m_fData[12] * m_fData[1] * m_fData[7] - 
			  m_fData[12] * m_fData[3] * m_fData[5];

	inv.m_fData[14] = -m_fData[0]  * m_fData[5] * m_fData[14] + 
			   m_fData[0]  * m_fData[6] * m_fData[13] + 
			   m_fData[4]  * m_fData[1] * m_fData[14] - 
			   m_fData[4]  * m_fData[2] * m_fData[13] - 
			   m_fData[12] * m_fData[1] * m_fData[6] + 
			   m_fData[12] * m_fData[2] * m_fData[5];

	inv.m_fData[3] = -m_fData[1] * m_fData[6] * m_fData[11] + 
			  m_fData[1] * m_fData[7] * m_fData[10] + 
			  m_fData[5] * m_fData[2] * m_fData[11] - 
			  m_fData[5] * m_fData[3] * m_fData[10] - 
			  m_fData[9] * m_fData[2] * m_fData[7] + 
			  m_fData[9] * m_fData[3] * m_fData[6];

	inv.m_fData[7] = m_fData[0] * m_fData[6] * m_fData[11] - 
			 m_fData[0] * m_fData[7] * m_fData[10] - 
			 m_fData[4] * m_fData[2] * m_fData[11] + 
			 m_fData[4] * m_fData[3] * m_fData[10] + 
			 m_fData[8] * m_fData[2] * m_fData[7] - 
			 m_fData[8] * m_fData[3] * m_fData[6];

	inv.m_fData[11] = -m_fData[0] * m_fData[5] * m_fData[11] + 
			   m_fData[0] * m_fData[7] * m_fData[9] + 
			   m_fData[4] * m_fData[1] * m_fData[11] - 
			   m_fData[4] * m_fData[3] * m_fData[9] - 
			   m_fData[8] * m_fData[1] * m_fData[7] + 
			   m_fData[8] * m_fData[3] * m_fData[5];

	inv.m_fData[15] = m_fData[0] * m_fData[5] * m_fData[10] - 
			  m_fData[0] * m_fData[6] * m_fData[9] - 
			  m_fData[4] * m_fData[1] * m_fData[10] + 
			  m_fData[4] * m_fData[2] * m_fData[9] + 
			  m_fData[8] * m_fData[1] * m_fData[6] - 
			  m_fData[8] * m_fData[2] * m_fData[5];

	det = m_fData[0] * inv.m_fData[0] + m_fData[1] * inv.m_fData[4] + m_fData[2] * inv.m_fData[8] + m_fData[3] * inv.m_fData[12];

	// TODO: IsNull + CHK
	if (det == 0) 
		return;// false;

	det = 1.0 / det;

	for (i = 0; i < 16; i++)
		inv.m_fData[i] *= (float)det;

	*this = inv;
	
	return;// true;
}

void LXMatrix::SetIdentity()
{
	memset(m_fData, 0, sizeof(m_fData));
	m_fData[0] = m_fData[5] = m_fData[10] = m_fData[15] = 1.0f;
}

void LXMatrix::SetRotation(float phi, float u, float v, float w)
{
	float rcos = cosf(phi);
	float rsin = sinf(phi);
	m_fData[0] =      rcos + u*u*(1-rcos);
	m_fData[1] =  w * rsin + v*u*(1-rcos);
	m_fData[2] = -v * rsin + w*u*(1-rcos);
	m_fData[4] = -w * rsin + u*v*(1-rcos);
	m_fData[5] =      rcos + v*v*(1-rcos);
	m_fData[6] =  u * rsin + w*v*(1-rcos);
	m_fData[8] =  v * rsin + u*w*(1-rcos);
	m_fData[9] = -u * rsin + v*w*(1-rcos);
	m_fData[10] =      rcos + w*w*(1-rcos);
}

vec3f LXMatrix::GetEulerAngles() const
{
	float fAngleY = -asinf(m_fData[2]);
	float C = cosf(fAngleY);

	float fAngleX;
	float fAngleZ;

	if (fabsf(C) > 0.005)
	{
		float tx = m_fData[10] / C;
		float ty = -m_fData[6] / C;
		fAngleX = atan2f(ty, tx);
		tx = m_fData[0] / C;
		ty = -m_fData[1] / C;
		fAngleZ = atan2f(ty, tx);
	}
	else
	{
		fAngleX = 0.0f;
		float tx = m_fData[5];
		float ty = m_fData[4];
		fAngleZ = atan2(ty, tx);
	}

	return vec3f(LX_RADTODEG(fAngleX), LX_RADTODEG(fAngleY), LX_RADTODEG(fAngleZ));
}

//! Pitch Yaw Roll are (Y, Z, X)
void LXMatrix::SetRotation(float x, float y, float z)
{
	x = LX_DEGTORAD(x);
	y = LX_DEGTORAD(y);
	z = LX_DEGTORAD(z);

	float CosX = cosf(x);
	float SinX = sinf(x);
	
	float CosY = cosf(y);
	float SinY = sinf(y);
	
	float CosZ = cosf(z);
	float SinZ = sinf(z);

	float AD = CosX * SinY;
	float BD = SinX * SinY;

	m_fData[0] = CosY * CosZ;
	m_fData[1] = -CosY * SinZ;
	m_fData[2] = -SinY;
	m_fData[3] = 0.f;
	
	m_fData[4] = -BD * CosZ + CosX * SinZ;
	m_fData[5] =  BD * SinZ + CosX * CosZ;
	m_fData[6] = -SinX * CosY;
	m_fData[7] = 0.f;
		
	m_fData[8] =  AD * CosZ + SinX * SinZ;
	m_fData[9] = -AD * SinZ + SinX * CosZ;
	m_fData[10] = CosX * CosY;
	m_fData[11] = 0.f;

	m_fData[12] = 0.f;
	m_fData[13] = 0.f;
	m_fData[14] = 0.f;
	m_fData[15] = 1.f;
}

void LXMatrix::SetTranslation ( float x, float y, float z )
{
	m_fData[12] = x;
	m_fData[13] = y;
	m_fData[14] = z;
}

void LXMatrix::SetScale( float x, float y, float z )
{
	CHK(abs(x) > LX_SMALL_NUMBER6);
	CHK(abs(y) > LX_SMALL_NUMBER6);
	CHK(abs(z) > LX_SMALL_NUMBER6);

	m_fData[0] = x;
	m_fData[5] = y;
	m_fData[10] = z;
}

void LXMatrix::Rotate( float fAngle, const vec3f& vAxis)
{
	LXMatrix matRot;
	matRot.SetRotation(fAngle, vAxis.x, vAxis.y, vAxis.z);

	if (IsIdentity())
		operator=(matRot);
	else
		operator*=(matRot);

	CHK(::IsValid(*this));
}

void LXMatrix::RotateX(float phi)
{
	float a1[4],a2[4];
	float SinPhi = sinf(phi);
	float CosPhi = cosf(phi);

	a1[0] = m_fData[1];
	a1[1] = m_fData[5];
	a1[2] = m_fData[9];
	a1[3] = m_fData[13];

	a2[0] = m_fData[2];
	a2[1] = m_fData[6];
	a2[2] = m_fData[10];
	a2[3] = m_fData[14];

	m_fData[1]=CosPhi*a1[0]+SinPhi*a2[0];
	m_fData[5]=CosPhi*a1[1]+SinPhi*a2[1];
	m_fData[9]=CosPhi*a1[2]+SinPhi*a2[2];
	m_fData[13]=CosPhi*a1[3]+SinPhi*a2[3];

	m_fData[2]=-SinPhi*a1[0]+CosPhi*a2[0];
	m_fData[6]=-SinPhi*a1[1]+CosPhi*a2[1];
	m_fData[10]=-SinPhi*a1[2]+CosPhi*a2[2];
	m_fData[14]=-SinPhi*a1[3]+CosPhi*a2[3];

	CHK(IsValid(*this));
}

void LXMatrix::RotateY(float phi)
{
	float a0[4],a2[4];
	float SinPhi=sinf(phi);
	float CosPhi=cosf(phi);

	a0[0] = m_fData[0];
	a0[1] = m_fData[4];
	a0[2] = m_fData[8];
	a0[3] = m_fData[12];

	a2[0] = m_fData[2];
	a2[1] = m_fData[6];
	a2[2] = m_fData[10];
	a2[3] = m_fData[14];

	m_fData[0]=CosPhi*a0[0]-SinPhi*a2[0];
	m_fData[4]=CosPhi*a0[1]-SinPhi*a2[1];
	m_fData[8]=CosPhi*a0[2]-SinPhi*a2[2];
	m_fData[12]=CosPhi*a0[3]-SinPhi*a2[3];

	m_fData[2]=SinPhi*a0[0]+CosPhi*a2[0];
	m_fData[6]=SinPhi*a0[1]+CosPhi*a2[1];
	m_fData[10]=SinPhi*a0[2]+CosPhi*a2[2];
	m_fData[14]=SinPhi*a0[3]+CosPhi*a2[3];

	CHK(IsValid(*this));
}

void LXMatrix::RotateZ(float phi)
{
	float a0[4],a1[4];
	float SinPhi = sinf(phi);
	float CosPhi = cosf(phi);

	a0[0] = m_fData[0];
	a0[1] = m_fData[4];
	a0[2] = m_fData[8];
	a0[3] = m_fData[12];

	a1[0] = m_fData[1];
	a1[1] = m_fData[5];
	a1[2] = m_fData[9];
	a1[3] = m_fData[13];

	m_fData[0]=CosPhi*a0[0]+SinPhi*a1[0];
	m_fData[4]=CosPhi*a0[1]+SinPhi*a1[1];
	m_fData[8]=CosPhi*a0[2]+SinPhi*a1[2];
	m_fData[12]=CosPhi*a0[3]+SinPhi*a1[3];

	m_fData[1]=-SinPhi*a0[0]+CosPhi*a1[0];
	m_fData[5]=-SinPhi*a0[1]+CosPhi*a1[1];
	m_fData[9]=-SinPhi*a0[2]+CosPhi*a1[2];
	m_fData[13]=-SinPhi*a0[3]+CosPhi*a1[3];

	CHK(IsValid(*this));
}

void LXMatrix::SetOrigin( const vec3f& vo )
{
	CHK(IsValid(vo));
	m_fData[12] = vo.x;
	m_fData[13] = vo.y;
	m_fData[14] = vo.z;
}

void LXMatrix::SetVz( const vec3f& v, bool bDoOrthonormal  )
{
	if (bDoOrthonormal)
	{
		vec3f vz = Normalize(v);
		
		vec3f vx = CrossProduct(LX_VEC3F_Y, vz);
		vx.Normalize();

		vec3f vy = CrossProduct(vz, vx);
		vy.Normalize();

		vz = CrossProduct(vx, vy);
		vz.Normalize();

		m_fData[0] = vx.x;
		m_fData[1] = vx.y;
		m_fData[2] = vx.z;
		
		m_fData[4] = vy.x;
		m_fData[5] = vy.y;
		m_fData[6] = vy.z;
		
		m_fData[8] = vz.x;
		m_fData[9] = vz.y;
		m_fData[10] = vz.z;
	}
	else
	{
		m_fData[8] = v.x;
		m_fData[9] = v.y;
		m_fData[10] = v.z;
	}

	CHK(IsValid(*this));
}

 void LXMatrix::SetXYZ( const vec3f& vx, const vec3f& vy, const vec3f& vz)
 {
	 m_fData[0] = vx.x;
	 m_fData[1] = vx.y;
	 m_fData[2] = vx.z;
	 
	 m_fData[4] = vy.x;
	 m_fData[5] = vy.y;
	 m_fData[6] = vy.z;
	 
	 m_fData[8] = vz.x;
	 m_fData[9] = vz.y;
	 m_fData[10] = vz.z;
}

vec4f LXMatrix::GetRow( unsigned int i ) const
{
	i*=4;
	return vec4f(m_fData[i], m_fData[i+1], m_fData[i+2], m_fData[i+3]);
}

vec3f LXMatrix::GetScale( ) const
{
	vec4f vx = GetRow(0);
	vec4f vy = GetRow(1);
	vec4f vz = GetRow(2);
	return vec3f( vx.Length(), vy.Length(), vz.Length());
}

void LXMatrix::LocalToParentPoint(vec3f& point) const
{
	vec3f vTemp;
	vTemp.x = m_fData[0] * point.x + m_fData[4] * point.y + m_fData[8] *  point.z + m_fData[12];
	vTemp.y = m_fData[1] * point.x + m_fData[5] * point.y + m_fData[9] *  point.z + m_fData[13];
	vTemp.z = m_fData[2] * point.x + m_fData[6] * point.y + m_fData[10] * point.z + m_fData[14];
	point = vTemp;
	CHK(IsValid(point));
}

void LXMatrix::LocalToParentPoint(vec4f& point) const
{
	vec4f vTemp;
	vTemp.x = m_fData[0] * point.x + m_fData[4] * point.y + m_fData[8] *  point.z + m_fData[12] * point.w;
	vTemp.y = m_fData[1] * point.x + m_fData[5] * point.y + m_fData[9] *  point.z + m_fData[13] * point.w;
	vTemp.z = m_fData[2] * point.x + m_fData[6] * point.y + m_fData[10] * point.z + m_fData[14] * point.w;
	vTemp.w = m_fData[3] * point.x + m_fData[7] * point.y + m_fData[11] * point.z + m_fData[15] * point.w;
	point = vTemp;
}

void LXMatrix::LocalToParentVector(vec3f& vector) const
{
	vec3f vTemp;
	vTemp.x = m_fData[0] * vector.x + m_fData[4] * vector.y + m_fData[8] *  vector.z;
	vTemp.y = m_fData[1] * vector.x + m_fData[5] * vector.y + m_fData[9] *  vector.z;
	vTemp.z = m_fData[2] * vector.x + m_fData[6] * vector.y + m_fData[10] * vector.z;
	vector = vTemp;
}

void LXMatrix::LocalToParent(LXBBox& bbox) const 
{
	if (!bbox.IsValid())
		return;

	vec3f vPoint[8];
	bbox.GetPoints(vPoint);
	bbox.Reset();
	for (uint i = 0; i < 8; i++)
	{
		LocalToParentPoint(vPoint[i]);
		bbox.Add(vPoint[i]);
	}
}

void LXMatrix::ParentToLocal(LXBBox& bbox) const 
{
	if (!bbox.IsValid())
		return;

	vec3f vPoint[8];
	bbox.GetPoints(vPoint);
	bbox.Reset();
	for (uint i = 0; i < 8; i++)
	{
		ParentToLocalPoint(vPoint[i]);
		bbox.Add(vPoint[i]);
	}
}

void LXMatrix::ParentToLocalPoint(vec3f& point) const
{
	LXMatrix matInv = *this;
	matInv.Inverse();
	matInv.LocalToParentPoint(point);
}

void LXMatrix::ParentToLocalPoint(vec4f& point) const
{
	LXMatrix matInv = *this;
	matInv.Inverse2();
	matInv.LocalToParentPoint(point);
}

void LXMatrix::ParentToLocal(LXMatrix& Matrix) const
{
	LXMatrix matInv = *this;
	matInv.Inverse2();
	LXMatrix TmpMatrix = Matrix * matInv;
	Matrix = TmpMatrix;
}

void LXMatrix::ParentToLocalVector(vec3f& vector) const
{
	LXMatrix matInv(*this);
	matInv.Inverse();
	matInv.LocalToParentVector(vector);
}

void LXMatrix::ParentToLocal(LXAxis& axis) const
{
	ParentToLocalVector(const_cast<vec3f&>(axis.GetVector()));
	ParentToLocalPoint(const_cast<vec3f&>(axis.GetOrigin()));
}

void LXMatrix::LocalToParent(LXAxis& axis) const
{
	LocalToParentVector(const_cast<vec3f&>(axis.GetVector()));
	LocalToParentPoint(const_cast<vec3f&>(axis.GetOrigin()));
}

void LXMatrix::SetPerspectiveLH(float fovy, float aspect, float zNear, float zFar)
{
	float f = 1.0f/tanf(LX_DEGTORAD(fovy)*0.5f);

	m_fData[0] = f/aspect;
	m_fData[1] = 0.0f;
	m_fData[2] = 0.0f;
	m_fData[3] = 0.0f;
	
	m_fData[4] = 0.0f;
	m_fData[5] = f;
	m_fData[6] = 0.0f;
	m_fData[7] = 0.0f;

	m_fData[8] = 0.0f;
	m_fData[9] = 0.0f;
	m_fData[10] = -(zFar + zNear) / (zNear - zFar);
	m_fData[11] = 1.0f;
	
	m_fData[12] = 0.0f;
	m_fData[13] = 0.0f;
	m_fData[14] = (2.0f * zFar *zNear) / (zNear - zFar);
	m_fData[15] = 0.0f;
}

void LXMatrix::SetPerspectiveRH(float fovy, float aspect, float zNear, float zFar)
{
	float f = 1.0f / tanf(LX_DEGTORAD(fovy)*0.5f);

	m_fData[0] = f / aspect;
	m_fData[1] = 0.0f;
	m_fData[2] = 0.0f;
	m_fData[3] = 0.0f;

	m_fData[4] = 0.0f;
	m_fData[5] = f;
	m_fData[6] = 0.0f;
	m_fData[7] = 0.0f;

	m_fData[8] = 0.0f;
	m_fData[9] = 0.0f;
	m_fData[10] = (zFar + zNear) / (zNear - zFar);
	m_fData[11] = -1.0f;
	
	m_fData[12] = 0.0f;
	m_fData[13] = 0.0f;
	m_fData[14] = (2.0f * zFar * zNear) / (zNear - zFar);
	m_fData[15] = 0.0f;
}

//
// http://stackoverflow.com/questions/12965161/rendering-infinitely-large-plane
//

void LXMatrix::BuildInfinitePerspectiveLH(float fovy, float aspect, float zNear)
{
	float r = tanf(LX_DEGTORAD(fovy * aspect) * 0.5f);
	float t = tanf(LX_DEGTORAD(fovy) * 0.5f);

	float f = 1.0f / tanf(LX_DEGTORAD(fovy)* 0.5f);
	float zFar = 100000.0;

	m_fData[0] = f / aspect;
	m_fData[1] = 0.0f;
	m_fData[2] = 0.0f;
	m_fData[3] = 0.0f;

	m_fData[4] = 0.0f;
	m_fData[5] = f;
	m_fData[6] = 0.0f;
	m_fData[7] = 0.0f;

	m_fData[8] = 0.0f;
	m_fData[9] = 0.0f;
	m_fData[10] = 1.0f;
	m_fData[11] = 1.0f;

	m_fData[12] = 0.0f;
	m_fData[13] = 0.0f;
	m_fData[14] = -zNear;
	m_fData[15] = 0.0f;
}

void LXMatrix::BuildInfinitePerspectiveRH(float fovy, float aspect, float zNear)
{
	CHK(0 && L"Bad, see LH version");
	float r = tanf(LX_DEGTORAD(fovy * aspect) * 0.5f);
	float t = tanf(LX_DEGTORAD(fovy) * 0.5f);

	m_fData[0] = zNear / r;
	m_fData[1] = 0.0f;
	m_fData[2] = 0.0f;
	m_fData[3] = 0.0f;

	m_fData[4] = 0.0f;
	m_fData[5] = zNear / t;
	m_fData[6] = 0.0f;
	m_fData[7] = 0.0f;

	m_fData[8] = 0.0f;
	m_fData[9] = 0.0f;
	m_fData[10] = -1.0f;
	m_fData[11] = -2.0f * zNear;

	m_fData[12] = 0.0f;
	m_fData[13] = 0.0f;
	m_fData[14] = -1.0f;
	m_fData[15] = 0.0f;
}

void LXMatrix::SetOrtho ( float left, float right, float bottom, float top, float fNear, float fFar )
{
	float tx = -((right+left)/(right-left));
	float ty = -((top+bottom)/(top-bottom));
	float tz = -((fFar+fNear)/(fFar-fNear));

	m_fData[0] = 2.0f/(right-left);
	m_fData[1] = 0.0f;
	m_fData[2] = 0.0f;
	m_fData[3] = 0.0f;
	
	m_fData[4] = 0.0f;
	m_fData[5] = 2.0f/(top-bottom);
	m_fData[6] = 0.0f;
	m_fData[7] = 0.0f;

	m_fData[8] = 0.0f;
	m_fData[9] = 0.0f;
	m_fData[10] = (-2.0f)/(fFar-fNear);
	m_fData[11] = 0.0f;

	m_fData[12] = tx;
	m_fData[13] = ty;
	m_fData[14] = tz;
	m_fData[15] = 1.0f;
}

LXMatrix Inverse(const LXMatrix& matrix)
{
	LXMatrix m = matrix;
	m.Inverse();
	return m;
}

LXMatrix Transpose(const LXMatrix& mat)
{
	LXMatrix mOut;
	
	// diagonal first
	mOut.m_44[0][0] = mat.m_44[0][0];
	mOut.m_44[1][1] = mat.m_44[1][1];
	mOut.m_44[2][2] = mat.m_44[2][2];
	mOut.m_44[3][3] = mat.m_44[3][3];

	// then upper triangle
	float tmp;
	tmp = mat.m_44[0][1]; mOut.m_44[0][1] = mat.m_44[1][0]; mOut.m_44[1][0] = tmp;
	tmp = mat.m_44[0][2]; mOut.m_44[0][2] = mat.m_44[2][0]; mOut.m_44[2][0] = tmp;
	tmp = mat.m_44[0][3]; mOut.m_44[0][3] = mat.m_44[3][0]; mOut.m_44[3][0] = tmp;
	tmp = mat.m_44[1][2]; mOut.m_44[1][2] = mat.m_44[2][1]; mOut.m_44[2][1] = tmp;
	tmp = mat.m_44[1][3]; mOut.m_44[1][3] = mat.m_44[3][1]; mOut.m_44[3][1] = tmp;
	tmp = mat.m_44[2][3]; mOut.m_44[2][3] = mat.m_44[3][2]; mOut.m_44[3][2] = tmp;

	return mOut;
}

void LXMatrix::FlipHand()
{
	CHK(0 && L"NO TESTED");
	float m01 = m_fData[1];
	float m02 = m_fData[2];

	float m10 = m_fData[4];
	float m11 = m_fData[5];
	float m12 = m_fData[6];

	float m20 = m_fData[8];
	float m21 = m_fData[9];
	float m22 = m_fData[10];

	float m31 = m_fData[13];
	float m32 = m_fData[14];

	m_fData[1] = m02;
	m_fData[2] = m01;

	m_fData[4] = m20;
	m_fData[5] = m22;
	m_fData[6] = m21;

	m_fData[8] = m10;
	m_fData[9] = m12;
	m_fData[10] = m11;

	m_fData[13] = m32;
	m_fData[14] = m31;
}