//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXActorMesh.h"

class LXMaterialBase;
class LXTexture;

enum class ERenderingDrive
{
	OnEveryFrame,	// Rendered at each frame
	OnDemand		// Rendered calling the Render function
};

class LXActorRenderToTexture : public LXActorMesh
{

public:

	LXActorRenderToTexture(LXProject* project);
	virtual ~LXActorRenderToTexture();

	void SetMaterial(std::shared_ptr<LXMaterialBase>& material);
	const std::shared_ptr<LXMaterialBase>& GetMaterial() const { return _material; }

	void SetTexture(std::shared_ptr<LXTexture>& texture);
	const std::shared_ptr<LXTexture>& GetTexture() const { return _texture; }
	
	void SetRenderingDrive(ERenderingDrive renderingDrive) { _renderingDrive = renderingDrive; }
	ERenderingDrive GetRenderingDrive() const { return _renderingDrive; }
	
	void Render(int frameCount);
	void CopyDeviceToBitmap();
	
	// Delegates & Events
	LXDelegate<> Rendered_RT;
	LXDelegate<> AllFrameRendered;

	// Renderer
	std::atomic<bool> ClearRenderTarget = false;
	mutable int CurrentFrame = 0;
	int FrameCount = 0;
	
private:
	
	void MaterialChanged();

private:

	ERenderingDrive _renderingDrive = ERenderingDrive::OnEveryFrame;

	std::shared_ptr<LXMaterialBase> _material;
	std::shared_ptr<LXTexture> _texture;
};

