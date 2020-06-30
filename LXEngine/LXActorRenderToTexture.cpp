//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXActorRenderToTexture.h"
#include "LXCore.h"
#include "LXMaterial.h"
#include "LXMesh.h"
#include "LXMeshFactory.h"
#include "LXRenderer.h"
#include "LXRenderPassDynamicTexture.h"
#include "LXRenderPipelineDeferred.h"
#include "LXTexture.h"
#include "LXMemory.h" // --- Must be the last included ---

LXActorRenderToTexture::LXActorRenderToTexture(LXProject* project):
	LXActorMesh(project)
{
	SetName(L"RenderToTexture");
	_nCID |= LX_NODETYPE_RENDERTOTTEXTURE;
	
	Mesh = GetMeshFactory()->CreateSSTriangle();

	LXProperty::SetCurrentGroup(L"RenderToTexture");

	LXPropertyAssetPtr* propertyMaterial = DefineProperty("Material", (shared_ptr<LXAsset>*)&_material);
	propertyMaterial->SetName(L"Material");
	propertyMaterial->ValueChanged.AttachMemberLambda([this](LXProperty*) { MaterialChanged(); });
	
	LXPropertyAssetPtr* pPropTexture = DefinePropertyAsset(L"Texture", GetAutomaticPropertyID(), (shared_ptr<LXAsset>*)&_texture);
	pPropTexture->SetName(L"Texture");
}

LXActorRenderToTexture::~LXActorRenderToTexture()
{
}

void LXActorRenderToTexture::SetTexture(shared_ptr<LXTexture>& texture)
{
	CHK(texture);
	_texture = texture;
}

void LXActorRenderToTexture::SetMaterial(shared_ptr<LXMaterialBase>& material)
{
	CHK(material);
	(static_cast<LXPropertyAssetPtr*>(GetProperty(L"Material")))->SetValue(material);
}

void LXActorRenderToTexture::Render(int frameCount)
{
	CHK(_renderingDrive == ERenderingDrive::OnDemand);
	CHK(CurrentFrame == 0);

	// RenderCluster is already setup,
	// simply awake the rendering throw a RT task to guarantee
	// the execution order.

	LXTask* task = new LXTaskCallBack([this]()
	{
		LXRenderPipelineDeferred* rpd = dynamic_cast<LXRenderPipelineDeferred*>(GetRenderer()->GetRenderPipeline());
		LXRenderPassDynamicTexture* renderPassDynamicTexture = const_cast<LXRenderPassDynamicTexture*>(rpd->GetRenderPassDynamicTexture());
		renderPassDynamicTexture->AwakeActor(_renderData);
	});
	
	FrameCount = frameCount;
	GetRenderer()->EnqueueTask(task);
}

void LXActorRenderToTexture::CopyDeviceToBitmap()
{
	if (_texture)
		_texture->CopyDeviceToBitmap();
}

void LXActorRenderToTexture::MaterialChanged()
{
	Mesh->SetMaterial(_material);
}
