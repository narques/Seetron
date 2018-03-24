//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXQueryTransform.h"
#include "LXQueryManager.h"
#include "LXProject.h"
#include "LXPropertyManager.h"
#include "LXCore.h"
#include "LXCommandManager.h"
#include "LXActorCamera.h"
#include "LXLogger.h"
#include "LXAxis.h"
#include "LXActorLine.h"
#include "LXAnchor.h"
#include "LXViewState.h"
#include "LXViewport.h"
#include "LXMemory.h" // --- Must be the last included ---

LXQueryTransform::LXQueryTransform() :
_StartPosition(0.f, 0.f),
_EndPosition(0.f, 0.f),
_IsMouseMove(false)
{
}

LXQueryTransform::~LXQueryTransform(void)
{
}

void LXQueryTransform::OnLButtonDown(uint64 nFlags, LXPoint pntWnd, LXActor* Actor)
{
	CHK(Actor);
	if (!Actor)
		return;

	_PickedWT = GetCore().GetViewport()->WorldTransformation;

	_StartPosition = vec2f((float)pntWnd.x, (float)pntWnd.y);

	if (LXActorLine* pLine = dynamic_cast<LXActorLine*>(Actor))
	{
		int segmentID = pLine->GetPickedID();

#if LX_ANCHOR
		LXAnchor* pAnchor0 = pLine->GetAnchor(segmentID);
		LXAnchor* pAnchor1;
		if (segmentID == pLine->GetAnchorCount() - 1) // Loop
			pAnchor1 = pLine->GetAnchor(0);
		else
			pAnchor1 = pLine->GetAnchor(segmentID + 1);

		_listActors.push_back((LXActor*)pAnchor0);
		_listActors.push_back((LXActor*)pAnchor1);
#endif
	}
	else
		_listActors.push_back(Actor);

	_bIsActive = true;
	_IsMouseMove = false;
}

