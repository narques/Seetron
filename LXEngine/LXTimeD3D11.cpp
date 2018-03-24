//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXTimeD3D11.h"
#include "LXDirectX11.h"

LXTimeD3D11::LXTimeD3D11()
{
	auto Device = LXDirectX11::GetCurrentDevice();
	auto DeviceContext = LXDirectX11::GetCurrentDeviceContext();

	D3D11_QUERY_DESC desc;
	desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
	desc.MiscFlags = 0;

	Device->CreateQuery(&desc, &D3D11QueryTimestampDisjoint);

	desc.Query = D3D11_QUERY_TIMESTAMP;
	Device->CreateQuery(&desc, &D3D11QueryTimestampStart);
	Device->CreateQuery(&desc, &D3D11QueryTimestampStop);

	DeviceContext->Begin(D3D11QueryTimestampDisjoint);
	DeviceContext->End(D3D11QueryTimestampStart);
}

LXTimeD3D11::~LXTimeD3D11()
{
	auto DeviceContext = LXDirectX11::GetCurrentDeviceContext();
	D3D11QueryTimestampDisjoint->Release();
	D3D11QueryTimestampStart->Release();
	D3D11QueryTimestampStop->Release();
}

double LXTimeD3D11::Update()
{
	auto DeviceContext = LXDirectX11::GetCurrentDeviceContext();

	DeviceContext->End(D3D11QueryTimestampStop);
	DeviceContext->End(D3D11QueryTimestampDisjoint);

	UINT64 Start = 0;
	while (DeviceContext->GetData(D3D11QueryTimestampStart, &Start, sizeof(Start), 0) != S_OK);

	UINT64 Stop = 0;
	while (DeviceContext->GetData(D3D11QueryTimestampStop, &Stop, sizeof(Stop), 0) != S_OK);

	D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjointData;
	while (DeviceContext->GetData(D3D11QueryTimestampDisjoint, &disjointData, sizeof(disjointData), 0) != S_OK);

	double time = 0.;
	if (disjointData.Disjoint == FALSE)
	{
		UINT64 delta = Stop - Start;
		double frequency = static_cast<double>(disjointData.Frequency);
		time = (delta / frequency) * 1000.;
	}

	return time;
}

