//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXXMLDocument.h"
#include "LXMSXMLNode.h"
#include "LXLogger.h"

#include <objbase.h>
#include <msxml6.h>
#include <atlcomcli.h>

LXXMLDocument::LXXMLDocument()
{
	CoInitialize(NULL);
}

LXXMLDocument::~LXXMLDocument()
{
	if (_pRoot)
		delete _pRoot;

	if (_pXMLDom)
		_pXMLDom->Release();

	CoUninitialize();
}

bool LXXMLDocument::Load(const wchar_t* fileName)
{
	HRESULT hr = CoCreateInstance(__uuidof(DOMDocument60), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&_pXMLDom));
	if (SUCCEEDED(hr))
	{
		VARIANT_BOOL bSuccess = false;
		hr = _pXMLDom->load(CComVariant(fileName), &bSuccess);

		CComPtr<IXMLDOMElement> iRootElm;
		_pXMLDom->get_documentElement(&iRootElm);
		_pRoot = new LXMSXMLNode(iRootElm);
		if (bSuccess && (hr == S_OK))
			return true;

		LogE(XMLDocument, L"Fail to load xml document. Error %i", hr);
		return false;
	}
	else
		LogE(XMLDocument, L"Fail to load xml document. CoCreateInstance error %i", hr);
	return false;
}
