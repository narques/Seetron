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

class LXComponent;
class LXMatrix;
class LXPrimitive;
class LXWorldTransformation;

struct LXPOI // Point of intersection 
{
	LXPOI() {};
	LXPOI(const LXActor* n, const LXPrimitive* p, const vec3f& v) :node(n), primitive(p), point(v){ }
	const LXActor* node;
	const LXPrimitive* primitive;
	vec3f point;
};

typedef std::map<float, LXPOI> MapIntersection;

class LXCORE_API LXPickTraverser : public LXTraverser
{

public:

	LXPickTraverser(void);
	virtual ~LXPickTraverser(void);

	// Overridden from LXTraverser
	virtual void Apply() override;
	virtual void OnActor(LXActor* actor) override;
	virtual void OnPrimitive(LXActorMesh* actorMesh, LXComponentMesh* componentMesh, LXWorldPrimitive* worldPrimitive) override;
	
	void 				SetRay(const LXAxis& ray);
	
	uint				GetNumberOfIntersections	( ) { return (uint)m_mapIntersection.size(); }
	
	LXActor*			GetNearestNode				( );
	LXPrimitive*		GetNearestPrimitive			( );
	float				GetNearestDepth				( );
	vec3f*				GetNearestPOI				( );
	vec3f*				GetNearestPOIOnNode			( LXActor* pNode );

private:

	void PickPoints(const LXAxis& rayLCS, const LXActor* actor, const LXComponent* component, const LXPrimitive* primitive);
	void PickIndexedLineLoop(const LXAxis& rayLCS, const LXActor* actor, const LXComponent* component, const LXPrimitive* primitive );
	void PickLineLoop(const LXAxis& rayLCS, const LXActor* actor, const LXComponent* component, const LXPrimitive* primitive );
	void PickIndexedLines(const LXAxis& rayLCS, const LXActor* actor, const LXComponent* component, const LXPrimitive* primitive );
	void PickLines(const LXAxis& rayLCS, const LXActor* actor, const LXComponent* component, const LXPrimitive* Primitive );
	void PickIndexedTriangles(const LXAxis& rayLCS, const LXActor* actor, const LXComponent* component, const LXPrimitive* primitive, const LXMatrix* matrixWCS);
	void PickTriangles(const LXAxis& rayLCS, const LXActor* actor, const LXComponent* component, const LXPrimitive* primitive, const LXMatrix* matrixWCS);
	void PickIndexedTriangleStrip(const LXAxis& rayLCS, const LXActor* actor, const LXComponent* component, const LXPrimitive* primitive, const LXMatrix* matrixWCS);
	void PickTriangleStrip(const LXAxis& rayLCS, const LXActor* actor, const LXComponent* component, const LXPrimitive* primitive, const LXMatrix* matrixWCS);

private:

	void PickPrimitive(const LXActor* actor, const LXComponent* component, const LXPrimitive* primitive, const LXMatrix* matrixWCS);
	void PickPrimitiveInstance(const LXActor* actor, const LXComponent* component, const LXPrimitive* primitive, const LXMatrix* matrixWCS, const LXAxis& rayLCS);
	void AddPointOfInterest(float fDistance, const LXActor* actor, const LXComponent* component, const LXPrimitive* primitive, const vec3f& nearest, const wchar_t* method);

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
 