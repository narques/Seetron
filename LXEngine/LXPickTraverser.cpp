//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXPickTraverser.h"
#include "LXBBox.h"
#include "LXActor.h"
#include "LXActorMesh.h"
#include "LXPerformance.h"
#include "LXCore.h"
#include "LXActorLine.h"
#include "LXScene.h"
#include "LXTerrain.h"
#include "LXActorLight.h"
#include "LXActorCamera.h"
#include "LXAnchor.h"
#include "LXWorldTransformation.h"
#include "LXPrimitiveInstance.h"
#include "LXLogger.h"
#include "LXPrimitive.h"
#include "LXMaterial.h"
#include "LXStatistic.h"
#include "LXMemory.h" // --- Must be the last included ---

LXPickTraverser::LXPickTraverser(void)
{
}

LXPickTraverser::~LXPickTraverser(void)
{
}

LXMatrix* GetMatrixWCS(LXActorMesh* ActorMesh, LXPrimitiveInstance* PrimitiveInstance)
{
	if (PrimitiveInstance->Matrix)
		return PrimitiveInstance->Matrix;
	else
		return &const_cast<LXMatrix&>(ActorMesh->GetMatrixWCS());
}

class Segment
{
public:
	Segment(const vec3f& p0, const vec3f& p1 ):P0(p0), P1(p1){ }
	vec3f P0, P1;
};

class Line
{
public:
	Line(const vec3f& p0, const vec3f& p1 ):P0(p0), P1(p1){ }
	vec3f P0, P1;
};

void nearestPoint( const Line& L, const vec3f& P, vec3f& oNearestPoint )
{
	vec3f v = L.P1 - L.P0;
	vec3f w = P - L.P0;
	double c1 = Dot(w,v);
	double c2 = Dot(v,v);
	double b = c1 / c2;
	oNearestPoint = L.P0 + (float)b * v;
}

// dist3D_Segment_to_Segment(): get the 3D minimum distance between 2 segments
//    Input:  two 3D line segments S1 and S2
//    Return: the shortest distance between S1 and S2
// Source : http://geomalgorithms.com/a07-_distance.html

