//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXActorMesh.h"
#include "LXActorMeshGizmo.h"
#include "LXAnchor.h"
#include "LXAssetMesh.h"
#include "LXCommandManager.h"
#include "LXConsoleManager.h"
#include "LXCore.h"
#include "LXEventManager.h"
#include "LXFrustum.h"
#include "LXActorLine.h"
#include "LXLogger.h"
#include "LXPerformance.h"
#include "LXPickTraverser.h"
#include "LXPrimitive.h"
#include "LXProject.h"
#include "LXQueryManager.h"
#include "LXRectangularSelectionTraverser.h"
#include "LXScene.h"
#include "LXSelectionManager.h"
#include "LXSettings.h"
#include "LXTrackBallCameraManipulator.h"
#include "LXVec3.h"
#include "LXViewState.h"
#include "LXViewport.h"
#include "LXViewportManager.h"
#include "LXWorldTransformation.h"
#include "LXMemory.h" // --- Must be the last included ---

namespace
{
	const wchar_t PointOfIntersectionObjectName[] = L"PointOfIntersection";
}

LXViewport::LXViewport():
	m_vPickedPoint(0.0f, 0.0f, 0.0f),
	m_fPickedZ(1.0f),
	m_pCamManip(NULL),
	m_nHeight(0), 
	m_nWidth(0),
	m_pPointedPrimitive(NULL),
	m_pPointedMesh(NULL)
{
	LogI(Viewport, L"LXViewport created");
		
	//
	// Manipulator
	//

	m_pCamManip = new LXTrackBallCameraManipulator();
	m_pCamManip->_Viewport = this;

	//
	// To manager
	//

	GetCore().GetViewportManager().AddViewport(this);
}

LXViewport::~LXViewport(void)
{
	delete m_pCamManip;
	GetCore().GetViewportManager().RemoveViewport(this);
}

//
// --- Mouse Events ---
//

/*virtual*/
void LXViewport::OnLButtonDblClk(uint64 nFlags, LXPoint pntWnd)
{
	if(!_pDocument || _pDocument->IsLoading())
		return;
}

/*virtual*/
void LXViewport::OnLButtonDown(uint64 nFlags, LXPoint pntWnd)
{
	if (!_pDocument)
		return;

	ActorPicked = PickActor(pntWnd.x, pntWnd.y, &PointPicked, &PrimitivePicked);
	LXActorMesh* ActorMesh = dynamic_cast<LXActorMesh*>(ActorPicked);
	
	//
	// Query Translation
	//
 
	bool bObjectIsSelected = false; // ActorMesh && GetSelectionManager()->IsSelected(ActorMesh);
#if LX_ANCHOR
	bObjectIsSelected |= (ActorMesh && dynamic_cast<LXAnchor*>(ActorMesh))?true:false;
#endif
	bObjectIsSelected |= (ActorMesh && dynamic_cast<LXActorMeshGizmo*>(ActorMesh)) ? true : false;
		
	if (bObjectIsSelected)
	{
		if (LXQuery* pQuery = GetCurrentQuery())
		{
			if (ActorMesh && ActorMesh->IsPickable())
			{
				pQuery->OnLButtonDown(nFlags, pntWnd, ActorMesh);
				pQuery->SetPickedPoint(PointPicked);
				pQuery->SetConstraint(ActorMesh->GetConstraint(PrimitivePicked));
				return;
			}
		}
	}

	// Event
	GetEventManager()->BroadCastEvent(new LXEvenLButtonDown(PointPicked));


	m_pCamManip->OnButtonDown(LX_LBUTTON, pntWnd.x, pntWnd.y, ActorPicked ? true:false, PointPicked);
}

