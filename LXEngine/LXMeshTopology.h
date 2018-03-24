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
#include "LXVec3.h"

class LXCORE_API LXTopoVertex : public LXObject
{
public:
	virtual ~LXTopoVertex() {}
	ListFaces m_listFaces;
};

class LXIndexedVertex
{
public:
	vec3f*  pVec3f;
	uint		nCurrentIndex; 
	uint		nNewIndex; 
};

class LXActorMeshTopology : public LXObject
{

public:

	LXActorMeshTopology(void);
	virtual ~LXActorMeshTopology(void);

};

//--------------------------------------------------------------------------------
// Create list of adjacent faces per vertex
//--------------------------------------------------------------------------------
LXTopoVertex* CreateTopoVertices(const ArrayVec3ui& pFaces, uint nFaces, uint nVertices);

//--------------------------------------------------------------------------------
// Create vertex normal indexation and corresponding normal array
// TODO : UpdateNormalIndexation, same thing but respect existing indices
//--------------------------------------------------------------------------------
void CreateNormalIndexation ( const vec3f* pNormals, uint nFaces, CFace3** ppNFaces, ArrayVec3f& arrayNewNormals );