/*virtual*/
void LXQueryTransform::OnMouseMove(uint64 nFlags, LXPoint pntWnd)
{
	CHK(_bIsActive);

	_IsMouseMove = true;
	_EndPosition = vec2f((float)pntWnd.x, (float)pntWnd.y);
	vec2f mouseChange = _EndPosition - _StartPosition;
	float lenght = mouseChange.Length();
	if (mouseChange.Length() > 1.f)
	{
		vec3f vPickedPoint;

		// Axis
		LXAxis axis;
		_PickedWT = GetCore().GetViewport()->WorldTransformation;

		float xScale = _PickedWT.Width() / GetCore().GetViewport()->GetWidth();
		float yScale = _PickedWT.Height() / GetCore().GetViewport()->GetHeight();
		_PickedWT.GetPickAxis(axis, _EndPosition.x * xScale, _EndPosition.y * yScale);

		for (auto It = _listActors.begin(); It != _listActors.end(); It++)
		{
			LXActor* pMesh = *It;

			switch (_constraint)
			{
			case EConstraint::None: // ScreenSpace Dragging
			{
// 				LXAxis plane;
// 				plane.SetOrigin(m_vPickedPoint);
// 				
// 
// 				LXActorCamera* pCamera = GetCore().GetProject()->GetActiveView()->GetCamera();
// 				CHK(pCamera);
//  				if (pCamera)
//  					plane.SetVector(pCamera->GetViewVector());
// 
// 				axis.IntersectPlane(plane, &vPickedPoint);
// 				vec3f v2 = vPickedPoint - m_vPickedPoint;
// 				matWCS.ParentToLocalVector(v2);
// 				
// 				LXMatrix matLocalTrs;
// 				matLocalTrs.SetOrigin(v2);
// 				matLCS = matLCS * matLocalTrs;
			}
			break;

			case EConstraint::Local_X:
			case EConstraint::Local_Y:
			case EConstraint::Local_Z:
			{
				LXAxis v;
				v.SetOrigin(_PickedPoint);
				if (_constraint == EConstraint::Local_X)
					v.SetVector(pMesh->GetVx());
				else if (_constraint == EConstraint::Local_Y)
					v.SetVector(pMesh->GetVy());
				else if (_constraint == EConstraint::Local_Z)
					v.SetVector(pMesh->GetVz());
				vec3f p0;
				axis.Distance(v, &p0, &vPickedPoint);
				vec3f v2 = vPickedPoint - _PickedPoint;
				vec3f NewPosition = pMesh->GetPosition() + v2;
				pMesh->SetPosition(NewPosition);
			}
			break;

			case EConstraint::Local_XY:
			case EConstraint::Local_XZ:
			case EConstraint::Local_YZ:
			{
				LXAxis plane;
				plane.SetOrigin(_PickedPoint);
 
				if (_constraint == EConstraint::Local_XY)
					plane.SetVector(pMesh->GetVz());
				else if (_constraint == EConstraint::Local_XZ)
					plane.SetVector(pMesh->GetVy());
				else if (_constraint == EConstraint::Local_YZ)
					plane.SetVector(pMesh->GetVx());
 
				axis.IntersectPlane(plane, &vPickedPoint);
				vec3f v2 = vPickedPoint - _PickedPoint;
				vec3f NewPosition = pMesh->GetPosition() + v2;
				pMesh->SetPosition(NewPosition);
			}
			break;

			case EConstraint::Local_Rotate_X:
			case EConstraint::Local_Rotate_Y:
			case EConstraint::Local_Rotate_Z:
			{
				vec3f vo = pMesh->GetPosition();
				//vo.z = _PickedPoint.z;

				LXAxis plane;
				plane.SetOrigin(vo/*_PickedPoint*/);

				if (_constraint == EConstraint::Local_Rotate_Z)
					plane.SetVector(pMesh->GetVz());
				else if (_constraint == EConstraint::Local_Rotate_Y)
					plane.SetVector(pMesh->GetVy());
				else if (_constraint == EConstraint::Local_Rotate_X)
					plane.SetVector(pMesh->GetVx());

				if (axis.IntersectPlane(plane, &vPickedPoint))
				{
					vec3f va = _PickedPoint - vo;
					vec3f vb = vPickedPoint - vo;
					va.Normalize();
					vb.Normalize();

					float ca = CosAngle(va, vb);
					if (ca > 1.0)
					{
						int foo = 0;
					}

					ca = Clamp(ca, -1.f, 1.f);
					float a = acosf(ca);
					CHK(IsValid(a));

					vec3f cp = CrossProduct(va, vb);

					if (DotProduct(plane.GetVector(), cp) > 0.f)
					{
						a = -a;
					}

					a = LX_RADTODEG(a);

					vec3f NewRotation;
					if (_constraint == EConstraint::Local_Rotate_Z)
						NewRotation = pMesh->GetRotation() + vec3f(0.f, 0.f, a);
					else if (_constraint == EConstraint::Local_Rotate_Y)
						NewRotation = pMesh->GetRotation() + vec3f(0.f, -a, 0.f);
					else if (_constraint == EConstraint::Local_Rotate_X)
						NewRotation = pMesh->GetRotation() + vec3f(a, 0.f, 0.f);

					pMesh->SetRotation(NewRotation);
				}
			}
			break;

			default:CHK(0);
			}

// 			if (pMesh->GetCID() & LX_NODETYPE_ANCHOR)
// 			{
// 				LXAnchor* pAnchor = (LXAnchor*)pMesh;
// 				pAnchor->GetOwner()->OnAnchorMove(pAnchor, matLCS);
// 			}
// 			else
// 			{
// 				LXPropertyVec3f* pProperty = dynamic_cast<LXPropertyVec3f*>(pMesh->GetProperty(LXPropertyID::POSITION));
// 				CHK(pProperty);
// 				GetCore().GetCommandManager().PreviewChangeProperty(pProperty, matLCS.GetOrigin());
// 			}
		}

		_PickedPoint = vPickedPoint;
	}
}

/*virtual*/
void LXQueryTransform::OnLButtonUp(uint64 nFlags, LXPoint pntWnd)
{
	if (_listActors.size())
	{
		for (auto It = _listActors.begin(); It != _listActors.end(); It++)
		{
			LXActor* Actor = *It;
			if (Actor->GetCID() & LX_NODETYPE_ANCHOR)
			{
#if LX_ANCHOR
				LXAnchor* pAnchor = (LXAnchor*)Actor;
				pAnchor->GetOwner()->OnAnchorReleased(pAnchor);
#endif
			}
			else
			{
				LXPropertyVec3f* pProperty = dynamic_cast<LXPropertyVec3f*>(Actor->GetProperty(LXPropertyID::POSITION));
				CHK(pProperty);
				GetCore().GetCommandManager().ChangeProperty(pProperty);
			}
		}
	}

	_listActors.clear();
	_bIsActive = false;
}