/*virtual*/
void LXViewport::OnLButtonUp(uint64 nFlags, LXPoint pntWnd)
{
	if (!_pDocument)
		return;

	if (nFlags & MK_RBUTTON)
		return;

	if (LXQueryManager* pQueryManager = GetQueryManager())
	{
		LXQuery* pQuery = GetCurrentQuery();
		if (pQuery && pQuery->IsActive())
		{
			pQueryManager->OnLButtonUp(nFlags, pntWnd);
			if (pQuery->MouseMoved()) 
				return;
		}
	}

	m_pCamManip->OnButtonUp(LX_LBUTTON, pntWnd.x, pntWnd.y);

	if (!m_pCamManip->MouseHasMoved())
	{
		if (ActorPicked && ActorPicked->IsPickable())
		{
			switch (GetCore().GetSelectionMode())
			{
			case ESelectionMode::SelectionModeActor: GetCore().GetCommandManager().AddToSelection2(ActorPicked, nFlags); break;
			case ESelectionMode::SelectionModePrimitive: GetCore().GetCommandManager().AddToSelection2(PrimitivePicked, nFlags); break;
			case ESelectionMode::SelectionModeMaterial: GetCore().GetCommandManager().AddToSelection2(PrimitivePicked ? PrimitivePicked->GetMaterial().get(): nullptr, nFlags); break;
			default:CHK(0);
			}
		}
		else
			GetCore().GetCommandManager().AddToSelection2(NULL, nFlags );

		// TEST SPLIT LINE
		if(0)
		{
			if (LXActorLine* pLine = dynamic_cast<LXActorLine*>(ActorPicked))
			{
				pLine->SplitWithPickedPoint();
				//Invalidate(LX_INVALIDATE_RENDERLIST);
			}
		}
	}
	else
	{

		LXScene* pScene	= NULL;
		//LXViewState* pViewState = NULL;
		if (_pDocument)
		{
			pScene =  _pDocument->GetScene();
			//	pViewState =  m_pDocument->GetViewState();
		}

		if (!pScene)
			return;

		vec2f p1 = m_pCamManip->GetPoint();
		vec2f p2 = m_pCamManip->GetCurrentPoint();

		if (p1.x == p2.x || p1.y == p2.y)
			return;

		vec2f pTemp = p2;

		pTemp.y = m_nHeight - p1.y;
		p1.y = m_nHeight - p2.y;
		p2 = pTemp;

		int foo = 0;

		/*
		LXBBox box;
		box.Add(vec3f(p1.x, p1.y, 0.0f));
		box.Add(vec3f(p2.x, p2.y, 0.0f));
		//m_pRenderer->GetPickBox(box, p1.x, p1.y, p2.x, p2.y);
		*/







		LXRectangularSelectionTraverser t;
		t.SetScene(pScene);
		//t.SetRenderer(m_pRenderer);


		vec3f v[8];


		v[0].Set(p1.x, p2.y, 0.0f);
		v[1].Set(p1.x, p1.y, 0.0f);
		v[2].Set(p2.x, p1.y, 0.0f);
		v[3].Set(p2.x, p2.y, 0.0f);

		v[4].Set(p1.x, p2.y, 1.0f);
		v[5].Set(p1.x, p1.y, 1.0f);
		v[6].Set(p2.x, p1.y, 1.0f);
		v[7].Set(p2.x, p2.y, 1.0f);

		for (int i=0; i<8; i++)
			WorldTransformation.UnProject(v[i]);
			
		LXFrustum frustum;
		frustum.Update(v);

		t.SetFrustum(&frustum);
		t.Apply();

		SetSmartObjects setActors;


		for (SetActors::const_iterator It = t.GetNodes().begin(); It != t.GetNodes().end(); It++)
		{
			LXActor* pActor = *It;
			if (pActor)
				setActors.insert(pActor);			
		}

		GetCore().GetCommandManager().AddToSelection2(setActors, nFlags);
	}
}


