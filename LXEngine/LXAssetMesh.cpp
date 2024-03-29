//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXAssetMesh.h"
#include "LXMesh.h"
#include "LXSmartObject.h"
#include "LXMSXMLNode.h"
#include "LXFile.h"
#include "LXPrimitive.h"
#include "LXAssetManager.h"
#include "LXEngine.h"
#include "LXProject.h"
#include "LXMaterial.h"
#include "LXShader.h"
#include "LXStatistic.h"

LXAssetMesh::LXAssetMesh()
{
	LX_COUNTSCOPEINC(LXAssetMesh)
}

LXAssetMesh::LXAssetMesh(std::shared_ptr<LXMesh>& mesh) :_Root(mesh)
{
	LX_COUNTSCOPEINC(LXAssetMesh)
}

LXAssetMesh::~LXAssetMesh()
{
	LX_COUNTSCOPEDEC(LXAssetMesh)
}

bool LXAssetMesh::Load()
{
	if (State == EResourceState::LXResourceState_Loaded)
		return true;

	LXFilepath strFilenameGeo = _filepath;
	strFilenameGeo.ChangeExtensionTo(LX_MESHBIN_EXT);
	
	bool Result = false;
	
	if (LoadGeometries(strFilenameGeo, _mapGeometries) == true)
		Result = LoadWithMSXML(_filepath);

	if (Result)
		State = EResourceState::LXResourceState_Loaded;
		
	return false;
}

std::shared_ptr<LXMesh>& LXAssetMesh::GetMesh()
{
	return _Root;
}

bool LXAssetMesh::LoadGeometries(const LXFilepath& strFilename, MapGeometries& mapGeometries)
{
	LXFile file;

	if (!file.Open(strFilename, L"rb"))
		return false;

	LXPrimitive* Primitive = nullptr;

	TFileTag fileTag;

	while (file.Read(&fileTag, sizeof(TFileTag)))
	{
		if (fileTag.nId != -1)
		{
			std::shared_ptr<LXPrimitive>& PrimitivePtr = mapGeometries[fileTag.nId];
			CHK(PrimitivePtr.get() == nullptr);
			PrimitivePtr = std::make_shared<LXPrimitive>();
			Primitive = PrimitivePtr.get();
			Primitive->SetTopology(fileTag.eMode);
		}
		else
		{
			switch (fileTag.eDataTarget)
			{
			case LX_INDICES:
			{
				uint nSize = fileTag.nCount / sizeof(uint);
				uint* pIndices = new uint[nSize];
				file.Read(pIndices, fileTag.nCount);
				ArrayUint& arrayIndices = Primitive->GetArrayIndices();
				arrayIndices.reserve(nSize);
				for (uint i = 0; i < nSize; i++)
					arrayIndices.push_back(pIndices[i]);

				delete[] pIndices;
			}
			break;

			case LX_VERTICES:
			{
				uint nSize = fileTag.nCount / sizeof(vec3f);
				vec3f* pVertices = new vec3f[nSize];
				file.Read(pVertices, fileTag.nCount);

				ArrayVec3f& arrayVertices = const_cast<ArrayVec3f&>(Primitive->GetArrayPositions());
				arrayVertices.reserve(nSize);
				for (uint i = 0; i < nSize; i++)
					arrayVertices.push_back(pVertices[i]);
				
				delete[] pVertices;
			}
			break;

			case LX_TEXTCOORD:
			{
				uint nSize = fileTag.nCount / sizeof(vec2f);
				vec2f* pTexCoords = new vec2f[nSize];
				file.Read(pTexCoords, fileTag.nCount);

				ArrayVec2f& arrayTexCoords = const_cast<ArrayVec2f&>(Primitive->GetArrayTexCoords());
				arrayTexCoords.reserve(nSize);
				for (uint i = 0; i < nSize; i++)
					arrayTexCoords.push_back(pTexCoords[i]);

				delete[] pTexCoords;
			}
			break;

			case LX_NORMALS:
			{
				uint nSize = fileTag.nCount / sizeof(vec3f);
				vec3f* pNormals = new vec3f[nSize];
				file.Read(pNormals, fileTag.nCount);

				ArrayVec3f& arrayNormals = const_cast<ArrayVec3f&>(Primitive->GetArrayNormals());
				arrayNormals.reserve(nSize);
				for (uint i = 0; i < nSize; i++)
					arrayNormals.push_back(pNormals[i] * -1.f);

				delete[] pNormals;

			}
			break;

			case LX_TANGENTS:
			{
				uint nSize = fileTag.nCount / sizeof(vec3f);
				vec3f* pTangents = new vec3f[nSize];
				file.Read(pTangents, fileTag.nCount);

				ArrayVec3f& arrayTangents = const_cast<ArrayVec3f&>(Primitive->GetArrayTangents());
				arrayTangents.reserve(nSize);
				for (uint i = 0; i < nSize; i++)
					arrayTangents.push_back(pTangents[i]);

				delete[] pTangents; 
			}
			break;


			case LX_BINORMALS:
			{
				uint nSize = fileTag.nCount / sizeof(vec3f);
				vec3f* pBiNormals = new vec3f[nSize];
				file.Read(pBiNormals, fileTag.nCount);

				ArrayVec3f& arrayBiNormals = const_cast<ArrayVec3f&>(Primitive->GetArrayBiNormals());
				arrayBiNormals.reserve(nSize);
				for (uint i = 0; i < nSize; i++)
					arrayBiNormals.push_back(pBiNormals[i]);

				delete[] pBiNormals;
			}
			break;

			default:CHK(0);
			}
		}
	}

	file.Close();
	return true;
}

bool LXAssetMesh::OnLoadChild(const TLoadContext& loadContext)
{
	const LXString& name = loadContext.node.name();
	bool bRet = false;

	if (name == L"LXMesh")
	{
		CHK(_Root == nullptr);
		std::shared_ptr<LXMesh> mesh = std::make_shared<LXMesh>(this);
		mesh->Load(loadContext);
		_Root = mesh;
		bRet = true;
	}

	return bRet;
}

void LXAssetMesh::OnLoaded()
{
	_Root->ComputeMatrixRCS();
}

bool LXAssetMesh::OnSaveChild(const TSaveContext& saveContext) const
{
	_Root->Save(saveContext);
	return true;
}

void LXAssetMesh::OnMeshesdTransformationChanged()
{
	InvokeCB(L"TransformationChanged");
}

void LXAssetMesh::OnMeshesBoundingBoxInvalidated()
{
	InvokeCB(L"MeshesBoundingBoxInvalidated");
}

void LXAssetMesh::OnMeshVisibilityChanged()
{
	InvokeCB(L"VisibiltyChanged");
}