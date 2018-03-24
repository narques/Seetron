//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXTraverser.h"
#include "LXAxis.h"

class LXWorldTransformation;
class LXPrimitive;
class LXMatrix;

struct LXPOI // Point of intersection 
{
	LXPOI(){}
	LXPOI(LXActor* n, LXPrimitive* p, const vec3f& v) :node(n), primitive(p), point(v){ }
	LXActor* node;
	LXPrimitive* primitive;
	vec3f point;
};

typedef map<float, LXPOI> MapIntersection;

class LXCORE_API LXPickTraverser : public LXTraverser
{

public:

	LXPickTraverser(void);
	virtual ~LXPickTraverser(void);

	// Overridden from LXTraverser
	virtual void		Apply( ) override;
	virtual void		OnActor( LXActor* pGroup ) override;
	virtual void		OnPrimitive(LXActorMesh* pMesh, LXWorldPrimitive* WorldPrimitive) override;
	
	void				SetRay(LXAxis& ray);
	
	uint				GetNumberOfIntersections	( ) { return (uint)m_mapIntersection.size(); }
	
	LXActor*			GetNearestNode				( );
	LXPrimitive*		GetNearestPrimitive			( );
	float				GetNearestDepth				( );
	vec3f*				GetNearestPOI				( );
	vec3f*				GetNearestPOIOnNode			( LXActor* pNode );

private:

	void				PickPoints					(LXAxis& rayLCS, LXActor* pMesh, LXPrimitive* Primitive );
	void				PickIndexedLineLoop			(LXAxis& rayLCS, LXActor* pMesh, LXPrimitive* Primitive );
	void				PickLineLoop				(LXAxis& rayLCS, LXActor* pMesh, LXPrimitive* Primitive );
	void				PickIndexedLines			(LXAxis& rayLCS, LXActor* pMesh, LXPrimitive* Primitive );
	void				PickLines					(LXAxis& rayLCS, LXActor* pMesh, LXPrimitive* Primitive );
	void				PickIndexedTriangles		(LXAxis& rayLCS, LXActor* pMesh, LXPrimitive* Primitive, LXMatrix* MatrixWCS);
	void				PickTriangles				(LXAxis& rayLCS, LXActor* pMesh, LXPrimitive* Primitive, LXMatrix* MatrixWCS);
	void				PickIndexedTriangleStrip	(LXAxis& rayLCS, LXActor* pMesh, LXPrimitive* Primitive, LXMatrix* MatrixWCS);
	void				PickTriangleStrip			(LXAxis& rayLCS, LXActor* pMesh, LXPrimitive* Primitive, LXMatrix* MatrixWCS);

private:

	void				PickPrimitive				(LXActor* Actor, LXPrimitive* Primitive, LXMatrix* MatrixWCS);
	void				PickPrimitiveInstance		(LXActor* pMesh, LXPrimitive* pPrimitive, LXMatrix* MatrixWCS, LXAxis& rayLCS);
	void				AddPointOfInterest			(float fDistance, LXActor* pMesh, LXPrimitive* Primitive, const vec3f& nearest, const wchar_t* Method);

public:

	LXWorldTransformation* WordTransformation = nullptr;
	
protected:

	LXAxis				m_ray;
	MapIntersection		m_mapIntersection;

private:

	uint				m_nTestedBoxes = 0;
	uint				m_nHitBoxes = 0;
	uint				m_nTestedPrimitivesBoxes = 0;
	uint				m_nHitPrimitivesBoxes = 0;
	uint				m_nTestedTriangles = 0;
	uint				m_nHitTriangles = 0;
	
};
 