/*virtual*/
void LXViewport::OnMButtonDown(uint64 nFlags, LXPoint pntWnd)
{
	if (!_pDocument)
		return;

	vec3f intersection;
	LXActor* Actor = PickActor(pntWnd.x, pntWnd.y, &intersection);

	m_pCamManip->OnButtonDown( LX_MBUTTON, pntWnd.x, pntWnd.y, Actor?true:false, intersection);
}

/*virtual*/
void LXViewport::OnMButtonUp(uint64 nFlags, LXPoint pntWnd)
{
	if (!_pDocument)
		return;

	m_pCamManip->OnButtonUp( LX_MBUTTON, pntWnd.x, pntWnd.y);
}

/*virtual*/
void LXViewport::OnRButtonDown(uint64 nFlags, LXPoint pntWnd)
{
	if (!_pDocument)
		return;

	vec3f intersection;
	LXActor* Actor = PickActor(pntWnd.x, pntWnd.y, &intersection);

	m_pCamManip->OnButtonDown(LX_RBUTTON, pntWnd.x, pntWnd.y, Actor ? true : false, intersection);
}

/*virtual*/
void LXViewport::OnRButtonUp(uint64 nFlags, LXPoint pntWnd)
{
	//ReleaseCapture();
	m_pCamManip->OnButtonUp( LX_RBUTTON, pntWnd.x, pntWnd.y);

	if (_pDocument && !m_pCamManip->MouseHasMoved())
	{
		LXActor* pActor = PickActor(pntWnd.x, pntWnd.y);
		if (!_pDocument->GetSelectionManager().IsSelected(pActor))
			GetCore().GetCommandManager().AddToSelection2(pActor, nFlags);
	}
}

/*virtual*/
void LXViewport::OnMouseMove(uint64 nFlags, LXPoint pntWnd)
{
	if (!_pDocument)
		return;

	EMouseButton MouseButton = EMouseButton::None;
	switch (nFlags)
	{
	case MK_LBUTTON: MouseButton = EMouseButton::Left; break;
	case MK_MBUTTON: MouseButton = EMouseButton::Middle; break;
	case MK_RBUTTON: MouseButton = EMouseButton::Right; break;
	}
	
	
	LXQuery* pQuery = GetCurrentQuery();
	if (pQuery && pQuery->IsActive())
	{
		// HACK
		//m_pCamManip->SetExtendedSelection(false);
		pQuery->OnMouseMove(nFlags, pntWnd);
		return;
		/*
		vec3f vNormal;
		m_pRenderer->ReadNormal(pntWnd.x, pntWnd.y, vNormal);
		pQueryManager->MouseMoveOnNormal(vNormal);

		vec3f vPosition;
		m_pRenderer->ReadPosition(pntWnd.x, pntWnd.y, vPosition);
		pQueryManager->MouseMoveOnPostion(vPosition);
		*/
	}
	
	if (!m_pCamManip->OnMouseMove(nFlags, pntWnd.x, pntWnd.y)/* && m_pRenderer*/)
	{
//  		LXActorMesh* pMesh = m_pRenderer->PickMeshOnBufferColor(pntWnd.x, pntWnd.y);
//  		LXPrimitive* pPrimitive = dynamic_cast<LXPrimitive*>(m_pRenderer->PickDrawableOnBufferColor(pntWnd.x, pntWnd.y));
//  		if (pPrimitive != m_pPointedPrimitive)
//  		{
// 			m_pPointedPrimitive = pPrimitive;
//  			m_pPointedMesh = pMesh;
//  			m_pRenderer->Invalidate(LX_INVALIDATE_HIGHTING);
//  			GetCore().GetCommandManager().ChangeHighlight(pMesh);
//  		}
	}

	GetEventManager()->BroadCastEvent(new LXEventMouse(MouseButton));

}

/*virtual*/
void LXViewport::OnMouseWheel(uint16 nFlags, short zDelta, LXPoint pntWnd)
{
	m_pCamManip->OnMouseWheel(nFlags, zDelta, pntWnd);
}

