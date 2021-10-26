//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXMSXMLNode.h"

void LXMSXMLNode::get()
{ 
  if (pos!=-1)
  { 
	elem=0;
	CComPtr<IXMLDOMNode> inode;
	nlist->get_item(pos,&inode);
	if (inode==0) 
		return;

	DOMNodeType type; 
	inode->get_nodeType(&type);
	if (type!=NODE_ELEMENT) 
		return;
	CComQIPtr<IXMLDOMElement> e(inode);
	elem=e;
  }

  clen=0; 
  
  if (elem!=0)
  { 
	CComPtr<IXMLDOMNodeList> iNodeList;
	elem->get_childNodes(&iNodeList);
	iNodeList->get_length(&clen);  
  }
}

std::wstring LXMSXMLNode::name() const
{
  if (!elem) 
	  return _T("");
  
  CComBSTR bn; 
  elem->get_tagName(&bn);

  return std::wstring(bn);
}

std::wstring LXMSXMLNode::attr(const std::wstring& name) const
{ 
  if (!elem) 
	  return _T("");
  CComBSTR bname(name.c_str());
  CComVariant val(VT_EMPTY);
  elem->getAttribute(bname,&val);
  if (val.vt==VT_BSTR) 
	return std::wstring(val.bstrVal);
  else 
	return _T(""); 
}

bool LXMSXMLNode::attrBool(const std::wstring& name, bool def) const
{ 
  std::wstring a = attr(name);
  if (a==_T("true") || a==_T("TRUE")) 
	  return true; // TODO CompareNoCase
  else if (a==_T("false") || a==_T("FALSE")) 
	  return false;
  else 
  {
	  _ASSERT(0); 
	  return def; 
  }
}

int LXMSXMLNode::attrInt(const std::wstring& name, int def) const
{ 
  std::wstring a = attr(name);
  int i, res=swscanf_s(a.c_str(),_T("%i"), &i);
  if (res==1) 
	  return i; 
  else 
	  return def; 
}

unsigned int LXMSXMLNode::attrUint(const std::wstring& name, unsigned int def) const
{ 
	std::wstring a = attr(name);
	unsigned int i, res = swscanf_s(a.c_str(), _T("%u"), &i);
	if (res==1) 
		return i; 
	else 
	{ 
		_ASSERT(0);
		return def; 
	}
}

float LXMSXMLNode::attrFloat(const std::wstring& name, float def) const
{ 
  std::wstring a = attr(name);
  float f;
  int res = swscanf_s(a.c_str(), _T("%f"), &f);
  if (res==1) return f; 
  else { _ASSERT(0); return def; }
}

double LXMSXMLNode::attrDouble(const std::wstring& name, double def) const
{
	std::wstring a = attr(name);
	float f;
	int res = swscanf_s(a.c_str(), _T("%f"), &f);
	if (res == 1) return f;
	else { _ASSERT(0); return def; }
}

std::wstring LXMSXMLNode::val() const
{ 
  if (!elem) return _T("");
  CComVariant val(VT_EMPTY);
  elem->get_nodeTypedValue(&val);
  if (val.vt==VT_BSTR) return std::wstring(val.bstrVal);
  return _T("");
}

std::string LXMSXMLNode::valA() const
{ 
	if (!elem) return "";
	CComVariant val(VT_EMPTY);
	elem->get_nodeTypedValue(&val);
	//USES_CONVERSION;
	if (val.vt==VT_BSTR) 
	{
		//return wstringA(OLE2A(val.bstrVal));
		int size = lstrlenW(val.bstrVal);// + 1;
		
		char* toto = new char[size];
		int gkgk = (int)wcstombs( toto, val.bstrVal, size );
		
		/*
		char* toto2 = new char[size];
		size_t sizeConverted;
		wcstombs_s( &sizeConverted, toto2, size, val.bstrVal, size );
		*/
		
		return std::string(toto);
	}
	return "";
}

float LXMSXMLNode::valf() const
{
	if (!elem) return 0.0f;
	CComVariant val(VT_EMPTY);
	elem->get_nodeTypedValue(&val);
	if (val.vt==VT_BSTR) 
	{
		std::wstring a = val.bstrVal;
		float f;
		int res = swscanf_s(a.c_str(), _T("%f"), &f);
		if (res==1) 
			return f; 
		else { _ASSERT(0); return 0.0f; }
		
	}
	_ASSERT(0);
	return 0.0f;
}

LXMSXMLNode LXMSXMLNode::subnode(const std::wstring& name) const
{ 
  if (!elem) return LXMSXMLNode();
  for (LXMSXMLNode c=begin(); c!=end(); c++)
  { 
	if (c.name()==name) return c;
  }
  return LXMSXMLNode();
}

std::wstring LXMSXMLNode::subval(const std::wstring& name) const
{
  if (!elem) return _T("");
  LXMSXMLNode c=subnode(name);
  return c.val();
}

LXMSXMLNode LXMSXMLNode::begin() const
{ 
  if (!elem) return LXMSXMLNode();
  CComPtr<IXMLDOMNodeList> iNodeList;
  elem->get_childNodes(&iNodeList);
  return LXMSXMLNode(iNodeList);
}

LXMSXMLNode LXMSXMLNode::end() const
{ 
  return LXMSXMLNode(clen);
}

LXMSXMLNode LXMSXMLNode::operator++(int)
{ 
  if (pos!=-1) {pos++; get();}
  return *this;
}

bool LXMSXMLNode::operator!=(const LXMSXMLNode &e) const
{ 
  return pos!=e.clen;
}

