//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXTrackBallCameraManipulator.h"
#include "LXProject.h"
#include "LXViewport.h"
#include "LXWorldTransformation.h"
#include "LXActorCamera.h"
#include "LXScene.h"
#include "LXPropertyManager.h"
#include "LXProject.h"
#include "LXAnimationManager.h"
#include "LXMemory.h" // --- Must be the last included ---

#define MK_LBUTTON          0x0001
#define MK_RBUTTON          0x0002
#define MK_SHIFT            0x0004
#define MK_CONTROL          0x0008
#define MK_MBUTTON          0x0010

LXTrackBallCameraManipulator::LXTrackBallCameraManipulator(void):
_bMouseHasMoved(false),
_bManipulated(false),
_bExtendedSelection(false),
_rotateStart(0.f, 0.f),
_rotateEnd(0.f, 0.f),
_panStart(0.f, 0.f),
_panEnd(0.f, 0.f),
_dollyStart(0.f),
_dollyEnd(0.f),
_bPickedPoint(false)
{
}

LXTrackBallCameraManipulator::~LXTrackBallCameraManipulator(void)
{
}

LXActorCamera* LXTrackBallCameraManipulator::GetCamera()
{
	LXProject* project = GetCore().GetProject();
	if (!project)
		return nullptr;
	
	return project->GetCamera();
}

LXScene* LXTrackBallCameraManipulator::GetScene()
{
	CHK(_Viewport);
	if (!_Viewport)
		return NULL;

	LXProject* pDocument = _Viewport->GetDocument();
	if (!pDocument)
		return NULL;

	return pDocument->GetScene();
}

LXProject* LXTrackBallCameraManipulator::GetDocument()
{
	CHK(_Viewport);
	if (!_Viewport)
		return NULL;

	return _Viewport->GetDocument();
}

void LXTrackBallCameraManipulator::OnButtonDown( LXMouseButton eButton, int x, int y, bool Picked, const vec3f& POI )
{
	if (!_Viewport->WorldTransformation.IsValid())
		return;

	_pntWnd.x = (float)x;
	_pntWnd.y = (float)y;
	_wtPicked = _Viewport->WorldTransformation;

	_bPickedPoint = Picked;

	if (_bPickedPoint)
	{
		_vPickedPoint = POI;
		vec3f p = _vPickedPoint;
		_wtPicked.Project(p);
		_fPickedZ = p.z;
	}
	else
	{
// 		m_fPickedZ = 0.99999f;
// 		double dx, dy, dz;
// 		float xScale = _wtPicked.Width() / pViewport->GetWidth();
// 		float yScale = _wtPicked.Height() / pViewport->GetHeight();
// 		_wtPicked.UnProject(x * xScale, (pViewport->GetHeight() - y) * yScale, m_fPickedZ, dx, dy, dz);
// 		_vPickedPoint.Set((float)dx, (float)dy, (float)dz);

//			else if (m_pDocument->GetSelectionManager().HasMeshs())
//				pCenter = &m_pDocument->GetSelectionManager().GetBBox().GetCenter();
	//else
		//pCenter = (const vec3f*)(&pScene->GetBBoxWorld().GetCenter());


//		LXProject* Project = _Viewport->GetDocument();
		//_vPickedPoint = vec3f(-300.f, 40.f, 60.f);// Project->GetScene()->GetBBoxWorld().GetCenter();
		_vPickedPoint = vec3f(0.f, 0.f, 0.f);
		vec3f p = _vPickedPoint;
		_wtPicked.Project(p);
		_fPickedZ = p.z;
		_bPickedPoint = true;
	}
	
	_bMouseHasMoved = false;
	_bManipulated = true;

	// Rotate
	_rotateStart = _rotateEnd = vec2f((float)x, (float)y);
	_pntWndRotate = _pntWnd;

	// Pan 
	_panStart = _panEnd = vec2f((float)x, (float)y);
	_vPickedPointPan = _vPickedPoint;
	_wtPickedPan = _wtPicked;
	_fPickedZPan = _fPickedZ;

	// Dolly		
	_dollyStart = _dollyEnd = (float)y;
}

