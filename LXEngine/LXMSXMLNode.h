//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXPlatform.h"
#include <msxml6.h>
#include <atlcomcli.h>

class LXENGINE_API LXMSXMLNode
{

public:

	LXMSXMLNode() : elem(0), nlist(0), pos(-1), clen(0){ }
	LXMSXMLNode(int _clen) : elem(0), nlist(0), pos(-1), clen(_clen){ }
	LXMSXMLNode(CComPtr<IXMLDOMElement> _elem) : elem(_elem), nlist(0), pos(-1), clen(0){ get(); }
	LXMSXMLNode(CComPtr<IXMLDOMNodeList> _nlist) : elem(0), nlist(_nlist), pos(0), clen(0){ get(); }
	~LXMSXMLNode() { }

	void		get();
	std::wstring		name() const;
	std::wstring		attr(const std::wstring& name) const;
	bool		attrBool(const std::wstring& name, bool def) const;
	int			attrInt(const std::wstring& name, int def) const;
	UINT		attrUint(const std::wstring& name, UINT def) const;
	float		attrFloat(const std::wstring& name, float def) const;
	double		attrDouble(const std::wstring& name, double def) const;
	std::wstring		val() const;
	std::string		valA() const;
	float		valf() const;
	LXMSXMLNode	subnode(const std::wstring& name) const;
	std::wstring		subval(const std::wstring& name) const;
	LXMSXMLNode	begin() const;
	LXMSXMLNode	end() const;
	LXMSXMLNode	operator++(int);
	bool		operator!=(const LXMSXMLNode &e) const;

	CComPtr<IXMLDOMElement>		elem;
	CComPtr<IXMLDOMNodeList>	nlist;
	int							pos;
	long						clen;
};

