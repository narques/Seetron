//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXActorLine.h"
#include "LXProject.h"
#include "LXAnchor.h"
#include "LXVec3.h"
#include "LXAssetManager.h"
#include "LXPrimitive.h"

LXActorLine::LXActorLine(LXProject* pDocument):LXActorMesh(pDocument),_nPickedID(-1)
{
	SetName(L"Line");
	/*
	_primLine = LXPrimitive::CreateLineLoop();
	AddPrimitive(_primLine);
*/
	SetConstraint(EConstraint::Local_XY);

// 	LXMaterial* pMaterial = pDocument->GetResourceManager().GetMaterial(L"sysLine");
// //	pMaterial->SetLighting(false);
// 	pMaterial->SetColor(RED);
// 	pMaterial->SetOpacity(0.8f);
// 	//LXShaderProgramGL* pShaderLine = pDocument->GetShaderManager().GetShader(L"RenderLine.glsl");
// 	//pMaterial->SetShader(pShaderLine);
// 	pMaterial->SetSystem(true);
// 	//pMaterial->SetAlwaysOnTop(true);
// 	_primLine->SetMaterial(pMaterial);
}

/*virtual*/
LXActorLine::~LXActorLine(void)
{
}

void LXActorLine::AddPoint(const vec3f& p)
{
	CHK(0);
// 	// Primitive
// 	_primLine->AddPosition(p);
// 
// 	// Anchor
// 	LXAnchor* pAnchor = new LXAnchor(m_pDocument);
// 	pAnchor->SetConstraint(LX_CONSTRAINT_LOCAL_XY);
// 	LXMatrix matAnchor;
// 	matAnchor.SetOrigin(p);
// 	pAnchor->SetMatrix(matAnchor);
// 	AddAnchor(pAnchor);
// 	AddChild(pAnchor);
}

void LXActorLine::InsertPoint(const vec3f& p, int position)
{
	CHK(0);
// 	// Update Primitive
// 	_primLine->Invalidate();  // Invalidate OpenGL objects
// 	ArrayVec3f& positions = _primLine->GetArrayPositions();
// 	ArrayVec3f::iterator It = positions.begin();
// 	_primLine->GetArrayPositions().insert(It + position, p);
// 
// 	// Anchor
// 	LXAnchor* pAnchor = new LXAnchor(m_pDocument);
// 	pAnchor->SetConstraint(LX_CONSTRAINT_LOCAL_XY);
// 	LXMatrix matAnchor;
// 	matAnchor.SetOrigin(p);
// 	pAnchor->SetMatrix(matAnchor);
// 	InsertAnchor(pAnchor, position);
// 	AddChild(pAnchor);
}

#if LX_ANCHOR
void LXActorLine::OnAnchorMove(LXAnchor* pAnchor)
{
	CHK(_ArrayAnchors.size() == _primLine->GetArrayPositions().size());
	for(int i=0; i < _ArrayAnchors.size(); i++)
		_primLine->GetArrayPositions()[i].Set(_ArrayAnchors[i]->GetPosition());
	//_primLine->BanchUpdatePositions();
}
#endif

void LXActorLine::SetPickedID( int i, const vec3f& vPosition )
{
	_nPickedID = i;
	_vPicked = vPosition;
}

void LXActorLine::SplitWithPickedPoint()
{
	if (_nPickedID == -1)
		return;

	InsertPoint(_vPicked, _nPickedID + 1);
}