void LXViewport::OnKeyUp(uint64 nChar, uint nRepCnt, uint nFlags)
{
	if (ConsoleEnabled)
	{
		switch (nChar)
		{
			case VK_UP:
			{
				if (ConsoleHistory.size() > 0)
				{
					ConsoleString = ConsoleHistory[ConsoleHistorySeek];
					GetConsoleManager().GetNearestCommand(ConsoleString, ConsoleNearestCommand);
					ConsoleSuggestionSeek = 0;
					ConsoleHistorySeek--;
					if (ConsoleHistorySeek < 0)
						ConsoleHistorySeek = (int)ConsoleHistory.size() - 1;
				}
			}
			break;

			case VK_DOWN:
			{
				if (ConsoleHistory.size() > 0)
				{
					ConsoleString = ConsoleHistory[ConsoleHistorySeek];
					GetConsoleManager().GetNearestCommand(ConsoleString, ConsoleNearestCommand);
					ConsoleSuggestionSeek = 0;
					ConsoleHistorySeek++;
					if (ConsoleHistorySeek >= ConsoleHistory.size())
						ConsoleHistorySeek = 0;
				}
			}
			break;
			

			case VK_RIGHT:
			case VK_TAB:
			{
				if (ConsoleNearestCommand.size() > 0)
				{
					ConsoleSuggestionSeek--;
					if (ConsoleSuggestionSeek < 0)
						ConsoleSuggestionSeek = (int)ConsoleNearestCommand.size() - 1;
					ConsoleString = ConsoleNearestCommand[ConsoleSuggestionSeek];
				}
			}
			break;
		}
	}
	else
	{
		switch (nChar)
		{
		case VK_SHIFT:
			m_pCamManip->OnKeyUp(LX_SHIFT);
			break;
		case VK_CONTROL:
			m_pCamManip->OnKeyUp(LX_CONTROL);
			break;
		}
	}
}

void LXViewport::OnChar(uint64 Character)
{
	if ((Character == 178) || (Character == 35))
	{
		ConsoleEnabled = !ConsoleEnabled;
	}
	else if (ConsoleEnabled)
	{
		switch (Character)
		{
			case VK_ESCAPE: 
			{
				ConsoleString = L"";
			}
			break;
			case VK_BACK: 
			{
				ConsoleString = ConsoleString.Left(ConsoleString.size() - 1);
				GetConsoleManager().GetNearestCommand(ConsoleString, ConsoleNearestCommand);
				ConsoleSuggestionSeek = 0;
			}
			break;
			case VK_RETURN: 
			{
				GetConsoleManager().TryToExecute(ConsoleString);  
				ConsoleHistory.push_back(ConsoleString);
				if (ConsoleHistory.size() > 10)
					ConsoleHistory.pop_front();
				// Reset the history seek to the latest element
				ConsoleHistorySeek = (int)ConsoleHistory.size() - 1;
				ConsoleString = L"";
				ConsoleNearestCommand.clear();
				ConsoleSuggestionSeek = -1;
			}
			break;
			case VK_SPACE: ConsoleString.append(L" "); break;
			default:
			{
				if (Character > 32 && Character <= 125)
				{
					LXString Char = LXString((char*)&Character);
					ConsoleString.append(Char);
					GetConsoleManager().GetNearestCommand(ConsoleString, ConsoleNearestCommand);
					ConsoleSuggestionSeek = 0;
				}
			}
		}

		return;
	}
	else if ((Character == 'f') || (Character == 'F'))
	{
		if (_pDocument && _pDocument->GetActiveView())
		{
			LXBBox BBoxWCS = _pDocument->GetSelectionManager().GetBBoxWorld();
			_pDocument->GetActiveView()->ZoomOnBBoxAnimated(BBoxWCS);
		}
	}

}

void LXViewport::DropFile(const LXFilepath& Filename)
{
	GetCore().LoadFile(Filename);
}

