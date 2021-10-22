//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXActorSceneCapture.h"
#include "LXEngine.h"

LXActorSceneCapture::LXActorSceneCapture():LXActor(GetEngine().GetProject())
{
	SetName(L"SceneCapture");

	// --------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"ActorSceneCapture");
	// --------------------------------------------------------------------------------------------------------------
	LXPropertyAssetPtr* propertyAsset = DefineProperty(L"CubeMap", (std::shared_ptr<LXAsset>*)&_Texture);
	propertyAsset->SetUserData((int64)EAssetType::Texture);

	LXPropertyFloat* propertyFloat = DefinePropertyFloat("Intensity", LXPropertyID::LIGHT_INTENSITY, &_Intensity);
	propertyFloat->SetMinMax(0.f, 8.f);
}

LXActorSceneCapture::~LXActorSceneCapture()
{
}
