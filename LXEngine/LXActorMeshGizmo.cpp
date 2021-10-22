//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXActorCamera.h"
#include "LXActorMeshGizmo.h"
#include "LXCommandManager.h"
#include "LXEngine.h"
#include "LXEventManager.h"
#include "LXLogger.h"
#include "LXMesh.h"
#include "LXPrimitive.h"
#include "LXPrimitiveFactory.h"
#include "LXProject.h"
#include "LXSelectionManager.h"
#include "LXViewport.h"

LXActorMeshGizmo::LXActorMeshGizmo()
{
	// This object is volatile and marked as System.
	SetSystem(true);
	SetPersistent(false);
	SetName(L"Gizmo");
	SetVisible(false);
	_bCastShadows = false;
	_nCID |= LX_NODETYPE_CS;

	// Register Events.
	GetEventManager()->RegisterEvent(EEventType::SelectionChanged, this);

	Mesh = std::make_shared<LXMesh>(nullptr);

	// Translations Lines 
	const float LineWidth = 0.075f;
	const float LineLenght = 1.0f;

	// Translations Square
	const float SquareSize = 0.5f;
	const float SquareTickness = 0.025f;
	
	// Translate X
	{
		const std::shared_ptr<LXPrimitive>& Primitive = GetPrimitiveFactory()->CreateCube(LineLenght, LineWidth, LineWidth, false);
		Primitive->SetMaterial(L"Materials/M_UIRed.smat");
		Primitive->ComputeNormals();
		Primitive->ComputeTangents();
		_primitiveConstraints[Primitive.get()] = EConstraint::Local_X;
		AddPrimitive(Primitive);
	}

	// Translate Y
	{
		const std::shared_ptr<LXPrimitive>& Primitive = GetPrimitiveFactory()->CreateCube(LineWidth, LineLenght, LineWidth, false);
		Primitive->SetMaterial(L"Materials/M_UIGreen.smat");
		Primitive->ComputeNormals();
		Primitive->ComputeTangents();
		_primitiveConstraints[Primitive.get()] = EConstraint::Local_Y;
		AddPrimitive(Primitive);
	}

	// Translate Z
	{
		const std::shared_ptr<LXPrimitive>& Primitive = GetPrimitiveFactory()->CreateCube(LineWidth, LineWidth, LineLenght, false);
		Primitive->SetMaterial(L"Materials/M_UIBlue.smat");
		Primitive->ComputeNormals();
		Primitive->ComputeTangents();
		_primitiveConstraints[Primitive.get()] = EConstraint::Local_Z;
		AddPrimitive(Primitive);
	}

	// Translate XY
	{
		const std::shared_ptr<LXPrimitive>& Primitive = GetPrimitiveFactory()->CreateCube(SquareSize, SquareSize, SquareTickness, false);
		Primitive->SetMaterial(L"Materials/M_UIYellow.smat");
		Primitive->ComputeNormals();
		Primitive->ComputeTangents();
		_primitiveConstraints[Primitive.get()] = EConstraint::Local_XY;
		AddPrimitive(Primitive);
	}

	//XZ (Mauve)
	{
		const std::shared_ptr<LXPrimitive>& Primitive = GetPrimitiveFactory()->CreateCube(SquareSize, SquareTickness, SquareSize, false);
		Primitive->SetMaterial(L"Materials/M_UIYellow.smat");
		Primitive->ComputeNormals();
		Primitive->ComputeTangents();
		_primitiveConstraints[Primitive.get()] = EConstraint::Local_XZ;
		AddPrimitive(Primitive);
	}

	//YZ (Turquoise)
	{
		const std::shared_ptr<LXPrimitive>& Primitive = GetPrimitiveFactory()->CreateCube(SquareTickness, SquareSize, SquareSize, false);
		Primitive->SetMaterial(L"Materials/M_UIYellow.smat");
		Primitive->ComputeNormals();
		Primitive->ComputeTangents();
		_primitiveConstraints[Primitive.get()] = EConstraint::Local_YZ;
		AddPrimitive(Primitive);
	}

	// --- Rotation ---

	// Rotation X
	{
		LXMatrix Matrix;
		Matrix.SetRotation(LX_DEGTORAD(90.f), 0.f, 1.f, 0.f);
		const std::shared_ptr<LXPrimitive>& Primitive = GetPrimitiveFactory()->CreateTube(0.85f, 1.f, &Matrix);
		Primitive->SetMaterial(L"Materials/M_UIRed.smat");
		_primitiveConstraints[Primitive.get()] = EConstraint::Local_Rotate_X;
		AddPrimitive(Primitive);
	}

	// Rotation z
	{
		const std::shared_ptr<LXPrimitive>& Primitive = GetPrimitiveFactory()->CreateTube(0.85f, 1.f);
		Primitive->SetMaterial(L"Materials/M_UIBlue.smat");
		_primitiveConstraints[Primitive.get()] = EConstraint::Local_Rotate_Z;
		AddPrimitive(Primitive);
	}

	// Rotation Y
	{
		LXMatrix Matrix;
		Matrix.SetRotation(LX_DEGTORAD(90.f), 1.f, 0.f, 0.f);
		const std::shared_ptr<LXPrimitive>& Primitive = GetPrimitiveFactory()->CreateTube(0.85f, 1.f, &Matrix);
		Primitive->SetMaterial(L"Materials/M_UIGreen.smat");
		_primitiveConstraints[Primitive.get()] = EConstraint::Local_Rotate_Y;
		AddPrimitive(Primitive);
	}
}

