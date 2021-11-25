//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXPrimitive.h"
#include "LXActorMesh.h"
#include "LXEngine.h"
#include "LXProject.h"
#include "LXSettings.h"
#include "LXFile.h"
#include "LXMath.h"
#include "LXAssetManager.h"
#include "LXAssetMesh.h"
#include "LXMaterial.h"
#include "LXStatistic.h"

int LXPrimitive::m_snPrimitives = 0;
int LXPrimitive::m_snPoints = 0;

LXPrimitive::LXPrimitive():
	m_pMaterial(NULL),
	m_nId(-1),
	_Topology(LX_TRIANGLES),
	m_bValid(false)
{
	LX_COUNTSCOPEINC(LXPrimitive)
	// PropertServer settings
	SetName(L"Primitive");
	DefineProperties();
}

LXPrimitive::LXPrimitive(const LXPrimitive& primitive) :
	m_bboxLocal(primitive.m_bboxLocal),
	m_nId(-1),
	m_bValid(false)
{
	LX_COUNTSCOPEINC(LXPrimitive)
	m_arrayIndices = primitive.m_arrayIndices;
	m_arrayPositions = primitive.m_arrayPositions;
	m_arrayPositions4f = primitive.m_arrayPositions4f;
	m_arrayNormals = primitive.m_arrayNormals;
	m_arrayTangents = primitive.m_arrayTangents;
	m_arrayBiNormals = primitive.m_arrayBiNormals;
	m_arrayTexCoords = primitive.m_arrayTexCoords;
	m_arrayTexCoords3f = primitive.m_arrayTexCoords3f;
	_Topology = primitive._Topology;
	m_pMaterial = primitive.m_pMaterial;
	DefineProperties();
}

/*virtual*/
LXPrimitive::~LXPrimitive(void)
{
	LX_COUNTSCOPEDEC(LXPrimitive)
}

void LXPrimitive::Empty()
{
	m_arrayIndices.clear();
	m_arrayPositions.clear();
	m_arrayPositions4f.clear();
	m_arrayNormals.clear();
	m_arrayTangents.clear();
	m_arrayBiNormals.clear();
	m_arrayTexCoords.clear();
	m_arrayTexCoords3f.clear();
	m_bValid = false;
}

/*virtual*/
void LXPrimitive::DefineProperties( ) 
{
	
	// --------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Bounds");
	// --------------------------------------------------------------------------------------------------------------

	LXPropertyFloat* pPropSizeX = DefinePropertyFloat(L"SizeX", LXPropertyID::SIZEX, NULL);
	pPropSizeX->SetLambdaOnGet([this]()
	{
		return GetBBoxLocal().GetSizeX();
	});
	pPropSizeX->SetPersistent(false);
	pPropSizeX->SetReadOnly(true);

	LXPropertyFloat* pPropSizeY = DefinePropertyFloat(L"SizeY", LXPropertyID::SIZEY, NULL);
	pPropSizeY->SetLambdaOnGet([this]()
	{
		return GetBBoxLocal().GetSizeY();
	});
	pPropSizeY->SetPersistent(false);
	pPropSizeY->SetReadOnly(true);

	LXPropertyFloat* pPropSizeZ = DefinePropertyFloat(L"SizeZ", LXPropertyID::SIZEZ, NULL);
	pPropSizeZ->SetLambdaOnGet([this]()
	{
		return GetBBoxLocal().GetSizeZ();
	});
	pPropSizeZ->SetPersistent(false);
	pPropSizeZ->SetReadOnly(true);


	#ifdef LX_DEBUG_PRIMITIVE_PROPVISIBLE
	{
		auto p = DefinePropertyBool(L"Visible", LXPropertyID::PRIMITIVE_VISIBLE, &_bVisible);
		p->SetPersistent(false);
	}
	#endif

	// Modes
	LXPropertyEnum* pPropEnum = DefinePropertyEnum(L"Mode", LXPropertyID::PRIMITIVE_MODE, (uint*)&_Topology);
	pPropEnum->SetPersistent(false);
	pPropEnum->SetReadOnly(true);
	pPropEnum->AddChoice(L"Undefined", LX_MODE_UNDEFINED);
	pPropEnum->AddChoice(L"Points", LX_POINTS);
	pPropEnum->AddChoice(L"Lines", LX_LINES);
	pPropEnum->AddChoice(L"Line loop", LX_LINE_LOOP);
	pPropEnum->AddChoice(L"Line strip", LX_LINE_STRIP);    
	pPropEnum->AddChoice(L"Triangles", LX_TRIANGLES);
	pPropEnum->AddChoice(L"Triangle strip", LX_TRIANGLE_STRIP);
	pPropEnum->AddChoice(L"Triangle fan", LX_TRIANGLE_FAN);
	pPropEnum->AddChoice(L"Triangles adjacency", LX_TRIANGLES_ADJACENCY); 
	
	// Indices
	LXPropertyUint* pProp = DefinePropertyUint(L"Indices", LXPropertyID::PRIMITIVE_INDICES, NULL);
	pProp->SetPersistent(false);
	pProp->SetReadOnly(true);
	pProp->SetLambdaOnGet([this] { return GetIndices(); });
	
	// Vertices
	pProp = DefinePropertyUint(L"Vertices", LXPropertyID::PRIMITIVE_VERTICES, NULL);
	pProp->SetPersistent(false);
	pProp->SetReadOnly(true);
	pProp->SetLambdaOnGet([this] { return GetVertices(); });
	
	// Normals
	LXPropertyBool* pPropBool = DefinePropertyBool(L"Normals", LXPropertyID::PRIMITIVE_NORMALS, NULL);
	pPropBool->SetPersistent(false);
	pPropBool->SetReadOnly(true);
	pPropBool->SetLambdaOnGet([this] { return HasNormals(); });
	
	// TexCoords
	pPropBool = DefinePropertyBool(L"TexCoords", LXPropertyID::PRIMITIVE_TEXCOORDS, NULL);
	pPropBool->SetPersistent(false);
	pPropBool->SetReadOnly(true);
	pPropBool->SetLambdaOnGet([this] { return HasTexCoords(); });
	
	// Tangents
	pPropBool = DefinePropertyBool(L"Tangents", LXPropertyID::PRIMITIVE_TANGENTS, NULL);
	pPropBool->SetPersistent(false);
	pPropBool->SetReadOnly(true);
	pPropBool->SetLambdaOnGet([this] { return HasTangents(); });
		
	// BiNormals
	pPropBool = DefinePropertyBool(L"BiNormals", LXPropertyID::PRIMITIVE_BINORMALS, NULL);
	pPropBool->SetPersistent(false);
	pPropBool->SetReadOnly(true);
	pPropBool->SetLambdaOnGet([this] { return HasBiNormals(); });
	
	LXPropertyAssetPtr* pPropMaterial = LXPropertyAsset::Create(this, L"Material", LXPropertyID::PRIMITIVE_MATERIAL, (std::shared_ptr<LXAsset>*)&m_pMaterial);
	pPropMaterial->SetName(L"Material");
	pPropMaterial->SetUserData((int64)EAssetType::Material);
	pPropMaterial->SetPersistent(false);
}

