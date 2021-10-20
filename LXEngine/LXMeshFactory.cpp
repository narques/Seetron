//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXMeshFactory.h"
#include "LXMesh.h"
#include "LXEngine.h"
#include "LXPrimitive.h"
#include "LXPrimitiveFactory.h"

LXMeshFactory::LXMeshFactory()
{
}

LXMeshFactory::~LXMeshFactory()
{
	_meshes.clear();
}

const std::shared_ptr<LXMesh> LXMeshFactory::CreateEmptyMesh()
{
	std::shared_ptr<LXMesh> mesh = std::make_shared<LXMesh>();
	return mesh;
}

const std::shared_ptr<LXMesh> LXMeshFactory::CreateCube(float sizeX, float sizeY, float sizeZ, bool centered /*= true*/)
{
	std::shared_ptr<LXMesh> mesh = std::make_shared<LXMesh>();
	const std::shared_ptr<LXPrimitive>& primitive = GetPrimitiveFactory()->CreateCube(sizeX, sizeY, sizeZ, centered);
	primitive->SetPersistent(false);
	primitive->ComputeNormals();
	primitive->ComputeTangents();
	mesh->AddPrimitive(primitive);
	return mesh;
}

const std::shared_ptr<LXMesh> LXMeshFactory::CreateQuadXY(float sizeX, float sizeY, bool centered /*= true*/)
{
	std::shared_ptr<LXMesh> mesh = std::make_shared<LXMesh>();
	const std::shared_ptr<LXPrimitive>& primitive = GetPrimitiveFactory()->CreateQuadXY(sizeX, sizeY, centered);
	primitive->SetPersistent(false);
	primitive->ComputeTangents();
	mesh->AddPrimitive(primitive);
	return mesh;
}

const std::shared_ptr<LXMesh> LXMeshFactory::CreateSphere(float radius)
{
	std::shared_ptr<LXMesh> mesh = std::make_shared<LXMesh>();
	const std::shared_ptr<LXPrimitive>& primitive = GetPrimitiveFactory()->CreateSphere(radius);
	primitive->SetPersistent(false);
	primitive->ComputeTangents();
	mesh->AddPrimitive(primitive);
	return mesh;
}


const std::shared_ptr<LXMesh> LXMeshFactory::CreateCylinder(float radius, float height)
{
	std::shared_ptr<LXMesh> mesh = std::make_shared<LXMesh>();
	const std::shared_ptr<LXPrimitive>& primitive = GetPrimitiveFactory()->CreateCylinder(radius, height);
	primitive->SetPersistent(false);
	primitive->ComputeTangents();
	mesh->AddPrimitive(primitive);
	return mesh;
}

const std::shared_ptr<LXMesh> LXMeshFactory::CreateCone(float radius, float height)
{
	std::shared_ptr<LXMesh> mesh = std::make_shared<LXMesh>();
	const std::shared_ptr<LXPrimitive>& primitive = GetPrimitiveFactory()->CreateCone(radius, height);
	primitive->SetPersistent(false);
	primitive->ComputeTangents();
	mesh->AddPrimitive(primitive);
	return mesh;
}

const std::shared_ptr<LXMesh> LXMeshFactory::CreateSSTriangle()
{
	std::shared_ptr<LXMesh> mesh = std::make_shared<LXMesh>();
	const std::shared_ptr<LXPrimitive>& primitive = GetPrimitiveFactory()->CreateSSTriangle();
	primitive->SetPersistent(false);
	mesh->AddPrimitive(primitive);
	return mesh;
}
