//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXTrackBallCameraManipulator.h"
#include "LXActorCamera.h"
#include "LXAnimation.h"
#include "LXAnimationManager.h"
#include "LXCommandManager.h"
#include "LXEngine.h"
#include "LXMath.h"
#include "LXProject.h"
#include "LXViewport.h"

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
	LXProject* project = GetEngine().GetProject();
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
		double dx, dy, dz;
		float xScale = _wtPicked.Width() / _Viewport->GetWidth();
		float yScale = _wtPicked.Height() / _Viewport->GetHeight();
		_wtPicked.UnProject((double)x * xScale, (_Viewport->GetHeight() - (double)y) * yScale, 0.999f, dx, dy, dz);
		_vPickedPoint.Set((float)dx, (float)dy, (float)dz);
		
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
	bool somethingHasChanged = false;
	if (GetCamera())
	{
		somethingHasChanged |= RotateCamera(dFrameTime);
		somethingHasChanged |= DollyCamera(dFrameTime);
		somethingHasChanged |= PanCamera(dFrameTime);
	}
	return somethingHasChanged;
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
		double t = dFrameTime /  125.0;
		if (t > 1.0)
			_rotateStart = _rotateEnd;
		else
			_rotateStart += mouseChange * (float)t;

		// Apply to Camera
		LXScene* pScene = GetScene();
		LXProject* pDocument = GetDocument();

		float rx = _rotateStart.x - _pntWndRotate.x;
		float ry = _rotateStart.y - _pntWndRotate.y;
		_pntWndRotate.x = _rotateStart.x;
		_pntWndRotate.y = _rotateStart.y;

		const float rotationSpeed = 2.5f;

		rx /= rotationSpeed;
		ry /= rotationSpeed;

		vec3f rotation = pCamera->GetRotation() + vec3f(0.f, ry, -rx);
		LXProperty* propertyRotation = pCamera->GetProperty(LXPropertyID::ROTATION);
		GetEngine().GetCommandManager().PreviewChangeProperty(propertyRotation, rotation);
		
		if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
		{
			return true;
		}
		
		// Position to 'picked' CS
		vec3f localPosiion = pCamera->GetPosition() - _vPickedPoint;

		// Rotate around world z axis
		LXMatrix matrixRotation;
		matrixRotation.SetRotation(LX_DEGTORAD(rx), 0.f, 0.f, 1.f);
			
		// Rotate around Camera y axis
		LXMatrix matrixRotation2;
		vec3f right = pCamera->GetMatrix().GetVy();
		matrixRotation2.SetRotation(LX_DEGTORAD(ry), right.x, right.y, right.z);
		vec3f newPosition = matrixRotation2 * matrixRotation * localPosiion;
				
		// Back to world CS
		newPosition += _vPickedPoint;

		LXProperty* propertyPosition = pCamera->GetProperty(LXPropertyID::POSITION);
		GetEngine().GetCommandManager().PreviewChangeProperty(propertyPosition, newPosition);
		
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
		double t = dFrameTime / 92.5;
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

		LXProperty* propertyPosition = pCamera->GetProperty(LXPropertyID::POSITION);
		GetEngine().GetCommandManager().PreviewChangeProperty(propertyPosition, Pos);

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
