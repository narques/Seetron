//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------*

#include "stdafx.h"
#include "LXActorTree.h"
#include "LXProject.h"
#include "LXPrimitive.h"
#include "LXPrimitiveFactory.h"
#include "LXMemory.h" // --- Must be the last included ---

LXActorTree::LXActorTree()
{
// 	LXProperty::SetCurrentGroup(L"Trunk"); // Or tree
// 
// 	DefineProperty("TrunkHeight", &TrunkHeight, 1.f, 10000.f);
// 	DefineProperty("TrunkRadius", &TrunkRadius, 1.f, 10000.f);
// 	DefineProperty("Slices", &Slices, 1, 64);
// 	DefineProperty("Stacks", &Stacks, 1, 64);

	// Trunk
	const std::shared_ptr<LXPrimitive>& Primitive = GetPrimitiveFactory()->CreateCylinder(TrunkRadius, TrunkHeight);
	Primitive->SetPersistent(false);
	Primitive->ComputeTangents();
	AddPrimitive(Primitive);
	TrunkPrimitive = Primitive.get();

	// Branch (Level1)
// 	{
// 		LXTreeElement* Branch = new LXTreeElement();
// 		Branch->SetOrigin(vec3f(0.f, 0.f, TrunkHeight));
// 		AddChild(Branch);
// 	}
}

void LXActorTree::OnPropertyChanged(LXProperty* Property)
{
	__super::OnPropertyChanged(Property);

// 	if ((Property->GetVarPtr() == &TrunkHeight) || 
// 		(Property->GetVarPtr() == &TrunkRadius) ||
// 		(Property->GetVarPtr() == &Slices) ||
// 		(Property->GetVarPtr() == &Stacks))
// 	{
// 		//Rebuild the primitive
// 		CHK(0);
// //		RemovePrimitive(TrunkPrimitive);
// 		//m_pDocument->InvokeOnDeletePrimitive(this, TrunkPrimitive);
// 		LXPrimitive* pPrimitive = GetPrimitiveFactory()->CreateCylinder(TrunkRadius, TrunkHeight);
// 		pPrimitive->SetPersistent(false);
// 		pPrimitive->ComputeTangents();
// 		TrunkPrimitive = pPrimitive;
// 		AddPrimitive(pPrimitive);
// 		//m_pDocument->InvokeOnAddPrimitive(this, TrunkPrimitive);
// 	}
}

LXActorTree::~LXActorTree()
{
}

LXTreeElement::LXTreeElement()
{
	LXProperty::SetCurrentGroup(L"Branch"); // Or tree

	const std::shared_ptr<LXPrimitive>& Primitive = GetPrimitiveFactory()->CreateCylinder(10.f, 200.f);
	Primitive->SetPersistent(false);
	Primitive->ComputeTangents();
	AddPrimitive(Primitive);
}

LXTreeElement::~LXTreeElement()
{

}
