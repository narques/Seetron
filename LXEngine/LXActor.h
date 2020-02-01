//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXSmartObject.h"
#include "LXBBox.h"
#include "LXRenderClusterType.h"
#include "LXTransformation.h"

class LXProject;
class LXAnchor;
class LXPrimitive;


typedef vector<LXAnchor*> ArrayAnchors;

#define LX_NODETYPE_GROUP	LX_BIT(0)
#define LX_NODETYPE_ACTOR	LX_BIT(1)
#define LX_NODETYPE_MESH	LX_BIT(2)
#define LX_NODETYPE_CAMERA	LX_BIT(3)
#define LX_NODETYPE_LIGHT	LX_BIT(4)
#define LX_NODETYPE_ANCHOR	LX_BIT(5)
#define LX_NODETYPE_CS		LX_BIT(6)
#define LX_NODETYPE_RENDERTOTTEXTURE LX_BIT(7)
#define LX_NODES_UI ( LX_NODETYPE_LIGHT | LX_NODETYPE_ANCHOR | LX_NODETYPE_CS )

enum class EConstraint
{
	None,
	Local_X,
	Local_Y,
	Local_Z,
	Local_XY,
	Local_XZ,
	Local_YZ,
	Local_Rotate_X,
	Local_Rotate_Y,
	Local_Rotate_Z
};

class LXCORE_API LXActor : public LXSmartObject
{

public:

	LXActor();
	LXActor(LXProject* pDocument);
	virtual ~LXActor(void);

	virtual void		MarkForDelete();
	virtual bool		IsPickable( ) const { return _bPickable; }

	// Rendering
	void				InvalidateRenderState(LXFlagsRenderClusterRole renderStates = ERenderClusterRole::All);
	void				ValidateRensterState();
	bool				IsRenderStateValid() { return _RenderStateValid; }
	
#if LX_ANCHOR

	// Anchors 
	void				AddAnchor( LXAnchor* pAnchor );
	void				InsertAnchor( LXAnchor* pAnchor, int position );
	LXAnchor*			GetAnchor( uint position ) const;
	uint				GetAnchorCount( ) const  { return (uint)_ArrayAnchors.size(); }
	virtual void		OnAnchorMove( LXAnchor* pAnchor ) { }
	virtual void		OnAnchorMove( LXAnchor* pAnchor, const LXMatrix& lcs ) { } /* Anchor is not transformed, the matrix parameter is not applied */
	virtual void		OnAnchorReleased(LXAnchor* pAnchors) { }

#endif

	// Transformation
	const LXMatrix&		GetMatrix() { return _Transformation.GetMatrix(); }
	const LXMatrix&		GetMatrixWCS();
	void				SetMatrixWCS(const LXMatrix& matrix, bool ComputeLocalMatrix = false);
	void				InvalidateMatrixWCS();
	void				ValidateMatrixWCS() { _bValidMatrixWCS = true; }
	
	const vec3f&		GetPosition() const { return _Transformation.GetTranslation(); }
	const vec3f&        GetRotation() const { return _Transformation.GetRotation(); }
	const vec3f&        GetScale() const { return _Transformation.GetScale(); }
	vec3f				GetVx() { return GetMatrix().GetVx(); }
	vec3f				GetVy() { return GetMatrix().GetVy(); }
	vec3f				GetVz() { return GetMatrix().GetVz(); }
	
	void				SetPosition(const vec3f& Translation);
	void				SetRotation(const vec3f& Rotation);
	void				SetScale(const vec3f& Scale);
	void				SetScale(float Scale) { SetScale(vec3f(Scale)); }

	// Bounds
	LXBBox&				GetBBoxLocal();
	LXBBox&				GetBBoxWorld();

	void				InvalidateBounds(bool bPropagateToParent = false);
	void				InvalidateWorldBounds(bool bPropagateToParent);
	virtual bool		ParticipateToSceneBBox() const { return true; };
	
	// World hierarchy
	void				SetParent(LXActor* pGroup) { _Parent = pGroup; }
	LXActor*			GetParent() { return _Parent; }
	LXActor*			GetPreviousParent() { return _LastKnownParent; }
	ListActors&			GetChildren() { return _Children; }
	void				AddChild(LXActor* Actor);
	void				RemoveChild(LXActor* Actor);

	// Serialization
	bool				OnSaveChild(const TSaveContext& saveContext) const override;
	bool				OnLoadChild(const TLoadContext& loadContext) override;
	
	// Constraints
	EConstraint			GetConstraint() const { return _eConstraint; }
	virtual EConstraint GetConstraint(LXPrimitive* primitive) const { return _eConstraint; }
	void				SetConstraint(EConstraint e) { _eConstraint = e; }

	// Misc
	bool				IsHighlightable() const { return _bHighlightable; }
	bool				IsDrawSelection() const { return _bDrawSelection; }
	int					GetCID() const { return _nCID; }
	LXProject*			GetProject() const { return _Project; }
	virtual bool		IsVisible() const { return _bVisible; }
	void				SetVisible(bool bVisible);
	
	bool				IsBBoxVisible() const { return _bBBoxVisible; };
	void				SetBBoxVisible(bool visible);

	bool				IsPrimitiveBBoxVisible() const { return _bPrimitiveBBoxVisible; };
	void				SetPrimitiveBBoxVisible(bool visible);
	
	// Play
	virtual void		Run(double frameTime);
	virtual void		RunRT(double frameTime);

	// Event Callbacks from EventManager
	virtual void		OnMouseMove() { CHK(0); }
	virtual void		OnMouseButtonDown() { CHK(0); }
	virtual void		OnMouseButtonUp() { CHK(0); }
	virtual void		OnSelectionChanged() { CHK(0); }

	// Actor Events
	virtual void        OnPositionChanged() {}
	virtual void		OnRotationChanged() {}
	virtual void		OnScaleChanged(){}

protected:

	virtual void		ComputeBBoxLocal();
	virtual void		ComputeBBoxWorld();
	
private:

	void				DefineProperties();
	virtual	void		OnInvalidateMatrixWCS() { };

public:

	// Rendering
	atomic<bool>		EuqueuedInRenderTask = false;
	
protected:

	// Hierarchy
	LXActor*			_Parent = nullptr;
	LXActor*			_LastKnownParent = nullptr;
	ListActors			_Children;

	// Transformation
	LXTransformation	_Transformation; // Local transformation
	LXMatrix			_MatrixWCS;	 
	bool				_bValidMatrixWCS = false;

	// Bounds
	LXBBox				_BBoxLocal; // Local BBox including children
	LXBBox				_BBoxWorld; // World BBox including children
	
	// Rendering
	bool				_bVisible = true;
	bool				_bBBoxVisible = false;
	bool				_bPrimitiveBBoxVisible = false;
	
	// Misc
	LXProject*			_Project = nullptr;
	bool				_bHighlightable = true;	// Avoid DrawHighlight OnMouseMove
	bool				_bPickable = true;		// Avoid Selection OnMouseClick or RectangularSelection
	bool				_bDrawSelection = true;	// Avoid DrawSelection
	int					_nCID;
#if LX_ANCHOR
	ArrayAnchors		_ArrayAnchors;
#endif
	
private:

	// Rendering
	bool				_RenderStateValid = false;

	// Misc
	EConstraint			_eConstraint = EConstraint::None;
}; 

template<typename T>
__forceinline T* LXCast(LXActor* actor)
{
	return actor ? ((actor->GetCID() & T::GetCIDBit()) ? (T*)actor : nullptr) : nullptr;
}