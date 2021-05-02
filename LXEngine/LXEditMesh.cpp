//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXEditMesh.h"
#include "LXMeshTopology.h"
#include "LXCore.h"
#include "LXMath.h"
#include "LXMemory.h" // --- Must be the last included ---

LXEditMesh::LXEditMesh(void):
m_pTFaces(NULL),
m_pNFaces(NULL),
m_ppMaterials(NULL)
{
}

LXEditMesh::~LXEditMesh(void)
{
	LX_SAFE_DELETE_ARRAY(m_pTFaces);
	LX_SAFE_DELETE_ARRAY(m_pNFaces);
	LX_SAFE_DELETE_ARRAY(m_ppMaterials);
// 
// 	for (ArrayVec3f::iterator It = m_arrayNormals.begin(); It != m_arrayNormals.end(); It++)
// 	{
// 		delete *It;
// 	}
}

//--------------------------------------------------------------------------------
// Compute and build minimal vertex normals array with indice array
// Output : m_pNFaces, m_arrayNormals, m_nNormals
//--------------------------------------------------------------------------------
void LXEditMesh::ComputeNormals( bool bFlipped )
{
	CHK(m_PositionIndices.size());
	CHK(m_arrayPositions.size());

	CHK(!m_pNFaces);
	CHK(!m_arrayNormals.size());

	uint nFaces = (uint)m_PositionIndices.size();
	
	// --- Face normals ---
	vec3f* pFaceNormals = new vec3f[nFaces];
	for (uint i=0; i<nFaces; i++)
	{
		int a = m_PositionIndices[i][0];
		int b,c;

		if (bFlipped)
		{
			b = m_PositionIndices[i][2];
			c = m_PositionIndices[i][1];
		}
		else
		{
			b = m_PositionIndices[i][1];
			c = m_PositionIndices[i][2];
		}

		if(1)//!GetCore().GetSSE())
		{
			vec3f v0 = m_arrayPositions[a];
			vec3f v1 = m_arrayPositions[b];
			vec3f v2 = m_arrayPositions[c];
			vec3f v0v1 = v1 - v0;
			vec3f v0v2 = v2 - v0;
			pFaceNormals[i].CrossProduct(v0v1, v0v2);
			if (pFaceNormals[i].IsNull()) // cas triangle plat : ligne
				pFaceNormals[i] = LX_VEC3F_Z;
		}
		else
		{
			/*
			// TEST MMX SSE
			__declspec(align(16))vec4f v0 = m_arrayPositions[a];
			__declspec(align(16))vec4f v1 = m_arrayPositions[b];
			__declspec(align(16))vec4f v2 = m_arrayPositions[c];
			__declspec(align(16))vec4f v0v1;
			__declspec(align(16))vec4f v0v2;

			__m128* pv0 = (__m128*)&v0;
			__m128* pv1 = (__m128*)&v1;
			__m128* pv2 = (__m128*)&v2;
			__m128* pv0v1 = (__m128*)&v0v1;
			__m128* pv0v2 = (__m128*)&v0v2;

			*pv0v1 = _mm_sub_ps(*pv1, *pv0);
			*pv0v2 = _mm_sub_ps(*pv2, *pv0);

			pFaceNormals[i].CrossProduct(v0v1, v0v2);
			// END
			*/
		}
				
		pFaceNormals[i].Normalize();
	}
	// ------


	// --- Unindexed vertex normals ---
	vec3f* pVNormals = new vec3f[nFaces * 3];
	
	//ZeroMemory(pVNormals, sizeof(vec3f) * m_nFaces * 3);
	for (uint i = 0; i< nFaces * 3; i++)
		pVNormals[i].Set(0.0f, 0.0f, 0.0f);
	

	// hmmm ?
	/*
	for (uint i=0; i<m_nFaces; i++)
	{
	int a = m_PositionIndices[i][0];
	int b = m_PositionIndices[i][1];
	int c = m_PositionIndices[i][2];

	pVNormals[i*3]   = vec3f(0.0f, 0.0f, 0.0f);//pFaceNormals[i];
	pVNormals[i*3+1] = vec3f(0.0f, 0.0f, 0.0f);//pFaceNormals[i];
	pVNormals[i*3+2] = vec3f(0.0f, 0.0f, 0.0f);//pFaceNormals[i];
	}
	*/

	LXTopoVertex* pTopoVertices = CreateTopoVertices(m_PositionIndices, nFaces, (uint)m_arrayPositions.size());

	for (uint i=0; i<nFaces; i++) // For eatch face
	{
		for (uint j=0; j<3; j++) // for each face indices
		{
			//CFace3* pFace1 = &m_PositionIndices[i];
			vec3ui* pFace1 = &m_PositionIndices[i];

			uint k = (*pFace1)[j];

			ListFaces::iterator It;
			for (It=pTopoVertices[k].m_listFaces.begin(); It!=pTopoVertices[k].m_listFaces.end(); It++) // for each adjacency face
			{
				//CFace3* pFace2 = (*It);
				vec3ui* pFace2 = (*It);
								
				uint l = uint(pFace2 - &*m_PositionIndices.begin()); // Get face id;
				

				//if (i!=j) // hmmmm ?
				if (pFaceNormals[i].DotProduct(pFaceNormals[l]) > cosf(55.0f * (float)LX_PI / 180.0f))
				{
					pVNormals[i*3+j] += pFaceNormals[l];
				}
			}
		}
	}

	LX_SAFE_DELETE_ARRAY(pFaceNormals);


	for (uint i=0; i<nFaces; i++)
	{
		pVNormals[i*3].Normalize();
		pVNormals[i*3+1].Normalize();
		pVNormals[i*3+2].Normalize();
	}

	LX_SAFE_DELETE_ARRAY(pTopoVertices);

	CreateNormalIndexation(pVNormals, nFaces, &m_pNFaces, m_arrayNormals);

	LX_SAFE_DELETE_ARRAY(pVNormals);
}

