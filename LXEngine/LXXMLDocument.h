//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXPlatform.h"

struct IXMLDOMDocument;
class LXMSXMLNode;

class LXCORE_API LXXMLDocument
{

public:

	LXXMLDocument();
	~LXXMLDocument();
	bool Load(const wchar_t* fileName);
	LXMSXMLNode* GetRoot() const { return _pRoot; }

private:

	IXMLDOMDocument* _pXMLDom = nullptr;
	LXMSXMLNode* _pRoot = nullptr;

};