void LXViewport::OnKeyDown(uint64 nChar, uint nRepCnt, uint nFlags)
{
	if (!(nFlags & 0x4000))
	{
		switch(nChar) 
		{
		case VK_SHIFT:
			m_pCamManip->OnKeyDown(LX_SHIFT);
			break;
		case VK_CONTROL:
			m_pCamManip->OnKeyDown(LX_CONTROL);
			break;
		}
	}
}

/*virtual*/
//void LXViewport::SetRendererSize(uint nWidth, uint nHeight)
void LXViewport::SetViewportSize(uint nWidth, uint nHeight)
{
	m_nHeight = nHeight;
	m_nWidth  = nWidth;
// 	if (m_pRenderer)
// 	{
// 		m_pRenderer->SetViewportSize(nWidth, nHeight);
// 		m_pRenderer->OnSize(nWidth, nHeight);
// 	}
	//Invalidate(LX_INVALIDATE_RENDERLIST);
}

void LXViewport::OnPaint()
{
	//Invalidate(LX_INVALIDATE_SCREEN);
	Render();
}

void LXViewport::Render()
{
	//LXPerformance perf;

	LXScene* pScene	= NULL;
	LXViewState* pViewState = NULL;

	if (_pDocument)
	{
		pScene =  _pDocument->GetScene();
		pViewState =  _pDocument->GetActiveView();
	}

	if (m_pCamManip->ExtendedSelection())
	{
		const vec2f& p1 = m_pCamManip->GetPoint();
		const vec2f& p2 = m_pCamManip->GetCurrentPoint();
// 		if (m_pRenderer)
// 			m_pRenderer->SetExtendedSelection( p1.x, p1.y, p2.x, p2.y );  // TODO : Query
	}
// 	else if (m_pRenderer)
// 		m_pRenderer->SetExtendedSelection( false );  // TODO : Query
// 
// 	if (m_pRenderer)
// 	{
// 		m_pRenderer->SetRenderData(pScene, pViewState);
// 		m_pRenderer->MainRender();
// 		//m_pRenderer->SetRenderData(NULL, NULL);
// 	}

// 	double t = perf.GetTime();
// 	LX_COUNT("Render MS", t);
// 	LX_COUNT("Render FPS", 1000. / t);
}

void LXViewport::Animate( double dFrameTime )
{
	CHK(0);
}

bool LXViewport::GetPointOnBufferColor( int x, int y, vec3f& v )
{
// 	if (m_pRenderer)
// 		m_pRenderer->GetPointOnPickBuffer(x, y, v);
// 	else
// 		CHK(0);

	return true;
}

LXActorMesh* LXViewport::PickMesh(int x, int y)
{
	return dynamic_cast<LXActorMesh*>(PickActor(x,y));
}

// LXActor* LXViewport::PickActorOnBufferColor(int x, int y)
// {
// // 	if (m_pRenderer)
// // 		return dynamic_cast<LXActor*>(m_pRenderer->PickMeshOnBufferColor(x, y));
// // 	else
// 		return nullptr;
// }
// 
// LXAnchor* LXViewport::PickAnchorOnBufferColor(int x, int y)
// {
// // 	if (m_pRenderer)
// // 		return dynamic_cast<LXAnchor*>(m_pRenderer->PickMeshOnBufferColor(x, y));
// // 	else
// 		return nullptr;
// }
// 
// LXPrimitive* LXViewport::PickDrawableOnBufferColor(int x, int y)
// {
// 	return nullptr;
// 	//return m_pRenderer?m_pRenderer->PickDrawableOnBufferColor(x,y):nullptr;
// }