void LXTrackBallCameraManipulator::OnButtonUp( LXMouseButton eButton, int x, int y )
{
	_bManipulated = false;
	if (_bMouseHasMoved)
	{
		CHK(_Viewport);
		if (!_Viewport)
			return;
	}

	_bExtendedSelection = false;
}

bool LXTrackBallCameraManipulator::OnMouseMove( uint64 nFlags, int x, int y )
{
	CHK(_Viewport);
	if (!_Viewport)
		return false;

	if (!_bManipulated)
		return false;
	
	_bMouseHasMoved = true;
		
	//
	// Pan 
	//
	
	if (!(nFlags & MK_LBUTTON) && !(nFlags & MK_RBUTTON) && (nFlags & MK_MBUTTON))
	{
		_panEnd = vec2f((float)x, (float)y);
		return true;
	}
	
	//
	// Dolly
	//

// 	if ((nFlags & MK_LBUTTON) && (nFlags & MK_RBUTTON) && !(nFlags & MK_MBUTTON))
// 	{
// 		m_dollyEnd = (float)y;
// 		pViewport->Invalidate(LX_INVALIDATE_POINTOFVIEW);
// 		return true;
// 	}
		
	//
	// Orbit, Rotate
	//

	if (!(nFlags & MK_LBUTTON) && (nFlags & MK_RBUTTON)  && !(nFlags & MK_MBUTTON))
	{
		_rotateEnd = vec2f((float)x, (float)y);
		return true;
	}

	//
	// Selection
	//
	
	if ((nFlags & MK_LBUTTON) && !(nFlags & MK_RBUTTON) && !(nFlags & MK_MBUTTON))
	{
		_pntCurrentWnd.x = (float)x;
		_pntCurrentWnd.y = (float)y;
		_bExtendedSelection = true;
		return true;
	}

	return false;
}

void LXTrackBallCameraManipulator::OnKeyDown( LXKeyState eKey )
{	
}

void LXTrackBallCameraManipulator::OnKeyUp( LXKeyState eKey )
{
}

void LXTrackBallCameraManipulator::OnMouseWheel(uint16 nFlags, short zDelta, LXPoint pntWnd)
{
	CHK(_Viewport);
	if (!_Viewport)
		return;

	LXActorCamera* pCamera = GetCamera();
	if (!pCamera)
		return;

	LXProject* pDocument = _Viewport->GetDocument();
	CHK(pDocument);
	if (!pDocument)
		return;

	if ((nFlags & MK_LBUTTON) || (nFlags & MK_RBUTTON) || (nFlags & MK_MBUTTON))
		return;
	
	if (_Viewport->PickPoint((float)pntWnd.x, (float)pntWnd.y, _vPickedPointWhell))
	{
		//m_pRenderer->GetPoint(pntWnd.x, pntWnd.y, m_vPickedPointWhell);
		//else
		//m_vPickedPointWhell = LX_VEC3F_NULL;
		//pViewport->GetPoint(pntWnd.x, pntWnd.y, m_vPickedPointWhell);
		
		int nSgn = Sign(zDelta);

		if (nSgn > 0)
			_distanceToMove = pCamera->GetPosition().Distance(_vPickedPointWhell) * 0.5f;
		else
			_distanceToMove = pCamera->GetPosition().Distance(_vPickedPointWhell) * 0.6f;

		LXProperty* pPropPosition = pCamera->GetProperty(LXPropertyID::POSITION);
		LXProperty* pPropTarget = pCamera->GetProperty(LXPropertyID::CAMERA_TARGET);

		vec3f vDir = _vPickedPointWhell - pCamera->GetPosition();
		vDir.Normalize();

		vec3f vNewPosition = pCamera->GetPosition() + vDir * _distanceToMove * (float)nSgn;
		vec3f vNewTarget = *pCamera->GetTarget() + vDir * _distanceToMove * (float)nSgn;

		LXAnimation* pAnimation = new LXAnimation();
		pAnimation->SetDuration(500);
		pAnimation->AddKey(pPropPosition, vNewPosition, 500);
		pAnimation->AddKey(pPropTarget, vNewTarget, 500);
		pDocument->GetAnimationManager().PlayVolatileAnimation(pAnimation);
	}
}