/*
void LXEditMesh::Render()
{
	CHK(0);

	glBegin(GL_TRIANGLES);
	for(uint i=0; i<m_nFaces; i++)
	{
		vec3f va = m_arrayPositions[m_PositionIndices[i][0]];
		vec3f vb = m_arrayPositions[m_PositionIndices[i][1]];
		vec3f vc = m_arrayPositions[m_PositionIndices[i][2]];

		vec3f na = m_arrayNormals[m_pNFaces[i][0]];
		vec3f nb = m_arrayNormals[m_pNFaces[i][1]];
		vec3f nc = m_arrayNormals[m_pNFaces[i][2]];

		glNormal3f(na.x, na.y, na.z);
		//glTexCoord2f(ta.x, ta.y);
		glVertex3f(va.x, va.y, va.z);

		glNormal3f(nb.x, nb.y, nb.z);
		//glTexCoord2f(tb.x, tb.y);
		glVertex3f(vb.x, vb.y, vb.z);

		glNormal3f(nc.x, nc.y, nc.z);
		//glTexCoord2f(tc.x, tc.y);
		glVertex3f(vc.x, vc.y, vc.z);
	}
	glEnd();

	/*
	glBegin(GL_LINES);
	for(uint i=0; i<m_nFaces; i++)
	{
	vec3f va = m_arrayPositions[m_PositionIndices[i][0]];
	vec3f vb = m_arrayPositions[m_PositionIndices[i][1]];
	vec3f vc = m_arrayPositions[m_PositionIndices[i][2]];

	vec3f v2 = va + m_arrayNormals[i*3] * 5;
	glVertex3f(va.x, va.y, va.z);
	glVertex3f(v2.x, v2.y, v2.z);

	v2 = vb + m_arrayNormals[i*3+1] * 5;
	glVertex3f(vb.x, vb.y, vb.z);
	glVertex3f(v2.x, v2.y, v2.z);

	v2 = vc + m_arrayNormals[i*3+2] * 5;   
	glVertex3f(vc.x, vc.y, vc.z);
	glVertex3f(v2.x, v2.y, v2.z);
	}
	glEnd();
	*/
/*
}
*/