void LXViewport::OnDragObject( LXObject* pObject, LXPoint pntWnd)
{
	CHK(pObject);
	if(!pObject)
		return;

// 	if (LXMaterial* pMaterial = dynamic_cast<LXMaterial*>(pObject))
// 	{
// 
// 		LXPrimitive* pPrimitive = (LXPrimitive*)PickDrawableOnBufferColor(pntWnd.x, pntWnd.y);
// 		if (pPrimitive)
// 			pPrimitive->SetMaterial(pMaterial);
// 		else
// 			cout << "NoPrimitive" << endl;
// 
// 		//Invalidate(LX_INVALIDATE_RENDERLIST);
// 	}
}

bool LXViewport::DropAsset(std::shared_ptr<LXAsset>& Asset, LXPoint pntWnd)
{
	CHK(Asset);
	
	if (std::shared_ptr<LXMaterial> pMaterial = std::dynamic_pointer_cast<LXMaterial>(Asset))
		return DropMaterial(pMaterial, pntWnd);

	if (std::shared_ptr<LXAssetMesh> AssetMesh = std::dynamic_pointer_cast<LXAssetMesh>(Asset))
		return DropAssetMesh(AssetMesh, pntWnd);


	return false;
}

bool LXViewport::DropMaterial( std::shared_ptr<LXMaterial>& pMaterial, LXPoint pntWnd )
{
	CHK(pMaterial);
	if (!pMaterial)
		return false;

	LXPrimitive* Primitive = nullptr;
	LXActorMesh* ActorMesh = dynamic_cast<LXActorMesh*>(PickActor(pntWnd.x, pntWnd.y, nullptr, &Primitive));
	if (Primitive)
	{
		LXPropertyAssetPtr* pProp = dynamic_cast<LXPropertyAssetPtr*>(Primitive->GetProperty(LXPropertyID::PRIMITIVE_MATERIAL));
		CHK(pProp);
		if (pProp)
		{
			GetCore().GetCommandManager().ChangeProperty(pProp, (LXAssetPtr)pMaterial);
			return true;
		}
	}

	return false;
}

bool LXViewport::DropAssetMesh(std::shared_ptr<LXAssetMesh>& AssetMesh, LXPoint pntWnd)
{
	LXActorMesh* ActorMesh = new LXActorMesh();
	ActorMesh->SetMesh(AssetMesh->GetMesh());
	ActorMesh->SetAssetMesh(AssetMesh);
	GetProject()->GetScene()->AddChild(ActorMesh);
	LogD(Viewport, L"Use the CommandAddActor");
	return true;
}

//
// Picking
//

// bool LXViewport::PickPoint( LXActor* pActor, int x, int y, vec3f& vPoint )
// {
// 	if (!m_pDocument)
// 		return false;
// 
// 	LXScene* pScene = m_pDocument->GetScene();
// 	if (!pScene)
// 		return false;
// 
// 	LXAxis axis;
// 	float xScale = WorldTransformation.Width() / m_nWidth;
// 	float yScale = WorldTransformation.Height() / m_nHeight;
// 	WorldTransformation.GetPickAxis(axis, x * xScale, y * yScale);
// 
// 	// setup the RayIntersectTraverser
// 	LXPickTraverser t;
// 	t.SetScene(pScene);
// 	t.SetRay(axis);
// 	t.Apply();
// 
// 	// Determine the nearest object
// 	if (t.GetNumberOfIntersections() > 0)
// 	{
// 		vec3f* p = t.GetNearestPOIOnNode(pActor);
// 		if (p)
// 		{
// 			vPoint = *p;
// 			LogD(LXViewport, L"PickPoint %.2f %.2f %.2f", vPoint.x, vPoint.y, vPoint.z);
// 
// 			return true;
// 		}
// 	}
// 	return false;
// }

