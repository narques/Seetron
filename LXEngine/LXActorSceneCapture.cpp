//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXActorSceneCapture.h"
#include "LXCore.h"
#include "LXMemory.h" // --- Must be the last included ---

LXActorSceneCapture::LXActorSceneCapture():LXActor(GetCore().GetProject())
{
	// --------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"ActorSceneCapture");
	// --------------------------------------------------------------------------------------------------------------
	DefineProperty(L"CubeMap", (LXAssetPtr*)&_Texture);

	auto p = DefinePropertyFloat("Intensity", LXPropertyID::LIGHT_INTENSITY, &_Intensity);
	p->SetMinMax(0.f, 8.f);

}

LXActorSceneCapture::~LXActorSceneCapture()
{
}