float dist3D_Segment_to_Segment( const Segment& S1, const Segment& S2, vec3f& oCL1, vec3f& oCL2)
{
	vec3f   u = S1.P1 - S1.P0;
	vec3f   v = S2.P1 - S2.P0;
	vec3f   w = S1.P0 - S2.P0;
	float    a = Dot(u,u);         // always >= 0
	float    b = Dot(u,v);
	float    c = Dot(v,v);         // always >= 0
	float    d = Dot(u,w);
	float    e = Dot(v,w);
	float    D = a*c - b*b;        // always >= 0
	float    sc, sN, sD = D;       // sc = sN / sD, default sD = D >= 0
	float    tc, tN, tD = D;       // tc = tN / tD, default tD = D >= 0

	// compute the line parameters of the two closest points
	if (D < LX_SMALL_NUMBER6) { // the lines are almost parallel
		sN = 0.0;         // force using point P0 on segment S1
		sD = 1.0;         // to prevent possible division by 0.0 later
		tN = e;
		tD = c;
	}
	else {                 // get the closest points on the infinite lines
		sN = (b*e - c*d);
		tN = (a*e - b*d);
		if (sN < 0.0) {        // sc < 0 => the s=0 edge is visible
			sN = 0.0;
			tN = e;
			tD = c;
		}
		else if (sN > sD) {  // sc > 1  => the s=1 edge is visible
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}

	if (tN < 0.0) {            // tc < 0 => the t=0 edge is visible
		tN = 0.0;
		// recompute sc for this edge
		if (-d < 0.0)
			sN = 0.0;
		else if (-d > a)
			sN = sD;
		else {
			sN = -d;
			sD = a;
		}
	}
	else if (tN > tD) {      // tc > 1  => the t=1 edge is visible
		tN = tD;
		// recompute sc for this edge
		if ((-d + b) < 0.0)
			sN = 0;
		else if ((-d + b) > a)
			sN = sD;
		else {
			sN = (-d +  b);
			sD = a;
		}
	}
	// finally do the division to get sc and tc
	sc = (abs(sN) < LX_SMALL_NUMBER6 ? 0.0f : sN / sD);
	tc = (abs(tN) < LX_SMALL_NUMBER6 ? 0.0f : tN / tD);

	// get the difference of the two closest points
	vec3f   dP = w + (sc * u) - (tc * v);  // =  S1(sc) - S2(tc)

	oCL1 = S1.P0 + sc * u;
	oCL2 = S2.P0 + tc * v;

	//return norma(dP);   // return the closest distance
	return dP.Length();
}

bool IntersectRayTriangle(LXAxis& ray, const vec3f& p0, const vec3f& p1, const vec3f& p2, vec3f& pI)
{
	// Triangle edge vectors
	vec3f u = p1 - p0;
	vec3f v = p2 - p0;

	// Plane normal
	vec3f n;
	n.CrossProduct(u,v);

	
	vec3f dir = ray.GetVector();
	vec3f w0 = ray.GetOrigin() - p0;
	float a = - n.DotProduct(w0);
	float b = n.DotProduct(dir);
	
	// 1e-6 is not enought efficient, set to 1e-7
	// TODO: test with 1e-8 if it rejects less triangles
	if (fabs(b) < 0.00000001 /*LX_SMALL_NUMBER6*/)
	{
		// ray is parallel to triangle plane
		if (a == 0)
			return false;    // ray lies in triangle plane
		else 
			return false;    // ray disjoint from plane
	}

	// get intersect point of ray with triangle plane
	float r = a / b;
	if (r < 0.0)                   // ray goes away from triangle
		return false;                  // => no intersect
	// for a segment, also test if (r > 1.0) => no intersect

	pI = ray.GetOrigin() + r * dir;           // intersect point of ray and plane

	// is I inside T?
	float    uu, uv, vv, wu, wv, D;
	uu = u.DotProduct(u);
	uv = u.DotProduct(v);
	vv = v.DotProduct(v);
	vec3f w = pI - p0;
	wu = w.DotProduct(u);
	wv = w.DotProduct(v);
	D = uv * uv - uu * vv;

	// get and test parametric coords
	float s, t;
	s = (uv * wv - vv * wu) / D;
	if (s < 0.0 || s > 1.0)        // I is outside T
		return false;
	t = (uv * wu - uu * wv) / D;
	if (t < 0.0 || (s + t) > 1.0)  // I is outside T
		return false;

	return true;
}

bool IntersectRayQuad(LXAxis& ray, vec3f& p0, vec3f& p1, vec3f& p2, vec3f& p3, vec3f& pI)
{
	if (IntersectRayTriangle(ray, p0, p1, p2, pI))
		return true;
	else
		return IntersectRayTriangle(ray, p1, p2, p3, pI);
}

bool IntersectRayBox(LXAxis& ray, const LXBBox& box, vec3f& pI)
{
	// La B
	if (box.IsPoint())
		return true;

// 	if (box.IsLine())
// 		return true;

	vec3f p[8];
	box.GetPoints(p);

	if (IntersectRayQuad(ray, p[0], p[1], p[2], p[3], pI))
		return true;

	if (IntersectRayQuad(ray, p[4], p[5], p[6], p[7],pI))
		return true;

	if (IntersectRayQuad(ray, p[4], p[0], p[5], p[1],pI))
		return true;

	if (IntersectRayQuad(ray, p[6], p[2], p[7], p[3],pI))
		return true;

	if (IntersectRayQuad(ray, p[5], p[1], p[7], p[3],pI))
		return true;

	if (IntersectRayQuad(ray, p[4], p[0], p[6], p[2],pI))
		return true;

	return false;
}

/*virtual*/
void LXPickTraverser::Apply( )
{
	LX_PERFOSCOPE(LXPickTraverser_Apply)
	m_mapIntersection.clear();
	m_nTestedBoxes = 0;
	m_nTestedBoxes = 0;
	m_nTestedPrimitivesBoxes = 0;
	m_nHitPrimitivesBoxes = 0;
	m_nTestedTriangles = 0;
	m_nHitTriangles = 0;
	LXTraverser::Apply();
	//LogD(PickTraverser, L"Intersected(hit):  %i(%i) Actor BBoxes %i(%i) Primitive BBoxes %i(%i) Triangles", m_nTestedBoxes, m_nHitBoxes,
	//	m_nTestedPrimitivesBoxes, m_nHitPrimitivesBoxes,
	//	m_nTestedTriangles, m_nHitTriangles);
}

void LXPickTraverser::OnActor(LXActor* pGroup)
{
	CHK(pGroup);

	if (!pGroup->IsPickable())
		return;

	if (pGroup->IsVisible())
	{
		// The Scene is always Pickable
		// As some of his childs must be pickable from the outside
		if (dynamic_cast<LXScene*>(pGroup))
		{
			LXTraverser::OnActor(pGroup);
			return; 
		}
//		else if (dynamic_cast<LXActorLight*>(pGroup))
//		{
//			LXTraverser::OnActor(pGroup);
//			return;
//		}
#if LX_ANCHOR
		else if (dynamic_cast<LXAnchor*>(pGroup))
		{
			LXTraverser::OnActor(pGroup);
			return;
		}
#endif
		else if (dynamic_cast<LXActorCamera*>(pGroup))
		{
			LXTraverser::OnActor(pGroup);
			return;
		}
				
		// World Axis to LocalAxis
		LXAxis rayLCS = m_ray;
		pGroup->GetMatrixWCS().ParentToLocal(rayLCS);
		
		vec3f pI;
		m_nTestedBoxes++;
		if (IntersectRayBox(m_ray, pGroup->GetBBoxWorld(), pI))
		{
			m_nHitBoxes++;
			LXTraverser::OnActor(pGroup);
		}
	}
}

void LXPickTraverser::OnPrimitive(LXActorMesh* pMesh, LXWorldPrimitive* WorldPrimitive)
{
	LXPrimitive* Primitive = WorldPrimitive->PrimitiveInstance->Primitive.get();
	LXMatrix* MatrixWCS = &WorldPrimitive->MatrixWorld;
	
	LXAxis rayLCS = m_ray;
		
	vec3f pI;
	m_nTestedPrimitivesBoxes++;
	if (IntersectRayBox(m_ray, WorldPrimitive->BBoxWorld, pI))
	{
		m_nHitPrimitivesBoxes++;
		PickPrimitive(pMesh, Primitive, MatrixWCS);
	}
}

void LXPickTraverser::SetRay(LXAxis& ray)
{
	m_ray = ray; 
}

void LXPickTraverser::PickPrimitive(LXActor* pMesh, LXPrimitive* pPrimitive, LXMatrix* MatrixWCS)
{
#ifdef LX_DEBUG_PRIMITIVE_PROPVISIBLE
	if (!pPrimitive->_bVisible)
		return;
#endif
		
	if (LXActorMesh* ActorMesh = dynamic_cast<LXActorMesh*>(pMesh))
	{
		if (ActorMesh->GetArrayInstancePosition().size() > 0)
		{
			for (const vec3f& Position : ActorMesh->GetArrayInstancePosition())
			{
				LXMatrix LXMatrixInstanceWCS = *MatrixWCS;
				LXMatrixInstanceWCS.Translate(Position);
				LXAxis rayLCS = m_ray;
				LXMatrixInstanceWCS.ParentToLocal(rayLCS);
				PickPrimitiveInstance(pMesh, pPrimitive, &LXMatrixInstanceWCS, rayLCS);
			}
		}
		else
		{
			LXAxis rayLCS = m_ray;
			MatrixWCS->ParentToLocal(rayLCS);
			PickPrimitiveInstance(pMesh, pPrimitive, MatrixWCS, rayLCS);
		}
	}
}

void LXPickTraverser::PickPrimitiveInstance(LXActor* pMesh, LXPrimitive* pPrimitive, LXMatrix* MatrixWCS, LXAxis& rayLCS)
{
	ArrayUint& arrayIndices = pPrimitive->GetArrayIndices();

	if (arrayIndices.size())
	{
		if (pPrimitive->GetTopology() == LX_LINE_LOOP)
		{
			PickIndexedLineLoop(rayLCS, pMesh, pPrimitive);
		}
		else if ((pPrimitive->GetTopology() == LX_TRIANGLES) || (pPrimitive->GetTopology() == LX_3_CONTROL_POINT_PATCH))
		{
			PickIndexedTriangles(rayLCS, pMesh, pPrimitive, MatrixWCS);
		}
		else if (pPrimitive->GetTopology() == LX_TRIANGLE_STRIP)
		{
			PickIndexedTriangleStrip(rayLCS, pMesh, pPrimitive, MatrixWCS);
		}
		else
		{
			CHK(0);
		}
	}
	else
	{
		if (pPrimitive->GetTopology() == LX_POINTS)
		{
			PickPoints(rayLCS, pMesh, pPrimitive);
		}
		else if (pPrimitive->GetTopology() == LX_LINE_LOOP)
		{
			PickLineLoop(rayLCS, pMesh, pPrimitive);
		}
		else if (pPrimitive->GetTopology() == LX_TRIANGLE_STRIP)
		{
			PickTriangleStrip(rayLCS, pMesh, pPrimitive, MatrixWCS);
		}
		else if ((pPrimitive->GetTopology() == LX_TRIANGLES) || (pPrimitive->GetTopology() == LX_3_CONTROL_POINT_PATCH))
		{
			PickTriangles(rayLCS, pMesh, pPrimitive, MatrixWCS);
		}
		else if (pPrimitive->GetTopology() == LX_LINES)
		{
			PickLines(rayLCS, pMesh, pPrimitive);
		}
		else
		{
			CHK(0);
		}
	}
}

void LXPickTraverser::PickPoints(LXAxis& rayLCS, LXActor* pMesh, LXPrimitive* pPrimitive)
{
	 ArrayVec3f& arrayPosition = pPrimitive->GetArrayPositions();

	for (int i = 0; i < (int)arrayPosition.size(); i++)
	{
		vec3f& p0 = arrayPosition[i];
		vec3f nearest;
		nearestPoint(Line(rayLCS.GetOrigin(), rayLCS.GetOrigin() + rayLCS.GetVector() * 50000.f), p0, nearest);

		if (WordTransformation && WordTransformation->IsValid())
		{
			vec3f vaScreen = p0;		// Point on ray
			vec3f vbScreen = nearest;	// Point on ? p0 ?
			WordTransformation->Project(vaScreen);
			WordTransformation->Project(vbScreen);
			float dist = vaScreen.Distance(vbScreen);
			if (dist < 10.f)
			{
				float fDistance = 0.f; //TODO Point Priority
				pMesh->GetMatrixWCS().LocalToParentPoint(nearest);
				AddPointOfInterest(fDistance, pMesh, pPrimitive, nearest, L"PickPoints");
			}
		}
	}
}

void LXPickTraverser::PickLineLoop(LXAxis& rayLCS, LXActor* pMesh, LXPrimitive* pPrimitive)
{
	ArrayVec3f& arrayPosition = pPrimitive->GetArrayPositions();

	for (int i = 0; i < (int)arrayPosition.size(); i++)
	{
		vec3f& p0 = arrayPosition[i];
		int ip1 = i + 1;

		// Close the loop
		if (i == arrayPosition.size() - 1)
			ip1 = 0;

		vec3f& p1 = arrayPosition[ip1];

		vec3f v = p1 - p0;
		LXAxis edge;
		edge.Set(p0, v);

		vec3f va, vb;

		// TODO dist_Ray_Segment
		dist3D_Segment_to_Segment(Segment(rayLCS.GetOrigin(), rayLCS.GetOrigin() + rayLCS.GetVector() * 50000.f), Segment(p0, p1), va, vb);
		if (WordTransformation && WordTransformation->IsValid())
		{
			vec3f vaScreen = va; // Point on Ray
			vec3f vbScreen = vb; // Point on Segment
			WordTransformation->Project(vaScreen);
			WordTransformation->Project(vbScreen);
			float dist = vaScreen.Distance(vbScreen);
			if (dist < 5.f)
			{
				//float fDistance = vb.Distance(rayLCS.GetOrigin()); 
				float fDistance = 1.f; //Line Priority // 
				pMesh->GetMatrixWCS().LocalToParentPoint(va);
				AddPointOfInterest(fDistance, pMesh, pPrimitive, va, L"PickLineLoop");
				
				// We save the picked segment ID for a future usage.
				if (LXActorLine* pLine = dynamic_cast<LXActorLine*>(pMesh))
				{
					pLine->SetPickedID(i, va);
				}
			}
		}
	}
}

void LXPickTraverser::PickIndexedLineLoop(LXAxis& rayLCS, LXActor* pMesh, LXPrimitive* )
{
	CHK(0);
}

void LXPickTraverser::PickIndexedLines(LXAxis& rayLCS, LXActor* pMesh, LXPrimitive* )
{
	CHK(0);
}

void LXPickTraverser::PickLines(LXAxis& rayLCS, LXActor* pMesh, LXPrimitive* pPrimitive)
{
	ArrayVec3f& arrayPosition = pPrimitive->GetArrayPositions();

	for (int i = 0; i < (int)arrayPosition.size(); i+=2)
	{
		vec3f& p0 = arrayPosition[i];
		vec3f& p1 = arrayPosition[i+1];
		
		LXAxis edge;
		edge.Set(p0, p1);

		vec3f va, vb;

		// TODO dist_Ray_Segment
		dist3D_Segment_to_Segment(Segment(rayLCS.GetOrigin(), rayLCS.GetOrigin() + rayLCS.GetVector() * 50000.f), Segment(p0, p1), va, vb);

		if (WordTransformation && WordTransformation->IsValid())
		{
			vec3f vaScreen = va; // Point on Ray
			vec3f vbScreen = vb; // Point on Segment
			WordTransformation->Project(vaScreen);
			WordTransformation->Project(vbScreen);
			float dist = vaScreen.Distance(vbScreen);
			if (dist < 25.f)
			{
				//float fDistance = vb.Distance(rayLCS.GetOrigin()); 
				float fDistance = 1.f; // Line Priority // 
				pMesh->GetMatrixWCS().LocalToParentPoint(va);
				AddPointOfInterest(fDistance, pMesh, pPrimitive, va, L"PickLines");

				// We save the picked segment ID for a future usage.
				if (LXActorLine* pLine = dynamic_cast<LXActorLine*>(pMesh))
				{
					pLine->SetPickedID(i, va);
				}
			}
		}
	}
}

void LXPickTraverser::PickIndexedTriangles(LXAxis& rayLCS, LXActor* pMesh, LXPrimitive* pPrimitive, LXMatrix* MatrixWCS)
{
	const ArrayVec3f& arrayPosition = pPrimitive->GetArrayPositions();
	const ArrayUint& arrayIndices = pPrimitive->GetArrayIndices();
	
	 
	for (int i = 0; i < (int)arrayIndices.size(); i += 3)
	{
		uint a = arrayIndices[i];
		uint b = arrayIndices[i + 1];
		uint c = arrayIndices[i + 2];

		const vec3f& v0 = arrayPosition[a];
		const vec3f& v1 = arrayPosition[b];
		const vec3f& v2 = arrayPosition[c];

		if (LXActorTerrain* Terrain = dynamic_cast<LXActorTerrain*>(pMesh))
		{
			vec3f w0, w1, w2;
			w0 = v0;
			w1 = v1;
			w2 = v2;
			
			MatrixWCS->LocalToParentPoint(w0);
			MatrixWCS->LocalToParentPoint(w1);
			MatrixWCS->LocalToParentPoint(w2);
 
			w0.z = Terrain->GetHeightAt(w0.x, w0.y);
			w1.z = Terrain->GetHeightAt(w1.x, w1.y);
			w2.z = Terrain->GetHeightAt(w2.x, w2.y);

			MatrixWCS->ParentToLocalPoint(w0);
			MatrixWCS->ParentToLocalPoint(w1);
			MatrixWCS->ParentToLocalPoint(w2);

			vec3f pI;
			m_nTestedTriangles++;

			if (IntersectRayTriangle(rayLCS, w0, w1, w2, pI))
			{
				m_nHitTriangles++;
				MatrixWCS->LocalToParentPoint(pI);
				float fDistance = pI.Distance(m_ray.GetOrigin());
				{
					AddPointOfInterest(fDistance, pMesh, pPrimitive, pI, L"PickIndexedTriangles");
				}
			}
		}
		else
		{
			vec3f pI;
			m_nTestedTriangles++;

			if (IntersectRayTriangle(rayLCS, v0, v1, v2, pI))
			{
				m_nHitTriangles++;
				MatrixWCS->LocalToParentPoint(pI);
				float fDistance = pI.Distance(m_ray.GetOrigin());
				AddPointOfInterest(fDistance, pMesh, pPrimitive, pI, L"PickIndexedTriangles");
			}
		}
	}
}

void LXPickTraverser::PickTriangles(LXAxis& rayLCS, LXActor* pMesh, LXPrimitive* pPrimitive, LXMatrix* MatrixWCS)
{
	ArrayVec3f& arrayPosition = pPrimitive->GetArrayPositions();

	for (int i = 0; i < (int)arrayPosition.size(); i += 3)
	{
		vec3f& v0 = arrayPosition[i];
		vec3f& v1 = arrayPosition[i + 1];
		vec3f& v2 = arrayPosition[i + 2];
		vec3f pI;

		if (IntersectRayTriangle(rayLCS, v0, v1, v2, pI))
		{
			MatrixWCS->LocalToParentPoint(pI);
			float fDistance = pI.Distance(m_ray.GetOrigin());
			AddPointOfInterest(fDistance, pMesh, pPrimitive, pI, L"PickTriangles");
		}
	}
}

void LXPickTraverser::PickIndexedTriangleStrip(LXAxis& rayLCS, LXActor* pMesh, LXPrimitive* pPrimitive, LXMatrix* MatrixWCS)
{
	ArrayVec3f& arrayPosition = pPrimitive->GetArrayPositions();
	ArrayUint& arrayIndices = pPrimitive->GetArrayIndices();

	for (int i = 0; i < (int)arrayIndices.size() -2; i++)
	{
		uint a = arrayIndices[i];
		uint b = arrayIndices[i + 1];
		uint c = arrayIndices[i + 2];

		vec3f& v0 = arrayPosition[a];
		vec3f& v1 = arrayPosition[b];
		vec3f& v2 = arrayPosition[c];

		vec3f pI;
		m_nTestedTriangles++;

		if (IntersectRayTriangle(rayLCS, v0, v1, v2, pI))
		{
			m_nHitTriangles++;
			MatrixWCS->LocalToParentPoint(pI);
			float fDistance = pI.Distance(m_ray.GetOrigin());
			AddPointOfInterest(fDistance, pMesh, pPrimitive, pI, L"PickIndexedTriangleStrip");
		}
	}
}

void LXPickTraverser::PickTriangleStrip(LXAxis& rayLCS, LXActor* pMesh, LXPrimitive* pPrimitive, LXMatrix* MatrixWCS)
{
	ArrayVec3f& arrayPosition = pPrimitive->GetArrayPositions();

	for (int i = 0; i < (int)arrayPosition.size() - 2; i++)
	{
		vec3f& v0 = arrayPosition[i];
		vec3f& v1 = arrayPosition[i + 1];
		vec3f& v2 = arrayPosition[i + 2];
		vec3f pI;

		if (IntersectRayTriangle(rayLCS, v0, v1, v2, pI))
		{
			MatrixWCS->LocalToParentPoint(pI);
			float fDistance = pI.Distance(m_ray.GetOrigin());
			AddPointOfInterest(fDistance, pMesh, pPrimitive, pI, L"PickTriangleStrip");
		}
	}
}

LXActor* LXPickTraverser::GetNearestNode( )
{
	if (m_mapIntersection.size())
	{
		MapIntersection::iterator It = m_mapIntersection.begin();
		return It->second.node;
	}
	else
		return NULL;
}

LXPrimitive* LXPickTraverser::GetNearestPrimitive()
{
	if (m_mapIntersection.size())
	{
		MapIntersection::iterator It = m_mapIntersection.begin();
		return It->second.primitive;
	}
	else
		return NULL;

}

float LXPickTraverser::GetNearestDepth( )
{
	if (m_mapIntersection.size())
	{
		MapIntersection::iterator It = m_mapIntersection.begin();
		return It->first;
	}
	else
		return 0.0f;
}

vec3f* LXPickTraverser::GetNearestPOI( )
{
	if (m_mapIntersection.size())
	{
		MapIntersection::iterator It = m_mapIntersection.begin();
		return &It->second.point;
	}
	else
	{
		return NULL;
	}	
}

vec3f* LXPickTraverser::GetNearestPOIOnNode( LXActor* pNode )
{
	for (auto It=m_mapIntersection.rbegin(); It!=m_mapIntersection.rend(); It++)
	{
		if (It->second.node == pNode)
			return &It->second.point;
	}
	return NULL;
}

void LXPickTraverser::AddPointOfInterest(float fDistance, LXActor* Actor, LXPrimitive* Primitive, const vec3f& nearest, const wchar_t* Method)
{
	// "Rescale" the Gizmo picked distances to make it a priority 
	if (Actor->GetCID() & LX_NODETYPE_CS)
	{
		fDistance = SmoothStep(fDistance, 0.f, 10.f);
	}
	
	m_mapIntersection[fDistance] = LXPOI(Actor, Primitive, nearest);
	//LogD(PickTraverser, L"Picked %s distance %f (method %s)", Actor->GetName().GetBuffer(), fDistance, Method);
}