LXActorMeshGizmo::~LXActorMeshGizmo()
{
	// Unregister Events.
	GetEventManager()->UnregisterEvent(EEventType::SelectionChanged, this);
}

EConstraint LXActorMeshGizmo::GetConstraint(LXPrimitive* primitive) const
{
	auto It = _primitiveConstraints.find(primitive);
	if (It != _primitiveConstraints.end())
		return It->second;
	else
		return EConstraint::None;
}

void LXActorMeshGizmo::Run(double frameTime)
{
	UpdateScale();
}

void LXActorMeshGizmo::UpdateScale()
{
	if (LXActorCamera* ActorCamera = GetProject()->GetCamera())
	{
		// Set the scale according the distance to the current Camera.
		float Distance = ActorCamera->GetPosition().Distance(GetPosition());
		float Scale = tanf(LX_DEGTORAD(ActorCamera->GetFov()) * 0.5f) * Distance * 0.20f;
		SetScale(Scale);
	}
}

void LXActorMeshGizmo::OnSelectionChanged() 
{ 
	LXSelectionManager *SelectionManager = GetSelectionManager();
	
	SetActors Actors;
	SelectionManager->GetUserActors(Actors);

	if (Actors.size() == 1)
	{
		const LXActor* Actor = *Actors.begin();
		if (Actor != GetProject()->GetCamera())
		{
			// Disable the targeted actor transform update to avoid infinite loop. 
			// Furthermore, we get the target transform to the gizmo, not it's not necessary to do the inverte op.
			_UpdateActorTransform = false;
			SetPosition(Actor->GetPosition());
			SetRotation(Actor->GetRotation());
			_UpdateActorTransform = true;
			UpdateScale();
			SetVisible(true);
			LogD(LXActorMeshGizmo, L"Rotation: %f %f %f", GetRotation().x, GetRotation().y, GetRotation().z);
		}
	}
	else if (Actors.size() > 1)
	{
		LXBBox BBox = SelectionManager->GetBBoxWorld();
		SetPosition(BBox.GetCenter());
		UpdateScale();
		SetVisible(true);
	}
	else
	{
		SetVisible(false);
	}
}

void LXActorMeshGizmo::OnRotationChanged()
{
	OnPositionChanged();
}

void LXActorMeshGizmo::OnPositionChanged()
{
	LXSelectionManager *SelectionManager = GetSelectionManager();
	if (!SelectionManager || !_UpdateActorTransform)
		return;

	SetActors Actors;
	SelectionManager->GetUserActors(Actors);

	if (Actors.size() == 1)
	{
		LXActor* actor = *Actors.begin();
		if (actor != GetProject()->GetCamera())
		{
			if (LXProperty* property = actor->GetProperty(LXPropertyID::POSITION))
			{
				GetEngine().GetCommandManager().PreviewChangeProperty(property, GetPosition());
			}

			if (LXProperty* property = actor->GetProperty(LXPropertyID::ROTATION))
			{
				GetEngine().GetCommandManager().PreviewChangeProperty(property, GetRotation());
			}
		}
	}
}

