//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"

class LXPrimitive;
class LXMatrix;

class LXPrimitiveFactory : public LXObject
{
	friend LXPrimitive;

public:

	LXPrimitiveFactory();
	virtual ~LXPrimitiveFactory();

	void DeleteAllPrimitives();

	void Tick();

	// Create a "ScreenSpace" Triangle
	const std::shared_ptr<LXPrimitive>& CreateSSTriangle();

	// Create cone 
	const std::shared_ptr<LXPrimitive>& CreateCone(float radius, float height);

	// Creates an Indexed Sphere with UVs and Normals
	const std::shared_ptr<LXPrimitive>& CreateSphere(float radius);

	// Create an indexed Cube with UVs
	const std::shared_ptr<LXPrimitive>& CreateCube(float x, float y, float z, bool bCentered = true);

	// Create an indexed tube
	const std::shared_ptr<LXPrimitive>& CreateTube(float innerRadius, float Radius, const LXMatrix* Matrix = nullptr);
	
	// Create indexed square with UVs and Normals
	const std::shared_ptr<LXPrimitive>& CreateTerrainPatch(int QuadCount, int QuadSize);
	
	// Create an indexed Cylinder with UVs and Normals
	const std::shared_ptr<LXPrimitive>& CreateCylinder(float radius, float height);

	// Create grass 
	const std::shared_ptr<LXPrimitive>& CreateGrassPatch();

	// Create an indexed WireframeCube., centered or not 
	const std::shared_ptr<LXPrimitive>& CreateWireframeCube(float SizeX, float SizeY, float SizeZ, bool bCentered = true);

	// Create an indexed WireframeCube. Min & Max positions based 
	const std::shared_ptr<LXPrimitive>& CreateWireframeCube(float x0, float y0, float z0, float x1, float y1, float z1);

	const std::shared_ptr<LXPrimitive>& CreateQuadXY(float width, float height, bool Centered);
	const std::shared_ptr<LXPrimitive>& CreateQuadXZ(float width, float height, bool Centered);

	//
	// Getters 
	//

	const std::shared_ptr<LXPrimitive>& GetWireframeCube() const {	return _PrimitiveWireFrameCube; }
	const std::shared_ptr<LXPrimitive>& GetPlaneXZ() const { return _PlaneXZ; }

private:

	const std::shared_ptr<LXPrimitive>& CreatePrimitive();

private:

	std::list<std::shared_ptr<LXPrimitive>> ListPrimitives;

	//
	// Useful primitives
	//

	// Centered WireframeCube (size 1)
	std::shared_ptr<LXPrimitive> _PrimitiveWireFrameCube;

	// Centered Plane (XZ) (size 1)
	std::shared_ptr<LXPrimitive> _PlaneXZ;
};