bool LXTrackBallCameraManipulator::Update( double dFrameTime )
{
	bool bSomethingAsChanged = false;
	if (GetCamera())
	{
		bSomethingAsChanged |= RotateCamera(dFrameTime);
		bSomethingAsChanged |= DollyCamera(dFrameTime);
		bSomethingAsChanged |= PanCamera(dFrameTime);
	}
	return bSomethingAsChanged;
}

bool LXTrackBallCameraManipulator::DollyCamera( double dFrameTime )
{
	LXActorCamera* pCamera = GetCamera();
	if (!pCamera)
		return false;

#if 1

	if (_zDelta != 0)
	{

		double t = dFrameTime / 1000.0f; // 1 second
		if (t > 1.0)
		{
			//m_dollyStart = m_dollyEnd;
			_zDelta = 0;
		}
		else
		{
			//m_dollyStart += mouseChange * (float)t;

			_distanceToMove = _distanceToMove * (float)t;
		}

		// Apply to Camera
		if (pCamera->IsOrtho())
		{
			CHK(0);
// 			float fHeight = pCamera->GetHeight();
// 			fHeight += fHeight * m_dollyStart * 0.00005f;
// 			pCamera->SetHeight(fHeight);
		}
		else
		{
// 			float fDistance = pCamera->GetPosition().Distance(m_vPickedPoint);
// 			float f = fDistance * m_dollyStart * 0.0001f;
// 			pCamera->MoveTo(f, m_vPickedPoint);

			
			pCamera->MoveTo(_distanceToMove, _vPickedPointWhell);
		}

		return true;
	}
	else
	{
		return false;
	}

#else

	float mouseChange = _dollyEnd - _dollyStart;
	
	if ( abs(mouseChange) > 1.0f )
	{
		double t = dFrameTime / 60.0f;
		if (t > 1.0)
			_dollyStart = _dollyEnd;
		else
			_dollyStart += mouseChange * (float)t;

		//cout << "DollyCamera dollyStart " << m_dollyStart << endl;
		
		
		// Apply to Camera
		if (pCamera->IsOrtho())
		{
			float fHeight = pCamera->GetHeight();
			fHeight += fHeight * _dollyStart * 0.00005f;
			pCamera->SetHeight(fHeight);
		}
		else
		{
			float fDistance = pCamera->GetPosition().Distance(_vPickedPoint);
			float f = fDistance * _dollyStart * 0.0001f;
			pCamera->MoveTo(f, _vPickedPoint);
		}

		return true;
	}
	else
	{
		_dollyEnd = _dollyStart = 0.f;
		//cout << "DollyCamera finished dollyStart m_dollyEnd" << m_dollyStart << " " << m_dollyEnd << endl << endl;
		return false;
	}

#endif
}