bool LXPrimitive::Save( const TSaveContext& saveContext )
{
	LXAssetMesh* AssetMesh = dynamic_cast<LXAssetMesh*>(saveContext.Owner);
	CHK(AssetMesh);
	LXFile* pGeometryFile = AssetMesh->GetGeometryFile();
	
	if (pGeometryFile == nullptr)
		return false;

	// Geometry tag
	{
		TFileTag fileTag;
		fileTag.nId = GetId();
		fileTag.eDataTarget = LX_DATATARGET_UNDEFINED;
		fileTag.eDataType = LX_DATATYPE_UNDEFINED;
		fileTag.eMode = _Topology;
		fileTag.nCount = 0;
		uint nSize = sizeof(TFileTag);
		pGeometryFile->Write((void*)&fileTag, nSize);
	}

	// Indices
	if (m_arrayIndices.size() > 0)
	{
		TFileTag fileTag;
		fileTag.nId = -1;
		fileTag.eDataTarget = LX_INDICES;
		fileTag.eDataType = LX_VEC3UI;
		fileTag.eMode = LX_MODE_UNDEFINED;
		fileTag.nCount = (uint)(sizeof(uint) * m_arrayIndices.size());
		pGeometryFile->Write((void*)&fileTag, sizeof(TFileTag));
		pGeometryFile->Write((void*)&(m_arrayIndices[0]), fileTag.nCount);
	}

	// Vertices
	if (m_arrayPositions.size() > 0)
	{
		CHK(m_arrayPositions4f.size() == 0);
		TFileTag fileTag;
		fileTag.nId = -1;
		fileTag.eDataTarget = LX_VERTICES;
		fileTag.eDataType = LX_VEC3F;
		fileTag.eMode = LX_MODE_UNDEFINED;
		fileTag.nCount = (uint)(sizeof(vec3f) * m_arrayPositions.size());
		pGeometryFile->Write((void*)&fileTag, sizeof(TFileTag));
		pGeometryFile->Write((void*)&(m_arrayPositions[0]), fileTag.nCount);
	}

	if (m_arrayPositions4f.size() > 0)
	{
		CHK(m_arrayPositions.size() == 0);
		TFileTag fileTag;
		fileTag.nId = -1;
		fileTag.eDataTarget = LX_VERTICES;
		fileTag.eDataType = LX_VEC4F;
		fileTag.eMode = LX_MODE_UNDEFINED;
		fileTag.nCount = (uint)(sizeof(vec4f)* m_arrayPositions4f.size());
		pGeometryFile->Write((void*)&fileTag, sizeof(TFileTag));
		pGeometryFile->Write((void*)&(m_arrayPositions4f[0]), fileTag.nCount);
	}

	// Normals
	if ( m_arrayNormals.size() > 0)
	{
		TFileTag fileTag;
		fileTag.nId = -1;
		fileTag.eDataTarget = LX_NORMALS;
		fileTag.eDataType = LX_VEC3F;
		fileTag.eMode = LX_MODE_UNDEFINED;
		fileTag.nCount = (uint)(sizeof(vec3f) * m_arrayNormals.size());
		pGeometryFile->Write((void*)&fileTag, sizeof(TFileTag));
		pGeometryFile->Write((void*)&(m_arrayNormals[0]), fileTag.nCount);
	}

	// TextCoords
	if ( m_arrayTexCoords.size() > 0)
	{
		TFileTag fileTag;
		fileTag.nId = -1;
		fileTag.eDataTarget = LX_TEXTCOORD;
		fileTag.eDataType = LX_VEC2F;
		fileTag.eMode = LX_MODE_UNDEFINED;
		fileTag.nCount = (uint)(sizeof(vec2f) * m_arrayTexCoords.size());
		pGeometryFile->Write((void*)&fileTag, sizeof(TFileTag));
		pGeometryFile->Write((void*)&(m_arrayTexCoords[0]), fileTag.nCount);
	}

	// Tangents
	if ( m_arrayTangents.size() > 0)
	{
		TFileTag fileTag;
		fileTag.nId = -1;
		fileTag.eDataTarget = LX_TANGENTS;
		fileTag.eDataType = LX_VEC3F;
		fileTag.eMode = LX_MODE_UNDEFINED;
		fileTag.nCount = (uint)(sizeof(vec3f) * m_arrayTangents.size());
		pGeometryFile->Write((void*)&fileTag, sizeof(TFileTag));
		pGeometryFile->Write((void*)&(m_arrayTangents[0]), fileTag.nCount);
	}

	// BiNormals
	if ( m_arrayBiNormals.size() > 0)
	{
		TFileTag fileTag;
		fileTag.nId = -1;
		fileTag.eDataTarget = LX_BINORMALS;
		fileTag.eDataType = LX_VEC3F;
		fileTag.eMode = LX_MODE_UNDEFINED;
		fileTag.nCount = (uint)(sizeof(vec3f) * m_arrayBiNormals.size());
		pGeometryFile->Write((void*)&fileTag, sizeof(TFileTag));
		pGeometryFile->Write((void*)&(m_arrayBiNormals[0]), fileTag.nCount);
	}

	return true;
}

