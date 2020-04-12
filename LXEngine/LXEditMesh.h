//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include "LXFace.h"
#include "LXMaterial.h"
#include "LXPrimitive.h"

class LXCORE_API LXEditFace : public LXObject
{
public:
	virtual ~LXEditFace() {};
	CFace3* m_pVFace;
	CFace3* m_pTFaces;
};

struct CIndexVTN
{
	CIndexVTN(uint nV, uint nN, uint nT){v=nV;n=nN;t=nT;}
	uint v;
	uint n;
	uint t;
};

//--------------------------------------------------------------------------------
// LXEditMesh : MultiIndexed
//--------------------------------------------------------------------------------

class LXCORE_API LXEditMesh : public LXObject
{

public:

	LXEditMesh(void);
	virtual ~LXEditMesh(void);

	void                    ComputeNormals( bool bFlipped );
	LXPrimitive*			CreateMonoIndexedVertexArray( );
	bool					CreateMonoIndexedVertexArray2( ListPrimitives& listGeometries );

private:

	bool					FinalizeGeometry( LXPrimitive* pGeometry, vector<CIndexVTN>& arrayIndexedVectors );

public:

	ArrayVec3ui				m_PositionIndices;		
	CFace3*                 m_pTFaces;				// TexCoords indices
	CFace3*                 m_pNFaces;				// Normals indices
	shared_ptr<LXMaterial>* m_ppMaterials;

	ArrayVec3f              m_arrayPositions;
	ArrayVec2f 				m_arrayTexCoords;
	ArrayVec3f              m_arrayNormals;
};

//--------------------------------------------------------------------------------
// LXEditMesh2 & LXPrimitive : MultiIndexed + MultiPrimitive
//--------------------------------------------------------------------------------

/*
class LXPrimitive2 : public LXObject
{

public:

	LXPrimitive2(void){ };
	virtual ~LXPrimitive2(void) { };

	ArrayUint				m_PositionIndices;
	ArrayUint				m_TexCoordIndices;
	ArrayUint				m_NormalIndices;

};

typedef vector<LXPrimitive2> ArrayPrimitives;

class LXEditMesh2 : public LXObject
{

public:

	LXEditMesh2(void) { };
	virtual ~LXEditMesh2(void) { };

	ArrayVec3f              m_arrayPositions;
	ArrayVec2f 				m_arrayTexCoords;
	ArrayVec3f              m_arrayNormals;

	ArrayPrimitives			m_arrayPrimitives;

};
*/