LXPrimitive* LXEditMesh::CreateMonoIndexedVertexArray()
{
#if(0)
	LXPrimitive* pGeometry = new LXPrimitive();

	ArrayUint&  arrayIndices = pGeometry->GetArrayIndices();
	ArrayVec3f& arrayPositions = const_cast<ArrayVec3f&>(pGeometry->GetArrayPositions());
	ArrayVec2f& arrayTexCoords = const_cast<ArrayVec2f&>(pGeometry->GetArrayTexCoords());
	ArrayVec3f& arrayNormals = const_cast<ArrayVec3f&>(pGeometry->GetArrayNormals());
	ArrayVec3f& arrayTangents = const_cast<ArrayVec3f&>(pGeometry->GetArrayTangents());


	uint nFaces = m_PositionIndices.size();
	
	CFace3* pFaces = new CFace3[nFaces];
	std::vector<CIndexVTN*> arrayIndexedVectors;

	for(uint i=0; i<nFaces; i++)
	{
		for(uint j=0; j<3; j++)
		{
			uint v = m_PositionIndices[i][j];

			uint t = 0;
			if (m_pTFaces)
				t = m_pTFaces[i][j];

			uint n = m_pNFaces[i][j];

			bool bFind = false;
			int nPos = 0;

			for (std::vector<CIndexVTN*>::iterator It=arrayIndexedVectors.begin(); It!=arrayIndexedVectors.end(); It++)
			{
				CIndexVTN* p = *It;
				if (p->v == v)
					if (p->t == t)
						if (p->n == n)
						{
							bFind = true; 
							break;
						}
						nPos++;
			}

			if (bFind)
			{
				pFaces[i][j] = nPos;
			}
			else
			{
				pFaces[i][j] = nPos;

				CHK(0);
// 				CIndexVTN* p = new CIndexVTN;
// 				p->v = v; p->t = t; p->n = n;
// 				arrayIndexedVectors.push_back(p);
			}
		}
	}

	uint nVertices    = (uint)arrayIndexedVectors.size();

	for (uint i=0; i<nVertices; i++)
	{
		CIndexVTN* p = arrayIndexedVectors[i];
		arrayPositions.push_back(m_arrayPositions[p->v]);
		arrayNormals.push_back(m_arrayNormals[p->n]);
		if (m_arrayTexCoords.size())
			arrayTexCoords.push_back(m_arrayTexCoords[p->t]);
		delete p;
	}

	for(uint i=0; i<nFaces; i++)
	{
		arrayIndices.push_back(pFaces[i][0]);
		arrayIndices.push_back(pFaces[i][1]);
		arrayIndices.push_back(pFaces[i][2]);
	}
	
	if (m_arrayTexCoords.size())
	{
		for (uint i = 0; i< nVertices; i++)
			arrayTangents.push_back(vec3f(0.0f, 0.0f, 0.0f));

		for (uint i=0; i<nFaces; i++)
		{
			uint a = pFaces[i][0];
			uint b = pFaces[i][1];
			uint c = pFaces[i][2];

			vec3f v0 = arrayPositions[a];
			vec3f v1 = arrayPositions[b];
			vec3f v2 = arrayPositions[c];

			vec2f t0 = arrayTexCoords[a];
			vec2f t1 = arrayTexCoords[b];
			vec2f t2 = arrayTexCoords[c];

			vec3f side0 = v0-v1;
			vec3f side1 = v2-v1;

			side0.Normalize();
			side1.Normalize();

			// Face normal
			vec3f normal;
			normal.CrossProduct(side1, side0);
			normal.Normalize();

			// Now we use a formula to calculate the s tangent . We then use the same formula for the t tangent.

			float deltaT0=t0.y-t1.y;
			float deltaT1=t2.y-t1.y;
			vec3f sTangent=deltaT1*side0-deltaT0*side1;
			sTangent.Normalize();

			//Calculate t tangent
			float deltaS0=t0.x-t1.x;
			float deltaS1=t2.x-t1.x;
			vec3f tTangent=deltaS1*side0-deltaS0*side1;
			tTangent.Normalize();

			// Now, we take the cross product of the tangents to get a vector which should point in the same direction as our normal calculated above. If it points in the opposite direction (the dot product between the normals is less than zero), then we need to reverse the s and t tangents. This is because the triangle has been mirrored when going from tangent space to object space.

			//reverse tangents if necessary
			vec3f tangentCross;
			tangentCross.CrossProduct(sTangent, tTangent);
			if(tangentCross.DotProduct( normal )<0.0f)
			{
				int foo=0;

				sTangent.x=-sTangent.x;
				sTangent.y=-sTangent.y;
				sTangent.z=-sTangent.z;

				tTangent.x=-tTangent.x;
				tTangent.y=-tTangent.y;
				tTangent.z=-tTangent.z;
			}
			else
			{
				int foo=0;
			}

			arrayTangents[a] += sTangent;
			arrayTangents[b] += sTangent;
			arrayTangents[c] += sTangent;
		}


		for (uint i=0; i<pGeometry->GetVertices(); i++)
			arrayTangents[i].Normalize();
	}
	return pGeometry;
#endif
	return NULL;
}


