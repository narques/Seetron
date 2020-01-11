//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"

class LXMesh;

class LXMeshFactory : public LXObject
{

public:

	LXMeshFactory();
	virtual ~LXMeshFactory();

	const shared_ptr<LXMesh> CreateEmptyMesh();

	// Create a Cube with UVs, Normals, Tangents and BiNormals
	const shared_ptr<LXMesh> CreateCube(float sizeX, float sizeY, float sizeZ, bool centered = true);

	// Create a Quad with UVs, Normals, Tangents and BiNormals
	const shared_ptr<LXMesh> CreateQuadXY(float sizeX, float sizeY, bool centered = true);

	// Creates a Sphere with UVs, Normals, Tangents and BiNormals
	const shared_ptr<LXMesh> CreateSphere(float radius);

	// Create a Cylinder with UVs, Normals, Tangents and BiNormals
	const shared_ptr<LXMesh> CreateCylinder(float radius, float height);

	// Create a Cone with UVs, Normals, Tangents and BiNormals
	const shared_ptr<LXMesh> CreateCone(float radius, float height);

	// Create a "ScreenSpace" Triangle with UVs
	const shared_ptr<LXMesh> CreateSSTriangle();
	   
private:

	list<shared_ptr<LXMesh>> _meshes;

};

