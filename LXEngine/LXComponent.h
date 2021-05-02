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

class LXRenderData;
class LXTransformation;

class LXCORE_API LXComponent : public LXSmartObject
{
	
public:

	LXComponent(LXActor* actor);
	virtual ~LXComponent(void);

	virtual void MarkForDelete();
	virtual bool IsPickable() const { return _bPickable; }

	// Rendering
	void InvalidateRenderState(LXFlagsRenderClusterRole renderStates = ERenderClusterRole::All);
	void CreateRenderData(LXFlagsRenderClusterRole renderStates);
	void ReleaseRenderData(LXFlagsRenderClusterRole renderStates);
	virtual bool GetCastShadows() const { return false; }
	LXRenderData* GetRenderData() { return _renderData; }

	// Transformation
	const LXMatrix& GetMatrixWCS();

	// Bounds
	const LXBBox& GetBBoxLocal() const;
	LXBBox& GetBBoxWorld();

	void InvalidateBounds();
		
	// Misc
	bool IsHighlightable() const { return _bHighlightable; }
	bool IsDrawSelection() const { return _bDrawSelection; }
	int GetCID() const { return _nCID; }
	LXActor* GetActor() const { return _actor; }
	virtual bool IsVisible() const { return _bVisible; }
	void SetVisible(bool bVisible);

	bool IsBBoxVisible() const { return _bBBoxVisible; };
	void SetBBoxVisible(bool visible);

	bool IsPrimitiveBBoxVisible() const { return _bPrimitiveBBoxVisible; };
	void SetPrimitiveBBoxVisible(bool visible);

	// Scene construction 
	virtual void SceneLoaded() {};
	// Play
	virtual void Run(double frameTime);

	// Event Callbacks from EventManager
	virtual void OnMouseMove() { CHK(0); }
	virtual void OnMouseButtonDown() { CHK(0); }
	virtual void OnMouseButtonUp() { CHK(0); }
	virtual void OnSelectionChanged() { CHK(0); }

	// Component Events
	virtual void OnPositionChanged() {}
	virtual void OnRotationChanged() {}
	virtual void OnScaleChanged() {}

	// Delegates & Events
	LXDelegate<LXComponent*> PrimitiveWorldBoundsComputed;

protected:

	// Transformation
	void InvalidateMatrixWCS();

	// Bounds
	virtual void ComputeBBoxLocal();

private:

	// Construction
	void DefineProperties();

	// Transformation
	void ComputeMatrixWCS();

	// Bounds
	void ComputeBBoxWorld();

protected:

	LXActor* _actor = nullptr;
	int _nCID;

	// Bounds
	LXBBox _BBoxLocal;
	LXBBox _BBoxWorld; // World BBox
	

	// Rendering
	LXRenderData* _renderData = nullptr;
	LXFlagsRenderClusterRole _renderStates = ERenderClusterRole::Default;

private:

	// Rendering
	bool _bVisible = true;
	bool _bBBoxVisible = false;
	bool _bPrimitiveBBoxVisible = false;

	// Avoid DrawHighlight OnMouseMove
	bool _bHighlightable = true;
	
	// Avoid Selection OnMouseClick or RectangularSelection
	bool _bPickable = true;

	// Avoid DrawSelection
	bool _bDrawSelection = true;	
};

template<typename T>
__forceinline T* LXCast(LXComponent* actor)
{
	return actor ? ((actor->GetCID() & T::GetCIDBit()) ? (T*)actor : nullptr) : nullptr;
}

template <typename T>
class LXCORE_API LXComponentRawPtr : public LXComponent
{

public:
	
	LXComponentRawPtr(LXActor* actor) :LXComponent(actor) { _object = new T(); }
	virtual ~LXComponentRawPtr() { delete _object; };

	T* Get() const { return _object; }
	T* operator->() const { return _object; }
	
private:

	T* _object;
};

template <typename T>
class LXCORE_API LXComponentSharedPtr : public LXComponent
{

public:

	LXComponentSharedPtr(LXActor* actor) :LXComponent(actor) { _object = std::make_shared<T>(); }
	virtual ~LXComponentSharedPtr() {};

	const std::shared_ptr<T>& Get() const { return _object; }
	const std::shared_ptr<T>& operator->() const { return _object; }

private:

	std::shared_ptr<T> _object;
};
