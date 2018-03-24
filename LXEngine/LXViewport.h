//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXMouseEventHandler.h"
#include "LXWorldTransformation.h"
#include "LXDocumentOwner.h"

class LXTrackBallCameraManipulator;
class LXActorMesh;
class LXAnchor;
class LXMaterial;
class LXPrimitive;
class LXAssetMesh;
class LXAsset;

class LXCORE_API LXViewport : public LXMouseEventHandler, public LXDocumentOwner
{
	friend class LXViewportManager;

public:

	LXViewport();
	virtual ~LXViewport(void);

	//
	// Overridden form LXMouseEventHandler
	//

	virtual void			OnLButtonDblClk	(uint64 nFlags, LXPoint pntWnd ) override;

	virtual void			OnLButtonDown	(uint64 nFlags, LXPoint pntWnd ) override;
	virtual void			OnLButtonUp		(uint64 nFlags, LXPoint pntWnd ) override;

	virtual void			OnMButtonDown	(uint64 nFlags, LXPoint pntWnd ) override;
	virtual void			OnMButtonUp		(uint64 nFlags, LXPoint pntWnd ) override;
	
	virtual void			OnRButtonDown	(uint64 nFlags, LXPoint pntWnd ) override;
	virtual void			OnRButtonUp		(uint64 nFlags, LXPoint pntWnd ) override;
	
	virtual void			OnMouseWheel	(uint16 nFlags, short zDelta, LXPoint pntWnd ) override;
	virtual void			OnMouseMove		(uint64 nFlags, LXPoint pntWnd ) override;

	//
	// Overridden form LXDocumentOwnder
	//

	virtual void		   SetDocument		( LXProject* pDocument ) override;
		
	// Misc
	
	void					OnPaint			( );
	//void					SetRendererSize	( uint nWidth, uint nHeight );
	void					SetViewportSize	( uint nWidth, uint nHeight );
	uint					GetWidth		( )	const	{ return m_nWidth; }
	uint					GetHeight		( ) const	{ return m_nHeight; }
	float					GetAspectRatio() const { CHK(m_nHeight != 0); return (float)m_nWidth / (float)m_nHeight; }
	//void*					GetHWND			( ) const	{ return m_hWnd; } // HWND
	bool					IsMouseMoved	( );
		
	// Keyboard event
	
	void					OnKeyUp			( uint64 nChar, uint nRepCnt, uint nFlags );
	void					OnKeyDown		( uint64 nChar, uint nRepCnt, uint nFlags );
	void					OnChar			( uint64 Character );

	// Drag and Drop
	
	void					DropFile		( const class LXFilepath& Filename );
	void					OnDragObject	( LXObject* pObject, LXPoint pntWnd);
	bool					DropAsset		( LXAsset* Asset, LXPoint pntWnd );
	bool					DropMaterial	( LXMaterial* pMaterial, LXPoint pntWnd );
	bool					DropAssetMesh	( LXAssetMesh* AssetMesh, LXPoint pntWnd );

	// Misc

	LXPrimitive*			GetPointedPrimitive ( ) const { return m_pPointedPrimitive; }
	LXActorMesh*					GetPointedMesh ( ) const { return m_pPointedMesh; }
	LXTrackBallCameraManipulator*	GetCameraManipulator( ) const { return m_pCamManip; }
	bool					GetPointOnBufferColor( int x, int y, vec3f& v );
	void					InvalidateTerrainTextures( );

	//
	// Pick point on a specified object
	//
	
	// Pick a point
	bool					PickPoint(float x, float y, vec3f& vPoint);
			
private:

	void					Render			( );

	//
	// Common mouse events stuff
	//

// 	void					OnButtonDown	( LXPoint pntWnd );
// 	void					OnButtonUp		( LXPoint pntWnd );

	//
	// Picking (Geometric)
	//

	LXActor*				PickActor		( int x, int y, vec3f* intersection = NULL, LXPrimitive** primitive = NULL );
	LXActorMesh*			PickMesh		( int x, int y );

// 	LXActor*				PickActorOnBufferColor		( int x, int y );
// 	LXAnchor*				PickAnchorOnBufferColor		( int x, int y );
// 	LXPrimitive*				PickDrawableOnBufferColor	( int x, int y );

	//
	// Animation
	//

	void					Animate			( double dFrameTime );

	//
	// ViewportManager
	//

	void					OnChangeHighlight ( LXActor* pActor );
	void ClearRenderList();

private:

	//
	// Handles
	//

	//void*					m_hWnd;				// HWND

	//
	// Size
	//

	uint					m_nHeight;
	uint					m_nWidth;
	
	//
	// Cursor
	//
	LXPoint					m_pntWnd;              // Window cursor position at mouse button down
	float					m_fPickedZ;	           // Window Z value of cursor position at mouse button down
	vec3f					m_vPickedPoint;        // World cursor position at mouse button down
	LXWorldTransformation   m_wtPicked;			   // World transformation at mouse button down

	LXPrimitive*			m_pPointedPrimitive;   // Primitive under mouse
	LXActorMesh*					m_pPointedMesh;			

	//
	// Manipulator
	//

	LXTrackBallCameraManipulator*		m_pCamManip;

	//
	// Misc
	//

	public:

	LXWorldTransformation WorldTransformation;
	bool ConsoleEnabled = false;
	deque<LXString> ConsoleHistory;
	int ConsoleHistorySeek;
	LXString ConsoleString;
	vector<LXString> ConsoleNearestCommand;
	int ConsoleSuggestionSeek;

	private:

	LXActor* ActorPicked = nullptr;
	LXPrimitive* PrimitivePicked = nullptr;
	vec3f PointPicked;

};

typedef list<LXViewport*> ListViewports;