//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXMatrix.h"

struct ID3D11Buffer;

struct LXConstantBufferDataSpotLight
{
	LXMatrix MatrixLightView;		// Row-major
	LXMatrix MatrixLightProjection; // Row-major
	vec4f ShadowMapCoords;
	vec4f LightPosition;
	vec4f LightDirection;
	vec4f Color;
	float LightIntensity;
	float Angle;
	bool CastShadow;
};

struct LXConstantBufferDataIBL
{
	float AmbientIntensity;
	vec3f Pad;
};

struct LXConstantBufferData0 // Updated per Frame
{
	LXMatrix View;
	LXMatrix Projection;
	LXMatrix ViewProjectionInv;
	LXMatrix ProjectionInv;
	LXMatrix ViewInv;
	vec4f CameraPosition; 
	vec2f RendererSize;
	float Time;
	float DeltaTime;
};

struct LXConstantBufferData1 // Volatile, Updated at cluster creation
{
	LXMatrix World;
	LXMatrix Normal; // In WorldView
	vec3f LocalPosition;
	float Pad;
};

struct LXConstantBufferData2
{
	float Time;
	vec3f pad;
};

class LXConstantBufferD3D11
{

public:

	LXConstantBufferD3D11();
	LXConstantBufferD3D11(const void* buffer, int bufferSize);
	~LXConstantBufferD3D11();

	bool CreateConstantBuffer(const void* buffer, int bufferSize);

	ID3D11Buffer* D3D11Buffer = nullptr;
};