void LXPrimitive::ComputeBBoxLocal()
{
	m_bboxLocal.Reset();

	for (uint i = 0; i < m_arrayPositions.size(); i++)
		m_bboxLocal.Add(m_arrayPositions[i]);

	for (uint i = 0; i < m_arrayPositions4f.size(); i++)
	{
		vec3f v = m_arrayPositions4f[i];
		m_bboxLocal.Add(v);
	}
}

int LXPrimitive::GetId()
{
	if (m_nId == -1)
	{
		static int nId = 0;
		m_nId = nId;
		nId++;
	}
	return m_nId;
}

void LXPrimitive::SetMaterial( std::shared_ptr<LXMaterialBase>& pMaterial )
{ 
	m_pMaterial = pMaterial;
}

void LXPrimitive::SetMaterial(const LXString& Filename)
{
	LXAssetManager& rm = GetEngine().GetProject()->GetAssetManager();
	std::shared_ptr<LXMaterial> material = rm.GetMaterial(Filename);
	std::shared_ptr<LXMaterialBase> materialBase = std::static_pointer_cast<LXMaterialBase>(material);
	SetMaterial(materialBase);
}

void LXPrimitive::ComputeNormals()
{
	if (m_arrayPositions.size() == 0)
		return;

	// Prepare Normals Array
	
	if (m_arrayNormals.size() == 0)
	{
		for(int i=0; i<m_arrayPositions.size(); i++)
			m_arrayNormals.push_back(LX_VEC3F_NULL);
	}
	else
	{
		for(int i=0; i<m_arrayNormals.size(); i++)
			m_arrayNormals[i].Set(0.f, 0.f, 0.f);
	}

	// Compute
		
	if (m_arrayIndices.size() > 0)
	{
		switch(_Topology)
		{
		case LX_TRIANGLE_STRIP:
			{
				CHK(0);
			}
			break;
		case LX_TRIANGLES:
		case LX_3_CONTROL_POINT_PATCH:
			{
				// Compute FaceNormal

				uint nTriangles = (uint)m_arrayIndices.size() / 3;
				vec3f* faceNormals = new vec3f[nTriangles];
				for (uint i=0; i<nTriangles; i++)
				{
					int a = m_arrayIndices[i*3];
					int b = m_arrayIndices[i*3+1];
					int c = m_arrayIndices[i*3+2];

					vec3f& v0 = m_arrayPositions[a];
					vec3f& v1 = m_arrayPositions[b];
					vec3f& v2 = m_arrayPositions[c];
					
					vec3f v0v1 = v1 - v0;
					vec3f v0v2 = v2 - v0;

					//faceNormals[i].CrossProduct(v0v1, v0v2);
					faceNormals[i].CrossProduct(v0v2, v0v1);
					faceNormals[i].Normalize();
				}

				// Compute VertexNormal
				for (uint i=0; i<nTriangles; i++)
				{
					int a = m_arrayIndices[i*3];
					int b = m_arrayIndices[i*3+1];
					int c = m_arrayIndices[i*3+2];

					m_arrayNormals[a] += faceNormals[i];
					m_arrayNormals[b] += faceNormals[i];
					m_arrayNormals[c] += faceNormals[i];
				}
								
				delete [] faceNormals;
			}
			break;
		default:CHK(0);
		}
	}
	else
	{
		switch(_Topology)
		{
		case LX_TRIANGLE_STRIP:
			{
				CHK(0);
			}
			break;
		case LX_TRIANGLES:
		case LX_3_CONTROL_POINT_PATCH:
			{
				CHK(0);
			}
			break;
		default:CHK(0);
		}
	}

	// Normalize
	
	for(int i=0; i<m_arrayNormals.size(); i++)
		m_arrayNormals[i].Normalize();
}