uint FindVertexIndice(uint v, uint n, uint t, std::vector<CIndexVTN>& arrayIndexedVectors )
{
	bool bFind = false;

	for(uint i = 0; i<arrayIndexedVectors.size(); i++)
	{
		CIndexVTN& p = arrayIndexedVectors[i];
		if (p.v == v && p.t == t && p.n == n)
			return i;
	}

	arrayIndexedVectors.push_back(CIndexVTN(v,n,t));
	return (uint)arrayIndexedVectors.size() - 1;
}

bool LXEditMesh::FinalizeGeometry(LXPrimitive* pGeometry, std::vector<CIndexVTN>& arrayIndexedVectors )
{
	ArrayVec3f& arrayPositions = const_cast<ArrayVec3f&>(pGeometry->GetArrayPositions());
	ArrayVec3f& arrayNormals = const_cast<ArrayVec3f&>(pGeometry->GetArrayNormals());
	ArrayVec2f& arrayTexCoords = const_cast<ArrayVec2f&>(pGeometry->GetArrayTexCoords());
	
	uint nVertices = (uint)arrayIndexedVectors.size();
	
	arrayPositions.reserve(nVertices);
	arrayNormals.reserve(nVertices);

	if (m_arrayTexCoords.size())
		arrayTexCoords.reserve(nVertices);

	for (uint i=0; i<nVertices; i++)
	{
		CIndexVTN& p = arrayIndexedVectors[i];
		arrayPositions.push_back(m_arrayPositions[p.v]);
		arrayNormals.push_back(m_arrayNormals[p.n]);
		if (m_arrayTexCoords.size())
			arrayTexCoords.push_back(m_arrayTexCoords[p.t]);
	}

	//if (arrayTexCoords.size() > 0)
		//pGeometry->ComputeTangents();

	return true;
}


bool LXEditMesh::CreateMonoIndexedVertexArray2(ListPrimitives& listGeometries)
{
	LXPrimitive* pGeometry = NULL;
	std::shared_ptr<LXMaterial> pMaterial;
	std::vector<CIndexVTN> arrayIndexedVectors;

	uint nFaces = (uint)m_PositionIndices.size();
		
	for(uint i=0; i<nFaces; i++)
	{
		//CFace3* pVFace = &m_PositionIndices[i];
		vec3ui* pVFace = &m_PositionIndices[i];
		CFace3* pNFace = &m_pNFaces[i];
		CFace3* pTFace = NULL;
		if (m_pTFaces)
			pTFace = &m_pTFaces[i];
		
		if (!pGeometry || (pMaterial != m_ppMaterials[i]))
		{
			if (pGeometry)
			{
				std::shared_ptr<LXMaterialBase> materialBase = std::static_pointer_cast<LXMaterialBase>(pMaterial);
				pGeometry->SetMaterial(materialBase);
				FinalizeGeometry(pGeometry, arrayIndexedVectors);
			}

			listGeometries.push_back(std::make_shared<LXPrimitive>());
			pGeometry = listGeometries.back().get();

			arrayIndexedVectors.clear();
			pMaterial = m_ppMaterials[i];
		}

		uint face[3];
		
		for(uint j=0; j<3; j++)
		{
			uint v = (*pVFace)[j];
			uint n = (*pNFace)[j];
			uint t = 0;
			if (pTFace)
				t = (*pTFace)[j];

			face[j] = FindVertexIndice(v,n,t, arrayIndexedVectors);
		}
		
		ArrayUint& arrayIndices = pGeometry->GetArrayIndices();

		arrayIndices.push_back(face[0]);
		arrayIndices.push_back(face[1]);
		arrayIndices.push_back(face[2]);
	}

	CHK(pGeometry);
	if (pGeometry)
	{
		std::shared_ptr<LXMaterialBase> materialBase = std::static_pointer_cast<LXMaterialBase>(pMaterial);
		pGeometry->SetMaterial(materialBase);
		FinalizeGeometry(pGeometry, arrayIndexedVectors);
	}

	return true;
}