bool LXViewport::PickPoint(float x, float y, vec3f& vPoint)
{
	if (!_pDocument)
		return false;

	LXScene* pScene = _pDocument->GetScene();
	if (!pScene)
		return false;

	if (!WorldTransformation.IsValid())
		return false;

	LXAxis axis;
	float xScale = WorldTransformation.Width() / m_nWidth;
	float yScale = WorldTransformation.Height() / m_nHeight;
	WorldTransformation.GetPickAxis(axis, x * xScale, y * yScale);

	// setup the RayIntersectTraverser
	LXPickTraverser t;
	t.SetScene(pScene);
	t.SetRay(axis);
	t.Apply();

	// Determine the nearest object
	if (t.GetNumberOfIntersections() > 0)
	{
		vec3f* p = t.GetNearestPOI();
		if (p)
		{
			if (LXActor* Actor = pScene->GetActor(PointOfIntersectionObjectName))
			{
				Actor->SetPosition(*p);
			}
			
			vPoint = *p;
			return true;
		}
	}
	return false;
}

void LXViewport::ToggleShowBBoxes()
{
	_showBBoxes = !_showBBoxes;

	if (_pDocument)
	{
		SetActors setActors;
		_pDocument->GetActors(setActors);
		for (LXActor* actor : setActors)
		{
			actor->SetBBoxVisible(_showBBoxes);
		}
	}
}

void LXViewport::ToggleShowPrimitiveBBoxes()
{
	_showPrimitiveBBoxes = !_showPrimitiveBBoxes;

	if (_pDocument)
	{
		SetActors setActors;
		_pDocument->GetActors(setActors);
		for (LXActor* actor : setActors)
		{
			actor->SetPrimitiveBBoxVisible(_showPrimitiveBBoxes);
		}
	}
}

LXActor* LXViewport::PickActor( int x, int y, vec3f* intersection, LXPrimitive** primitive )
{
	if (!_pDocument)
		return nullptr;

	if (!WorldTransformation.IsValid())
		return nullptr;

	LXScene* pScene = _pDocument->GetScene();
	if (!pScene)
		return nullptr;

	LXAxis axis;
	float xScale = WorldTransformation.Width() / m_nWidth;
	float yScale = WorldTransformation.Height() / m_nHeight;
	WorldTransformation.GetPickAxis(axis, (float)x * xScale, (float)y * yScale);
		
	// setup the RayIntersectTraverser
	LXPickTraverser t;
	t.SetScene(pScene);
	t.SetRay(axis);
	t.WordTransformation = &WorldTransformation;
	t.Apply();

	// Determine the nearest actor
	if (t.GetNumberOfIntersections() > 0)
	{
		LXActor* pNode = t.GetNearestNode();
		if (intersection)
		{
			*intersection = *t.GetNearestPOI();
// 			LogD(LXViewport, L"PickAcctor %i %i to %.2f %.2f %.2f", x, y, intersection->x, intersection->y, intersection->z);
// 
// 			double winx, winy, winz;
// 			WorldTransformation.Project(intersection->x, intersection->y, intersection->z, winx, winy, winz);
// 			LogD(LXViewport, L"Project %.2f %.2f %.2f to %.2f %.2f %.2f", intersection->x, intersection->y, intersection->z, winx, winy, winz);
// 			
// 			double px, py, pz;
// 			WorldTransformation.UnProject(winx, winy, winz, px, py, pz);
// 			LogD(LXViewport, L"Unproject %.2f %.2f %.2f to %.2f %.2f %.2f", winx, winy, winz, px, py, pz);


			if (LXActor* Actor = pScene->GetActor(PointOfIntersectionObjectName))
			{
				Actor->SetPosition(*intersection);
			}
		}

		if (primitive)
			*primitive = t.GetNearestPrimitive();

		if (LXActor* pActor = dynamic_cast<LXActor*>(pNode))
			return pActor;
		else
			CHK(0);
	}
	return NULL;
}

void LXViewport::SetDocument( LXProject* pDocument )
{
	LXDocumentOwner::SetDocument(pDocument);
}

bool LXViewport::IsMouseMoved( ) 
{ 
	return m_pCamManip ? m_pCamManip->MouseHasMoved() : false;
}