void LXPrimitive::ComputeTangents()
{
	if (_Topology != LX_TRIANGLES && _Topology != LX_TRIANGLE_FAN && _Topology != LX_TRIANGLE_STRIP && _Topology != LX_3_CONTROL_POINT_PATCH)
		return;

	if (m_arrayPositions.size())
	{ 
		if (m_arrayTexCoords.size())
			ComputeTangents2(m_arrayPositions, m_arrayTexCoords);
		else if (m_arrayTexCoords3f.size())
			ComputeTangents2(m_arrayPositions, m_arrayTexCoords3f);
		else
			CHK(0);
	}
	else if (m_arrayPositions4f.size())
	{
		if (m_arrayTexCoords.size())
			ComputeTangents2(m_arrayPositions4f, m_arrayTexCoords);
		else if (m_arrayTexCoords3f.size())
			ComputeTangents2(m_arrayPositions4f, m_arrayTexCoords3f);
		else
			CHK(0);
	}
	else
		CHK(0);

}

template<class T, class U>
void LXPrimitive::ComputeTangents2(std::vector<U>& arrayPositions, std::vector<T>& arrayTexCoords)
{
	
	int nVertices = (int)arrayPositions.size();
	CHK(nVertices);
	int sz = (int)m_arrayTangents.capacity();

	m_arrayTangents.clear();
	m_arrayBiNormals.clear();
	
	m_arrayTangents.reserve(nVertices);
	m_arrayBiNormals.reserve(nVertices);

	if (m_arrayIndices.size() > 0)
	{
		switch(_Topology)
		{
		case LX_TRIANGLE_STRIP:
		case LX_TRIANGLES:
		case LX_3_CONTROL_POINT_PATCH:
			{
				for (int i = 0; i < nVertices; i++)
				{
					m_arrayTangents.push_back(LX_VEC3F_NULL);
					m_arrayBiNormals.push_back(LX_VEC3F_NULL);
				}

				for (uint i=0; i<m_arrayIndices.size(); i+=3)
				{
					uint a = m_arrayIndices[i+0];
					uint b = m_arrayIndices[i+1];
					uint c = m_arrayIndices[i+2];

					vec3f v0 = arrayPositions[a];
					vec3f v1 = arrayPositions[b];
					vec3f v2 = arrayPositions[c];

					T t0 = arrayTexCoords[a];
					T t1 = arrayTexCoords[b];
					T t2 = arrayTexCoords[c];

					vec3f edge0 = v0 - v1;
					vec3f edge1 = v2 - v1;
					
					if(!edge0.IsNull())
						edge0.Normalize();
					
					if(!edge1.IsNull())
						edge1.Normalize();

					T texEdge0 = t0 - t1;
					T texEdge1 = t2 - t1;
					texEdge0.Normalize();
					texEdge1.Normalize();

					vec3f tangent;
					vec3f binormal;

					float det = (texEdge0.x * texEdge1.y) - (texEdge0.y * texEdge1.x);

					//CHK(det != 0.f);
					if (det == 0.f)
						det = 1.f;
					
					if (0/*Math::closeEnough(det, 0.0f))*/)
					{
						CHK(0);
					}
					else
					{
						det = 1.f / det;
						tangent.x = (texEdge1.y * edge0.x - texEdge0.y * edge1.x) * det;
						tangent.y = (texEdge1.y * edge0.y - texEdge0.y * edge1.y) * det;
						tangent.z = (texEdge1.y * edge0.z - texEdge0.y * edge1.z) * det;

						binormal.x = (-texEdge1.x * edge0.x + texEdge0.x * edge1.x) * det;
						binormal.y = (-texEdge1.x * edge0.y + texEdge0.x * edge1.y) * det;
						binormal.z = (-texEdge1.x * edge0.z + texEdge0.x * edge1.z) * det;

						if (!tangent.IsNull())
						{
							tangent.Normalize();
						}
						else
						{
							CHK(0);
						}
						
						if (!binormal.IsNull())
						{
							binormal.Normalize();
						}
						else
						{
							CHK(0);
						}
					}

					m_arrayTangents[a] = tangent;
					m_arrayTangents[b] = tangent;
					m_arrayTangents[c] = tangent;

					m_arrayBiNormals[a] = binormal;
					m_arrayBiNormals[b] = binormal;
					m_arrayBiNormals[c] = binormal;

				}
			}
			break;
		default:
			CHK(0);
		}

	}
	else
	{
		switch(_Topology)
		{

		case LX_TRIANGLES:
		case LX_3_CONTROL_POINT_PATCH:
			{
				for (int i = 0; i < nVertices; i++)
				{
					m_arrayTangents.push_back(LX_VEC3F_NULL);
					m_arrayBiNormals.push_back(LX_VEC3F_NULL);
				}

				for (int i = 0; i < nVertices; i+=3)
				{
					U& v0 = arrayPositions[i];
					U& v1 = arrayPositions[i+1];
					U& v2 = arrayPositions[i+2];

					T& t0 = arrayTexCoords[i];
					T& t1 = arrayTexCoords[i+1];
					T& t2 = arrayTexCoords[i+2];

					vec3f edge0 = v0 - v1;
					vec3f edge1 = v2 - v1;
					edge0.Normalize();
					edge1.Normalize();

					T texEdge0 = t0 - t1;
					T texEdge1 = t2 - t1;
					texEdge0.Normalize();
					texEdge1.Normalize();

					vec3f tangent;
					vec3f binormal;

					float det = (texEdge0.x * texEdge1.y) - (texEdge0.y * texEdge1.x);

					CHK(det != 0.f);

					if (0/*Math::closeEnough(det, 0.0f))*/)
					{

					}
					else
					{
						det = 1.f / det;
						tangent.x = (texEdge1.y * edge0.x - texEdge0.y * edge1.x) * det;
						tangent.y = (texEdge1.y * edge0.y - texEdge0.y * edge1.y) * det;
						tangent.z = (texEdge1.y * edge0.z - texEdge0.y * edge1.z) * det;

						binormal.x = (-texEdge1.x * edge0.x + texEdge0.x * edge1.x) * det;
						binormal.y = (-texEdge1.x * edge0.y + texEdge0.x * edge1.y) * det;
						binormal.z = (-texEdge1.x * edge0.z + texEdge0.x * edge1.z) * det;

						tangent.Normalize();
						binormal.Normalize();

					}
					m_arrayTangents[i] = tangent;
					m_arrayTangents[i+1] = tangent;
					m_arrayTangents[i+2] = tangent;

					m_arrayBiNormals[i] = binormal;
					m_arrayBiNormals[i+1] = binormal;
					m_arrayBiNormals[i+2] = binormal;
				}
			}
			break;

		case LX_TRIANGLE_STRIP:
			{
				for (int i = 0; i < nVertices; i++)
				{
					m_arrayTangents.push_back(LX_VEC3F_NULL);
					m_arrayBiNormals.push_back(LX_VEC3F_NULL);
				}

				for (int i = 0; i< (nVertices - 2); i++)
				{
					U& v0 = arrayPositions[i];
					U& v1 = arrayPositions[i+1];
					U& v2 = arrayPositions[i+2];

					T& t0 = arrayTexCoords[i];
					T& t1 = arrayTexCoords[i+1];
					T& t2 = arrayTexCoords[i+2];

					vec3f edge0 = v0 - v1;
					vec3f edge1 = v2 - v1;
					edge0.Normalize();
					edge1.Normalize();

					T texEdge0 = t0 - t1;
					T texEdge1 = t2 - t1;
					texEdge0.Normalize();
					texEdge1.Normalize();

					vec3f tangent;
					vec3f binormal;

					float det = (texEdge0.x * texEdge1.y) - (texEdge0.y * texEdge1.x);

					CHK(det != 0.f);

					if (0/*Math::closeEnough(det, 0.0f))*/)
					{

					}
					else
					{
						det = 1.f / det;
						tangent.x = (texEdge1.y * edge0.x - texEdge0.y * edge1.x) * det;
						tangent.y = (texEdge1.y * edge0.y - texEdge0.y * edge1.y) * det;
						tangent.z = (texEdge1.y * edge0.z - texEdge0.y * edge1.z) * det;

						binormal.x = (-texEdge1.x * edge0.x + texEdge0.x * edge1.x) * det;
						binormal.y = (-texEdge1.x * edge0.y + texEdge0.x * edge1.y) * det;
						binormal.z = (-texEdge1.x * edge0.z + texEdge0.x * edge1.z) * det;

						tangent.Normalize();
						binormal.Normalize();

					}
					m_arrayTangents[i] = tangent;
					m_arrayTangents[i+1] = tangent;
					m_arrayTangents[i+2] = tangent;

					m_arrayBiNormals[i] = binormal;
					m_arrayBiNormals[i+1] = binormal;
					m_arrayBiNormals[i+2] = binormal;
				}
			}
			break;
		default:
			CHK(0);
		}
	}
}