bool LXTrackBallCameraManipulator::RotateCamera( double dFrameTime )
{
	LXActorCamera* pCamera = GetCamera();
	if (!pCamera)
		return false;

	if (pCamera->IsOrtho())  // TODO or CONSTRAINT
		return false;

	vec2f mouseChange = _rotateEnd - _rotateStart;
	float lenght = mouseChange.Length();
	if ( mouseChange.Length() > 1.f )
	{
		
		//TRACE(L"Rotate %f\n", mouseChange.Length());


		double t = dFrameTime /  125.0;
		if (t > 1.0)
			_rotateStart = _rotateEnd;
		else
			_rotateStart += mouseChange * (float)t;

		// Apply to Camera
		LXScene* pScene = GetScene();
		LXProject* pDocument = GetDocument();

		float rx = ((float)(_rotateStart.x - _pntWndRotate.x)) / 150.0f;
		float ry = ((float)(_rotateStart.y - _pntWndRotate.y)) / 150.0f;
		_pntWndRotate.x = _rotateStart.x;
		_pntWndRotate.y = _rotateStart.y;
		vec3f vAxis;
		pCamera->GetUpVector(&vAxis);

		if (0/*m_bRotate*/)
		{
			//pCamera->Rotate(ry, vAxis.x, vAxis.y, vAxis.z);
			//pCamera->Rotate(-rx, 0, 1, 0);
		}
		else
		{
			const vec3f* pCenter;
			pCenter = &_vPickedPoint;
			

			vec3f view;
			pCamera->GetViewVector(&view);
			float angle = CosAngle(view, pDocument->GetUpAxis());
			if (abs(angle - sinf(ry)) > 0.999f)
			{
				float ry2 = asinf(0.999f - abs(angle));
				ry2 *= ry<0.0?-1.0f:1.0f;
				pCamera->OrbitAround(pCenter, ry2, vAxis.x, vAxis.y, vAxis.z);
			}
			else
				pCamera->OrbitAround(pCenter, ry, vAxis.x, vAxis.y, vAxis.z);

			pCamera->OrbitAround(pCenter, -rx, pDocument->GetUpAxis());

					
// 			Phi += ry;
// 			Theta += rx;
// 			
// 			// Clamp Phi within -LX_2PI to LX_2PI
// 			if (Phi > LX_2PI)
// 				Phi -= LX_2PI;
// 			else if (Phi < -LX_2PI)
// 				Phi += LX_2PI;
// 			
// 
// 			// Rotate Position
// 			// To Cartesian
// 			float Radius = pCenter->Distance(pCamera->GetPosition());
// 			float x = Radius * sinf(Phi) * sinf(Theta);
// 			float y = Radius * cosf(Phi);
// 			float z = Radius * sinf(Phi) * cosf(Theta);
// 			float w = 1.f;
// 
// 			vec3f NewPosition = vec3f(x, y, z) + *pCenter;
// 			pCamera->SetPosition(NewPosition);
// 
// 
// 			//Rotate Target
// 			Radius = pCenter->Distance(*pCamera->GetTarget());
// 			x = Radius * sinf(Phi) * sinf(Theta);
// 			y = Radius * cosf(Phi);
// 			z = Radius * sinf(Phi) * cosf(Theta);
// 			w = 1.f;
// 
// 			vec3f NewTarget = vec3f(x, y, z) + *pCenter;
// 			pCamera->SetTarget(NewTarget);



		}

		return true;
	}
	else
		return false;
}

bool LXTrackBallCameraManipulator::PanCamera( double dFrameTime )
{
	vec2f mouseChange = _panEnd - _panStart;
	float lenght = mouseChange.Length();

	if ( mouseChange.Length() > 1.f )
	{
		// Update Pan value
		double t = dFrameTime / 250;// 92.5;
		if (t > 1.0)
			_panStart = _panEnd;
		else
			_panStart += mouseChange * (float)t;
		
		// Apply to Camera
		LXActorCamera* pCamera = GetCamera();
		
		// ScreenSpace to World
		double x, y, z;

		float xScale = _wtPickedPan.Width() / _Viewport->GetWidth();
		float yScale = _wtPickedPan.Height() / _Viewport->GetHeight();
		
		_wtPickedPan.UnProject(_panStart.x * xScale, (_Viewport->GetHeight() - _panStart.y) * yScale, _fPickedZPan, x, y, z);
		vec3f vPickedPoint((float)x, (float)y, (float)z);
		
		vec3f v2 = vPickedPoint - _vPickedPointPan;
		CHK(IsValid(v2));
		
		vec3f Pos = pCamera->GetPosition();
		vec3f* pTar = pCamera->GetTarget();
		*pTar -= v2;
		Pos -= v2;

		CHK(IsValid(*pTar));
		CHK(IsValid(Pos));

		pCamera->SetPosition(Pos);
		_vPickedPointPan = vPickedPoint;
		
		_bAnimatedPan = true;
		return true;
	}
	else
	{
		_bAnimatedPan = false;
		return false;
	}
}
