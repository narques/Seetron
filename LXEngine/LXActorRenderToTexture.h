//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXActorMesh.h"

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

	void SetMaterial(shared_ptr<LXMaterial>& material);
	const shared_ptr<LXMaterial>& GetMaterial() const { return _material; }

	void SetTexture(shared_ptr<LXTexture>& texture);
	const shared_ptr<LXTexture>& GetTexture() const { return _texture; }
	
	void SetRenderingDrive(ERenderingDrive renderingDrive) { _renderingDrive = renderingDrive; }
	ERenderingDrive GetRenderingDrive() const { return _renderingDrive; }
	
	void Render(int frameCount);
	void CopyDeviceToBitmap();
	
	// Delegates & Events
	LXDelegate<> Rendered_RT;

	// Renderer
	atomic<bool> ClearRenderTarget = false;
	mutable int CurrentFrame = 0;
	int FrameCount = 0;
	
private:
	
	void MaterialChanged();

private:

	ERenderingDrive _renderingDrive = ERenderingDrive::OnEveryFrame;

	shared_ptr<LXMaterial> _material;
	shared_ptr<LXTexture> _texture;
};