void LXPrimitive::AddQuadXY(const vec3f& vPosition, float width, float height, float depth)
{
	CHK(_Topology == LX_TRIANGLES);
	
	vec3f v1(0.f, width, 0.f);
	vec3f v2(0.f, 0.f, 0.f);
	vec3f v3(width, width, 0.f);
	vec3f v4(width, 0.f, 0.f);

	v1 += vPosition;
	v2 += vPosition;
	v3 += vPosition;
	v4 += vPosition;

	const vec3f vn(0.0f, 0.0f, 1.0f);

	vec3f t1(0.0f, 1.0f, depth);
	vec3f t2(0.0f, 0.0f, depth);
	vec3f t3(1.0f, 1.0f, depth);
	vec3f t4(1.0f, 0.0f, depth);

	int nIndex = (int)GetArrayPositions().size();

	GetArrayPositions().push_back(v1);
	GetArrayPositions().push_back(v2);
	GetArrayPositions().push_back(v3);
	GetArrayPositions().push_back(v4);

	GetArrayNormals().push_back(vn);
	GetArrayNormals().push_back(vn);
	GetArrayNormals().push_back(vn);
	GetArrayNormals().push_back(vn);

	GetArrayTexCoords3f().push_back(t1);
	GetArrayTexCoords3f().push_back(t2);
	GetArrayTexCoords3f().push_back(t3);
	GetArrayTexCoords3f().push_back(t4);

	
	GetArrayIndices().push_back(nIndex);
	GetArrayIndices().push_back(nIndex+1);
	GetArrayIndices().push_back(nIndex+2);

	GetArrayIndices().push_back(nIndex+2);
	GetArrayIndices().push_back(nIndex+1);
	GetArrayIndices().push_back(nIndex+3);
}

