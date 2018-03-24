//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

struct ID3D11Query;

class LXTimeD3D11
{

public:

	LXTimeD3D11();
	~LXTimeD3D11();

	double Update();

private:

	ID3D11Query* D3D11QueryTimestampDisjoint = nullptr;
	ID3D11Query* D3D11QueryTimestampStart = nullptr;
	ID3D11Query* D3D11QueryTimestampStop = nullptr;

};

