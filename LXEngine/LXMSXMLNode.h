//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXPlatform.h"
#include <msxml6.h>
#include <atlcomcli.h>

using namespace std;

class LXCORE_API LXMSXMLNode
{

public:

	LXMSXMLNode() : elem(0), nlist(0), pos(-1), clen(0){ }
	LXMSXMLNode(int _clen) : elem(0), nlist(0), pos(-1), clen(_clen){ }
	LXMSXMLNode(CComPtr<IXMLDOMElement> _elem) : elem(_elem), nlist(0), pos(-1), clen(0){ get(); }
	LXMSXMLNode(CComPtr<IXMLDOMNodeList> _nlist) : elem(0), nlist(_nlist), pos(0), clen(0){ get(); }
	~LXMSXMLNode() { }

	void		get();
	wstring		name() const;
	wstring		attr(const wstring& name) const;
	bool		attrBool(const wstring& name, bool def) const;
	int			attrInt(const wstring& name, int def) const;
	UINT		attrUint(const wstring& name, UINT def) const;
	float		attrFloat(const wstring& name, float def) const;
	double		attrDouble(const wstring& name, double def) const;
	wstring		val() const;
	string		valA() const;
	float		valf() const;
	LXMSXMLNode	subnode(const wstring& name) const;
	wstring		subval(const wstring& name) const;
	LXMSXMLNode	begin() const;
	LXMSXMLNode	end() const;
	LXMSXMLNode	operator++(int);
	bool		operator!=(const LXMSXMLNode &e) const;

	CComPtr<IXMLDOMElement>		elem;
	CComPtr<IXMLDOMNodeList>	nlist;
	int							pos;
	long						clen;
};