void LXPrimitive::AddQuadYZ(const vec3f& vPosition, float width, float height, float depth)
{
	CHK(_Topology == LX_TRIANGLES);

	vec3f v1(0.f, 0.f, height);
	vec3f v2(0.f, 0.f, 0.f);
	vec3f v3(0.f, width, height);
	vec3f v4(0.f, width, 0.f);

	v1 += vPosition;
	v2 += vPosition;
	v3 += vPosition;
	v4 += vPosition;
		
	const vec3f vn(-1.0f, 0.0f, 0.0f);

	vec3f t1(0.0f,  /*1.0f*/height / width, depth);
	vec3f t2(0.0f, 0.0f, depth);
	vec3f t3(1.0f,  /*1.0f*/height / width, depth);
	vec3f t4(1.0f, 0.0f, depth);

	int nIndex = (int)GetArrayPositions().size();

	GetArrayPositions().push_back(v1);
	GetArrayPositions().push_back(v2);
	GetArrayPositions().push_back(v3);
	GetArrayPositions().push_back(v4);

	GetArrayNormals().push_back(vn);
	GetArrayNormals().push_back(vn);
	GetArrayNormals().push_back(vn);
	GetArrayNormals().push_back(vn);
	
	GetArrayTexCoords3f().push_back(t1);
	GetArrayTexCoords3f().push_back(t2);
	GetArrayTexCoords3f().push_back(t3);
	GetArrayTexCoords3f().push_back(t4);

	GetArrayIndices().push_back(nIndex);
	GetArrayIndices().push_back(nIndex+1);
	GetArrayIndices().push_back(nIndex+2);

	GetArrayIndices().push_back(nIndex+2);
	GetArrayIndices().push_back(nIndex+1);
	GetArrayIndices().push_back(nIndex+3);
}

void LXPrimitive::AddQuadXZ(const vec3f& vPosition, float width, float height, float depth)
{
	CHK(_Topology == LX_TRIANGLES);

	vec3f v1(0.f, 0.f, height);
	vec3f v2(0.f, 0.f, 0.f);
	vec3f v3(width, 0.f, height);
	vec3f v4(width, 0.f, 0.f);

	v1 += vPosition;
	v2 += vPosition;
	v3 += vPosition;
	v4 += vPosition;

	const vec3f vn(-1.0f, 0.0f, 0.0f);

	vec3f t1(0.0f, height / width, depth);
	vec3f t2(0.0f, 0.0f, depth);
	vec3f t3(1.0f,  height / width, depth);
	vec3f t4(1.0f, 0.0f, depth);

	int nIndex = (int)GetArrayPositions().size();

	GetArrayPositions().push_back(v1);
	GetArrayPositions().push_back(v2);
	GetArrayPositions().push_back(v3);
	GetArrayPositions().push_back(v4);

	GetArrayNormals().push_back(vn);
	GetArrayNormals().push_back(vn);
	GetArrayNormals().push_back(vn);
	GetArrayNormals().push_back(vn);

	GetArrayTexCoords3f().push_back(t1);
	GetArrayTexCoords3f().push_back(t2);
	GetArrayTexCoords3f().push_back(t3);
	GetArrayTexCoords3f().push_back(t4);

	GetArrayIndices().push_back(nIndex);
	GetArrayIndices().push_back(nIndex+1);
	GetArrayIndices().push_back(nIndex+2);

	GetArrayIndices().push_back(nIndex+2);
	GetArrayIndices().push_back(nIndex+1);
	GetArrayIndices().push_back(nIndex+3);
}

LXPrimitive* LXPrimitive::CreatePoint()
{
	LXPrimitive* p = new LXPrimitive();
	p->SetTopology(LX_POINTS);
	p->GetArrayPositions().push_back(vec3f(0.f, 0.f, 0.f));
	return p;
}

LXPrimitive* LXPrimitive::CreateLineStrip( )
{
	LXPrimitive* p = new LXPrimitive();
	p->SetTopology(LX_LINE_STRIP);
	return p;
}

LXPrimitive* LXPrimitive::CreateLineLoop( )
{
	LXPrimitive* p = new LXPrimitive();
	p->SetTopology(LX_LINE_LOOP);
	return p;
}

void LXPrimitive::LocalToParent( const LXMatrix& matrix )
{
	for(int i=0; i<m_arrayPositions.size(); i++)
		matrix.LocalToParentPoint(m_arrayPositions[i]);

	for(int i=0; i<m_arrayNormals.size(); i++)
		matrix.LocalToParentVector(m_arrayNormals[i]);

	for(int i=0; i<m_arrayTangents.size(); i++)
		matrix.LocalToParentVector(m_arrayTangents[i]);

	for(int i=0; i<m_arrayBiNormals.size(); i++)
		matrix.LocalToParentVector(m_arrayBiNormals[i]);
}

