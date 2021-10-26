//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXMeshTopology.h"

LXActorMeshTopology::LXActorMeshTopology()
{
}

LXActorMeshTopology::~LXActorMeshTopology()
{
}

LXTopoVertex* CreateTopoVertices(const ArrayVec3ui& pFaces, uint nFaces, uint nVertices)
{
  CHK(pFaces.size());
  CHK(nFaces);
  CHK(nVertices);
  if (!pFaces.size()) return NULL;
  if (!nFaces) return NULL;
  if (!nVertices) return NULL;

  LXTopoVertex* pTopoVertex = new LXTopoVertex[nVertices];
  
  for (uint i=0; i<nFaces; i++)
  {
	CFace3* pFace = (CFace3*)&pFaces[i];
	uint a = pFaces[i][0];
	uint b = pFaces[i][1];
	uint c = pFaces[i][2];
	CHK((a>=0) && (a<nVertices));
	CHK((b>=0) && (b<nVertices));
	CHK((c>=0) && (c<nVertices));
	pTopoVertex[a].m_listFaces.push_back(pFace);
	pTopoVertex[b].m_listFaces.push_back(pFace);
	pTopoVertex[c].m_listFaces.push_back(pFace);
  }
  return pTopoVertex;
}

bool  CompareIndexedVerticesByXYZ(LXIndexedVertex* p1, LXIndexedVertex* p2) 
{
  if ((p1->pVec3f->x <  p2->pVec3f->x) ||
	 ((p1->pVec3f->x == p2->pVec3f->x) && (p1->pVec3f->y <  p2->pVec3f->y)) ||
	 ((p1->pVec3f->x == p2->pVec3f->x) && (p1->pVec3f->y == p2->pVec3f->y) && (p1->pVec3f->z < p2->pVec3f->z)) )
  {
	return true;
  }
  return false; 
}

bool  CompareIndexedVertexByCurrentIndex(LXIndexedVertex* p1, LXIndexedVertex* p2)			
{ 
  return p1->nCurrentIndex < p2->nCurrentIndex;	
}

struct CIndexedVector
{
  vec3f*  pVec;
  uint		nOldIndex; 
	uint		nNewIndex; 
};

bool  _CompareVectorByXYZ(CIndexedVector* iv1, CIndexedVector* iv2) 
{
  if ((iv1->pVec->x <  iv2->pVec->x) ||
	 ((iv1->pVec->x == iv2->pVec->x) && (iv1->pVec->y <  iv2->pVec->y)) ||
	 ((iv1->pVec->x == iv2->pVec->x) && (iv1->pVec->y == iv2->pVec->y) && (iv1->pVec->z < iv2->pVec->z)) )
	return true;
  return false; 
}

bool  _CompareVectorByOldIndex(CIndexedVector* iv1, CIndexedVector* iv2)			
{ 
  return iv1->nOldIndex < iv2->nOldIndex;	
}

void CreateNormalIndexation(const vec3f* pNormals, uint nFaces, CFace3** ppNFaces, ArrayVec3f& arrayNewNormals)
{
	CHK(pNormals);
	CHK(nFaces);

	uint nNormals = nFaces * 3;

	//
	// Fill indexed vector array, needed to sort vectors
	//

	std::vector<CIndexedVector*> arrayVectors;
	arrayVectors.reserve(nNormals);
	for (uint i=0; i<nNormals; i++)
	{
		CIndexedVector* p = new CIndexedVector;
		p->nOldIndex = i; 
		p->nNewIndex = i;
		p->pVec = (vec3f*)&pNormals[i];
		arrayVectors.push_back(p);											
	}
	sort(arrayVectors.begin(), arrayVectors.end(), _CompareVectorByXYZ);

	//
	// Count and fill new indices
	//

	uint cpt = 0;
	CIndexedVector* pPrev = NULL;
	std::vector<CIndexedVector*>::iterator It;
	for (It=arrayVectors.begin(); It<arrayVectors.end(); It++)
	{
		if (pPrev)
		{
			pPrev->nNewIndex = cpt;
			if (pPrev->pVec->x != (*It)->pVec->x)
				cpt++;
			else if (pPrev->pVec->y != (*It)->pVec->y)
				cpt++;
			else if (pPrev->pVec->z != (*It)->pVec->z)
				cpt++;
		}
		pPrev = (*It);
	}
	pPrev->nNewIndex = cpt; // Last
	cpt++;

	//
	// Restore initial order
	//

	sort(arrayVectors.begin(), arrayVectors.end(), _CompareVectorByOldIndex);

	//
	// Create or fill new indice array
	// TODO: re-index existing

	CFace3* pNFaces = new CFace3[nFaces];
	for (uint i=0; i<nFaces; i++)
	{
		pNFaces[i].x = arrayVectors[i*3]->nNewIndex; 
		pNFaces[i].y = arrayVectors[i*3+1]->nNewIndex; 
		pNFaces[i].z = arrayVectors[i*3+2]->nNewIndex;
	}

	//
	// Create new vector array
	//

	vec3f* pNewNormals = new vec3f[cpt];
	for (It=arrayVectors.begin(); It<arrayVectors.end(); It++)
		pNewNormals[(*It)->nNewIndex] = pNormals[(*It)->nOldIndex];

	//
	// Release indexed vector
	//

	for (It=arrayVectors.begin(); It<arrayVectors.end(); It++)
		delete (*It);

	//
	// Set output
	//

	*ppNFaces     = pNFaces;     // Indices array

	arrayNewNormals.reserve(cpt);
	for (uint i=0;i<cpt;i++)
		arrayNewNormals.push_back(pNewNormals[i]);

	delete [] pNewNormals;
}
