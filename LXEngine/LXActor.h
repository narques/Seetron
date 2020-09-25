//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXActorType.h"
#include "LXSmartObject.h"
#include "LXBBox.h"
#include "LXRenderClusterType.h"
#include "LXTransformation.h"
#include "LXTree.h"

class LXAnchor;
class LXComponent;
class LXPrimitive;
class LXProject;
class LXRenderData;

typedef vector<LXAnchor*> ArrayAnchors;

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

class LXCORE_API LXActor : public LXSmartObject, public LXTreeNode<LXActor>
{
	friend LXRenderData;

public:

	LXActor();
	LXActor(LXProject* pDocument);
	virtual ~LXActor(void);

	virtual void		MarkForDelete();
	virtual bool		IsPickable( ) const { return _bPickable; }

	// Components
	void AddComponent(LXComponent* component);
	void RemoveComponent(LXComponent* component) { _components.remove(component); }
	const list<LXComponent*>& GetComponents() const { return _components; }

	template<typename T>
	T& CreateAndAddComponent()
	{
		T* component = new T(this);
		AddComponent(component);
		return *component;
	}

	// Return the first component of the given type
	template<typename T>
	T* GetComponent() const
	{
		for (LXComponent* component : _components)
		{
			if (T* typedComponent = dynamic_cast<T*>(component))
				return typedComponent;
		}

		return nullptr;
	}

	// Misc
	virtual ListProperties GetProperties() const override;

	// Rendering
	void				InvalidateRenderState(LXFlagsRenderClusterRole renderStates = ERenderClusterRole::All);
	void				CreateRenderData(LXFlagsRenderClusterRole renderStates);
	void				ReleaseRenderData(LXFlagsRenderClusterRole renderStates);
	virtual bool		GetCastShadows() const { return false; }

	
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
	LXTransformation& GetTransformation() { return _Transformation; }
	const LXMatrix&		GetMatrix() { return _Transformation.GetMatrix(); }
	const LXMatrix&		GetMatrixWCS() const;
	void				SetMatrixWCS(const LXMatrix& matrix, bool ComputeLocalMatrix = false);
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
	void				AddChild(LXActor* Actor) override;
	void				RemoveChild(LXActor* Actor) override;
	
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
	
	// Scene construction 
	virtual void		SceneLoaded() {};
	// Play
	virtual void		Run(double frameTime);

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

	// Transformation
	void				InvalidateMatrixWCS();
	virtual void		ComputePrimitiveWorldMatrices();
	virtual void		OnComputedPrimitivesWordBBoxes() {};

	// Bounds
	virtual void		ComputeBBoxLocal();
	
private:

	// Transformation
	void				InvalidateBranchMatrixWCS();
	void				ComputeMatrixWCS();
	
	// Bounds
	void				InvalidateBranchWorldBounds(bool bPropagateToParent);
	void				ComputeBBoxWorld();
	   
	void				DefineProperties();
	virtual	void		OnInvalidateMatrixWCS() { };

protected:

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
	
	LXRenderData*		_renderData = nullptr;
	
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

	list<LXComponent*>  _components;

	// Misc
	EConstraint			_eConstraint = EConstraint::None;
}; 

template<typename T>
__forceinline T* LXCast(LXActor* actor)
{
	return actor ? ((actor->GetCID() & T::GetCIDBit()) ? (T*)actor : nullptr) : nullptr;
}