ArrayVec3f& operator+=(ArrayVec3f& dest, const ArrayVec3f& src)
{
	dest.insert(dest.end(), src.begin(), src.end());
	return dest;
}

ArrayVec2f& operator+=(ArrayVec2f& dest, const ArrayVec2f& src)
{
	dest.insert(dest.end(), src.begin(), src.end());
	return dest;
}

void LXPrimitive::MergeIndices(ArrayUint& dest, const ArrayUint& src)
{
	const int offset = (int)m_arrayPositions.size();
	for(int i=0; i< src.size(); i++)
		dest.push_back(src[i] + offset);
}

/*virtual*/
bool LXPrimitive::Merge(const LXPrimitive& v)
{
	LXPrimitive* pSource = (LXPrimitive*)&v;

	if ((pSource->m_arrayIndices.size() > 0) != (m_arrayIndices.size() > 0))
	{
		return false;
	}
		
	if(pSource->_Topology != _Topology)
	{
		return false;
	}
	
	if(pSource->m_pMaterial != m_pMaterial)
	{
		return false;
	}
		
	if(_Topology == LX_TRIANGLE_STRIP)
	{
		return false;
	}
		
	MergeIndices(m_arrayIndices, pSource->m_arrayIndices);

	m_arrayPositions += pSource->m_arrayPositions;
	m_arrayNormals += pSource->m_arrayNormals;
	m_arrayTangents += pSource->m_arrayTangents;
	m_arrayBiNormals += pSource->m_arrayBiNormals;
	m_arrayTexCoords += pSource->m_arrayTexCoords;
	m_arrayTexCoords3f += pSource->m_arrayTexCoords3f;
	
	m_bValid = false;

	return true; 
}

void LXPrimitive::SetPositions(float * lVertices, int lPolygonVertexCount, const int VERTEX_STRIDE)
{
	switch (VERTEX_STRIDE)
	{
	case 3:
		for (int i = 0; i < lPolygonVertexCount; i++)
		{
			m_arrayPositions.push_back(*(vec3f*)lVertices);
			lVertices += 3;
		}
		break;
	case 4:
		for (int i = 0; i < lPolygonVertexCount; i++)
		{
			m_arrayPositions.push_back(*(vec3f*)lVertices);
			lVertices += 4;
		}
		break;
	default: CHK(0);
		break;
	}

	int foo = 0;
}

void LXPrimitive::SetNormals(float* pNormals, int lPolygonVertexCount, const int NORMAL_STRIDE)
{
	switch (NORMAL_STRIDE)
	{
	case 3:
		for (int i = 0; i < lPolygonVertexCount; i++)
		{
			m_arrayNormals.push_back(*(vec3f*)pNormals);
			pNormals += 3;
		}
		break;
	default: CHK(0);
		break;
	}

	int foo = 0;
}

void LXPrimitive::SetTexCoords(float* pTexCoors, int lPolygonVertexCount, const int TC_STRIDE)
{
	switch (TC_STRIDE)
	{
	case 2:
		for (int i = 0; i < lPolygonVertexCount; i++)
		{
			m_arrayTexCoords.push_back(*(vec2f*)pTexCoors);
			pTexCoors += 2;
		}
		break;
	default: CHK(0);
		break;
	}

	int foo = 0;
}

LXPrimitive* LXPrimitive::CreateDisk(float radius, float height)
{
	int slices = 8; // Min : 3
	
	LXPrimitive* p = new LXPrimitive();
	p->SetTopology(LX_TRIANGLES);

	p->GetArrayPositions().push_back(vec3f(0.f, 0.f, 0.f));
	p->GetArrayNormals().push_back(vec3f(0.f, 0.f, 1.f));
	p->GetArrayTexCoords().push_back(vec2f(0.5f, 0.5f));

	for (int j = 0; j < slices; j++)
	{
		float angle = LX_2PI * (float)j / (float)slices;
		float cos1 = cos(angle);
		float sin1 = sin(angle);

		vec3f vertex(radius*cos(angle), -radius*sin(angle), 0);
		p->GetArrayPositions().push_back(vertex);
		p->GetArrayNormals().push_back(vec3f(0.f, 0.f, 1.f));
		vec2f TextCoord = (vec2f(vertex.x / radius, vertex.y / radius) + 1.f) * 0.5;
		p->GetArrayTexCoords().push_back(TextCoord);
	}
	
	// Indices
	for (int j = 0; j < slices; j++)
	{
		if (j == (slices - 1))
		{
			p->GetArrayIndices().push_back(0);
			p->GetArrayIndices().push_back(j + 1);
			p->GetArrayIndices().push_back(1); // The last triangle returns to the first vertex
		}
		else
		{
			p->GetArrayIndices().push_back(0);
			p->GetArrayIndices().push_back(j + 1);
			p->GetArrayIndices().push_back(j + 2);
		}
	}

	return p;
}

int LXPrimitive::GetMask() const
{
	int Mask = 0;

	CHK(m_arrayPositions4f.size() == 0); // TODO
	CHK(m_arrayTexCoords3f.size() == 0);
	
	if (m_arrayIndices.size() > 0)
		Mask |= LX_PRIMITIVE_INDICES;
		
	if (m_arrayPositions.size()/* || m_arrayPositions4f.size()*/)
		Mask |= LX_PRIMITIVE_POSITIONS;
	
	if (m_arrayNormals.size())
		Mask |= LX_PRIMITIVE_NORMALS;
	
	if (m_arrayTangents.size())
		Mask |= LX_PRIMITIVE_TANGENTS;
	
	if (m_arrayBiNormals.size())
		Mask |= LX_PRIMITIVE_BINORMALS;
	
	if (m_arrayTexCoords.size())
		Mask |= LX_PRIMITIVE_TEXCOORDS;
	
	//if (m_arrayTexCoords3f.size())
		//Mask |= LX_PRIMITIVE_TEXCOORDS;

	return Mask;
}

void* LXPrimitive::CreateInterleavedVertexArray(const int Mask, const int VertexCount, int *OutVertexStructSize) const
{
	void* VertexStruct = nullptr;
	int VertexStructSize = 0;

	const int FixedMask = Mask & ~LX_PRIMITIVE_INDICES;
	
	switch (FixedMask)
	{
		case LX_PRIMITIVE_POSITIONS : 
		{
			Vertex_P* Vertices = new Vertex_P[VertexCount];
			VertexStructSize = sizeof(Vertex_P);
			for (int i = 0; i < VertexCount; i++)
			{
				Vertices[i].position = m_arrayPositions[i];
			}
			VertexStruct = Vertices;
		}
		break;

		case LX_PRIMITIVE_POSITIONS | LX_PRIMITIVE_NORMALS : 
		{
			Vertex_PN* Vertices = new Vertex_PN[VertexCount];
			VertexStructSize = sizeof(Vertex_PN);
			for (int i = 0; i < VertexCount; i++)
			{
				Vertices[i].position = m_arrayPositions[i];
				Vertices[i].normal = m_arrayNormals[i];
			}
			VertexStruct = Vertices;
		}
		break;

		case LX_PRIMITIVE_POSITIONS | LX_PRIMITIVE_NORMALS | LX_PRIMITIVE_TEXCOORDS : 
		{
			Vertex_PNT* Vertices = new Vertex_PNT[VertexCount]; 
			VertexStructSize = sizeof(Vertex_PNT);
			for (int i = 0; i < VertexCount; i++)
			{
				Vertices[i].position = m_arrayPositions[i];
				Vertices[i].normal = m_arrayNormals[i];
				Vertices[i].texCoord = m_arrayTexCoords[i];
			}
			VertexStruct = Vertices;
		}
		
		break;
		
		case LX_PRIMITIVE_POSITIONS | LX_PRIMITIVE_NORMALS | LX_PRIMITIVE_TANGENTS : 
		{
			VertexStruct = new Vertex_PNA[VertexCount]; VertexStructSize = sizeof(Vertex_PNA);
			CHK(0);
		}
		
		break;

		case LX_PRIMITIVE_POSITIONS | LX_PRIMITIVE_NORMALS | LX_PRIMITIVE_TANGENTS | LX_PRIMITIVE_BINORMALS : 
		{
			VertexStruct = new Vertex_PNAB[VertexCount]; VertexStructSize = sizeof(Vertex_PNAB);
			CHK(0);
		}
		break;

		case LX_PRIMITIVE_POSITIONS | LX_PRIMITIVE_NORMALS | LX_PRIMITIVE_TANGENTS | LX_PRIMITIVE_TEXCOORDS :
		{
			VertexStruct = new Vertex_PNAT[VertexCount]; VertexStructSize = sizeof(Vertex_PNAT);
			CHK(0);
		}
		break;

		case LX_PRIMITIVE_POSITIONS | LX_PRIMITIVE_NORMALS | LX_PRIMITIVE_TANGENTS | LX_PRIMITIVE_BINORMALS | LX_PRIMITIVE_TEXCOORDS : 
		{
			Vertex_PNABT* Vertices = new Vertex_PNABT[VertexCount]; 
			VertexStructSize = sizeof(Vertex_PNABT);
			for (int i = 0; i < VertexCount; i++)
			{
				Vertices[i].position = m_arrayPositions[i]; 
				Vertices[i].texCoord = m_arrayTexCoords[i];
				Vertices[i].normal = m_arrayNormals[i];
				Vertices[i].binormal = m_arrayBiNormals[i];
				Vertices[i].tangent = m_arrayTangents[i];
			}
			VertexStruct =  Vertices;
		}
		break;

		case LX_PRIMITIVE_POSITIONS | LX_PRIMITIVE_TEXCOORDS: 
		{
			Vertex_PT* Vertices = new Vertex_PT[VertexCount];
			VertexStructSize = sizeof(Vertex_PT);
			for (int i = 0; i < VertexCount; i++)
			{
				Vertices[i].position = m_arrayPositions[i];
				Vertices[i].texCoord = m_arrayTexCoords[i];
			}
			VertexStruct = Vertices;
		}
		break;
		
		default:CHK(0); 
	}
	 
	*OutVertexStructSize = VertexStructSize;
	return VertexStruct;
}
