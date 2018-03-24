//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXProperty.h"
#include "LXSettings.h"
#include "LXAssetManager.h"
#include "LXCore.h"
#include "LXProject.h"
#include "LXLogger.h"
#include "LXMaterialNode.h"
#include "LXMaterial.h"
#include "LXMSXMLNode.h"
#include "LXMatrix.h"
#include "LXAsset.h"
#include "LXMemory.h" // --- Must be the last included ---

//--------------------------------------------------------------------------
// Local Functions
//--------------------------------------------------------------------------

void GetValueFromXML(const LXMSXMLNode& node, vec2f& value )
{
	float x = node.attrFloat(L"X", 1.f);
	float y = node.attrFloat(L"Y", 1.f);
	value.Set(x,y);
}

void GetValueFromXML(const LXMSXMLNode& node, vec3f& value)
{
	float x = node.attrFloat(L"X", 1.f);
	float y = node.attrFloat(L"Y", 1.f);
	float z = node.attrFloat(L"Z", 1.f);
	value.Set(x,y,z);
}

void GetValueFromXML(const LXMSXMLNode& node, LXString& value)
{
	value =  node.attr(L"Value");
}

void SaveXML(const TSaveContext& saveContext, const LXString& strXMLName, const LXString& value )
{ 
	CHK(!strXMLName.IsEmpty());
	fwprintf(saveContext.pXMLFile,L"<%s Value=\"%s\"/>\n", strXMLName.GetBuffer(), value.GetBuffer());
}

//--------------------------------------------------------------------------
// LXProperty
//--------------------------------------------------------------------------

LXString LXPropertyInfo::_CurrentGroup = L"MISC";

LXProperty::LXProperty(void):
_Owner(NULL)
{
	_PropInfo = new LXPropertyInfo();
}

LXProperty::~LXProperty(void)
{
	delete _PropInfo;
}

void LXProperty::SetObject( LXSmartObject* pObject )
{
	CHK(!_Owner);
	_Owner = pObject;
}

LXSmartObject* LXProperty::GetLXObject( ) const
{
	CHK(_Owner);
	return _Owner;
}

//--------------------------------------------------------------------------
// LXPropertyT 
//--------------------------------------------------------------------------

template <class T>
LXPropertyT<T>::LXPropertyT():LXProperty(), _Var(nullptr)
{
};

template <class T>
LXPropertyT<T>::LXPropertyT(const LXPropertyT& prop)
{
	_Var = prop._Var;
	*_PropInfo = *prop._PropInfo;
	_Owner = prop._Owner;

	_funcOnChange = prop._funcOnChange;
	_funcOnGet = prop._funcOnGet;

	if (prop._PropInfo->_MinValue)
	{
		_PropInfo->_MinValue = new T(*(T*)prop._PropInfo->_MinValue);
	}
	else
		_PropInfo->_MinValue = nullptr;
	
	if (prop._PropInfo->_MaxValue)
	{
		_PropInfo->_MaxValue = new T(*(T*)prop._PropInfo->_MaxValue);
	}
	else
		_PropInfo->_MaxValue = nullptr;
}

/*virtual*/
template <class T>
LXPropertyT<T>::~LXPropertyT( )
{
	LX_SAFE_DELETE(_PropInfo->_MinValue);
	LX_SAFE_DELETE(_PropInfo->_MaxValue);
}

template <class T>
void LXPropertyT<T>::SetMinMax( const T& valueMin, const T& valueMax )
{ 
	LX_SAFE_DELETE(_PropInfo->_MinValue);
	LX_SAFE_DELETE(_PropInfo->_MaxValue);
	_PropInfo->_MinValue = new T(valueMin); 
	_PropInfo->_MaxValue = new T(valueMax); 
}

template <class T>
void LXPropertyT<T>::SetMin(const T& valueMin)
{
	LX_SAFE_DELETE(_PropInfo->_MinValue);
	_PropInfo->_MinValue = new T(valueMin);
}

/*virtual*/
template <class T>
void LXPropertyT<T>::LoadXML(const TLoadContext& LoadContext)
{ 
	T val;
	GetValueFromXML2(LoadContext, val);
	SetValue(val, false);
}

/*virtual*/
template <class T>
void LXPropertyT<T>::SaveXML(const TSaveContext& saveContext)
{
	if (!_PropInfo->_bPersistent)
	{
		LogE(Property, L"Try to save a non persistent property (%s)", _PropInfo->_Name.GetBuffer());
		return;
	}
	
	if (_PropInfo->_Name.IsEmpty())
	{
		LogE(Property, L"Try to save a unnamed property");
		return;
	}

	LXString TagName;
	
	if (_PropInfo->_bUserProperty)
	{
		LXString TypeName = GetTypeName();
		if (TypeName.IsEmpty())
		{
			LogE(Property, L"Fail to save a non typed user property");
			return;
		}
		TagName = L"UserProperty Name=\"" + _PropInfo->_Name + L"\"" + L" Type=\"" + TypeName + L"\"";
	}
	else
		TagName = _PropInfo->_Name;
		
	SaveXML2(saveContext, TagName, GetValue());
}

template <class T>
const T& LXPropertyT<T>::GetValue( ) const
{ 
	CHK(_Owner);

	if (_Var)
		return *_Var;
	else if (_funcOnGet)
	{
		// HACK
		static T v;
		v = _funcOnGet();
		return v;
	}
	else
	{
		// No way to return a value. 
		CHK(0);  
		static T foo;
		return foo;
	}
}

template <class T>
bool LXPropertyT<T>::CheckRange(const T& value)
{
	return true;
}

template<>
bool LXPropertyT<float>::CheckRange(const float& value)
{
	if (GetMin())
		CHK(value >= (*GetMin()));
	if (GetMax())
		CHK(value <= (*GetMax()));
	return true;
 }

template<>
bool LXPropertyT<double>::CheckRange(const double& value)
{
	if (GetMin())
		CHK(value >= (*GetMin()));
	if (GetMax())
		CHK(value <= (*GetMax()));
	return true;
}

template<>
bool LXPropertyT<int>::CheckRange(const int& value)
{
	if (GetMin())
		CHK(value >= (*GetMin()));
	if (GetMax())
		CHK(value <= (*GetMax()));
	return true;
}

template<>
bool LXPropertyT<uint>::CheckRange(const uint& value)
{
	if (GetMin())
		CHK(value >= (*GetMin()));
	if (GetMax())
		CHK(value <= (*GetMax()));
	return true;
}

template <class T>
void LXPropertyT<T>::SetValue( const T& value, bool InvokeOnProperyChanged )
{ 
	if (value == GetValue())
	{
		return;
	}

	CHK(CheckRange(value));

	if (_Var)
		*_Var = value;
	else if (_funcOnSet)
		_funcOnSet(value);
	else
	{
		// No way to set the Value to the property
		CHK(0);
	}

	// Event (By Property)
	if(_funcOnChange)
		_funcOnChange(this);

	// Event (By Object & By App)
	if (InvokeOnProperyChanged)
		_Owner->OnPropertyChanged(this);
}

// Explicit class Instantiation
template class LXCORE_API LXPropertyT<bool>;
template class LXCORE_API LXPropertyT<int>;
template class LXCORE_API LXPropertyT<uint>;
template class LXCORE_API LXPropertyT<float>;
template class LXCORE_API LXPropertyT<double>;
template class LXCORE_API LXPropertyT<vec2f>;
template class LXCORE_API LXPropertyT<vec3f>;
template class LXCORE_API LXPropertyT<vec4f>;
template class LXCORE_API LXPropertyT<LXColor4f>;
template class LXCORE_API LXPropertyT<LXString>;
template class LXCORE_API LXPropertyT<LXFilepath>;
template class LXCORE_API LXPropertyT<LXMatrix>;
template class LXCORE_API LXPropertyT<LXAssetPtr>;
template class LXCORE_API LXPropertyT<LXMaterialNodePtr>;
template class LXCORE_API LXPropertyT<ArraySmartObjects>;
template class LXCORE_API LXPropertyT<ArrayVec3f>;

//
// --- float ---
//

template<>
void LXPropertyT<float>::GetValueFromXML2(const TLoadContext& LoadContext, float& value )
{
	const LXMSXMLNode& node = LoadContext.node;
	value =  node.attrFloat(L"Value",1.f);
}

template<>
void LXPropertyT<float>::SaveXML2(const TSaveContext& saveContext, const LXString& strXMLName, const float& value)
{
	CHK(!strXMLName.IsEmpty());
	fwprintf(saveContext.pXMLFile, L"<%s Value=\"%f\"/>\n", strXMLName.GetBuffer(), value);
}

//
// --- double ---
//

template<>
void LXPropertyT<double>::GetValueFromXML2(const TLoadContext& LoadContext, double& value)
{
	const LXMSXMLNode& node = LoadContext.node;
	value = node.attrDouble(L"Value", 1.);
}

template<>
void LXPropertyT<double>::SaveXML2(const TSaveContext& saveContext, const LXString& strXMLName, const double& value)
{
	CHK(!strXMLName.IsEmpty());
	fwprintf(saveContext.pXMLFile, L"<%s Value=\"%f\"/>\n", strXMLName.GetBuffer(), value);
}

//
// --- LXString ---
//

template<>
void LXPropertyT<LXString>::GetValueFromXML2(const TLoadContext& LoadContext, LXString& value )
{
	const LXMSXMLNode& node = LoadContext.node;
	value = node.attr(L"Value");
}

template<>
void LXPropertyT<LXString>::SaveXML2(const TSaveContext& saveContext, const LXString& strXMLName, const LXString& value )
{ 
	fwprintf(saveContext.pXMLFile,L"<%s Value=\"%s\"/>\n", strXMLName.GetBuffer(), value.GetBuffer());
}

//
// --- LXFilepath ---
//

template<>
void LXPropertyT<LXFilepath>::GetValueFromXML2(const TLoadContext& LoadContext, LXFilepath& value )
{
	const LXMSXMLNode& node = LoadContext.node;
	value = LXFilepath(node.attr(L"Value"));
}

template<>
void LXPropertyT<LXFilepath>::SaveXML2(const TSaveContext& saveContext, const LXString& strXMLName, const LXFilepath& value )
{ 
	fwprintf(saveContext.pXMLFile,L"<%s Value=\"%s\"/>\n", strXMLName.GetBuffer(), value.GetBuffer());
}

//
// --- LXColor4f ---
//

template<>
void LXPropertyT<LXColor4f>::GetValueFromXML2(const TLoadContext& LoadContext, LXColor4f& value )
{
	const LXMSXMLNode& node = LoadContext.node;
	float r = node.attrFloat(L"R", 1.f);
	float g = node.attrFloat(L"G", 1.f);
	float b = node.attrFloat(L"B", 1.f);
	float a = node.attrFloat(L"A", 1.f);
	value.Set(r,g,b,a);
}

template<>
void LXPropertyT<LXColor4f>::SaveXML2(const TSaveContext& saveContext,  const LXString& strXMLName, const LXColor4f& color)
{
	fwprintf(saveContext.pXMLFile, L"<%s R=\"%f\" G=\"%f\" B=\"%f\" A=\"%f\"/>\n", strXMLName.GetBuffer(), color.r, color.g, color.b, color.a);
}

//
// --- Int ---
//

template<>
void LXPropertyT<int>::GetValueFromXML2(const TLoadContext& LoadContext, int& value )
{
	const LXMSXMLNode& node = LoadContext.node;
	value = node.attrInt(L"Value", 1);
}

template<>
void LXPropertyT<int>::SaveXML2(const TSaveContext& saveContext,  const LXString& strXMLName, const int& value )
{
	fwprintf(saveContext.pXMLFile,L"<%s Value=\"%i\"/>\n", strXMLName.GetBuffer(), value);
}

//
// --- Uint ---
//

template<>
void LXPropertyT<uint>::GetValueFromXML2(const TLoadContext& LoadContext, uint& value )
{
	const LXMSXMLNode& node = LoadContext.node;
	value =  node.attrUint(L"Value", 1);
}

template<>
void LXPropertyT<uint>::SaveXML2(const TSaveContext& saveContext,  const LXString& strXMLName, const uint& value )
{
	fwprintf(saveContext.pXMLFile,L"<%s Value=\"%u\"/>\n", strXMLName.GetBuffer(), value);
}

//
// --- Bool ---
//

template<>
void LXPropertyT<bool>::GetValueFromXML2(const TLoadContext& LoadContext, bool& value )
{
	const LXMSXMLNode& node = LoadContext.node;
	value = node.attrInt(L"Value", 0)!=0;
}

template<>
void LXPropertyT<bool>::SaveXML2(const TSaveContext& saveContext,  const LXString& strXMLName, const bool& value )
{
	fwprintf(saveContext.pXMLFile,L"<%s Value=\"%s\"/>\n", strXMLName.GetBuffer(), value?L"1":L"0");
}

//
// --- LXMatrix --- 
//

template<>
void LXPropertyT<LXMatrix>::GetValueFromXML2(const TLoadContext& LoadContext, LXMatrix& value )
{
	const LXMSXMLNode& node = LoadContext.node;

	vec3f vo, vx, vy, vz;

	for (LXMSXMLNode e = node.begin(); e != node.end(); e++)
	{
		if (e.name() == L"Origin")
			::GetValueFromXML(e, vo);
		else if (e.name() == L"VX")
			::GetValueFromXML(e, vx);
		else if (e.name() == L"VY")
			::GetValueFromXML(e, vy);
		else if (e.name() == L"VZ")
			::GetValueFromXML(e, vz);
		else
			CHK(0);	
	}
	
	value.SetOrigin(vo);
	value.SetXYZ(vx, vy, vz);
}

template<>
void LXPropertyT<LXMatrix>::SaveXML2(const TSaveContext& saveContext,  const LXString& strXMLName, const LXMatrix& value )
{
	vec3f vo = value.GetOrigin();
	vec3f vx = value.GetVx();
	vec3f vy = value.GetVy();
	vec3f vz = value.GetVz();

	fwprintf(saveContext.pXMLFile, L"<%s>\n", strXMLName.GetBuffer());
	fwprintf(saveContext.pXMLFile, L"<Origin X=\"%f\" Y=\"%f\" Z=\"%f\"/>\n", vo.x, vo.y, vo.z);
	fwprintf(saveContext.pXMLFile, L"<VX X=\"%f\" Y=\"%f\" Z=\"%f\"/>\n", vx.x, vx.y, vx.z);
	fwprintf(saveContext.pXMLFile, L"<VY X=\"%f\" Y=\"%f\" Z=\"%f\"/>\n", vy.x, vy.y, vy.z);
	fwprintf(saveContext.pXMLFile, L"<VZ X=\"%f\" Y=\"%f\" Z=\"%f\"/>\n", vz.x, vz.y, vz.z);
	fwprintf(saveContext.pXMLFile, L"</%s>\n", strXMLName.GetBuffer());
}

//
// --- vec2f ---
//

template<>
void LXPropertyT<vec2f>::GetValueFromXML2(const TLoadContext& LoadContext, vec2f& value )
{
	const LXMSXMLNode& node = LoadContext.node;
	float x = node.attrFloat(L"X", 1.f);
	float y = node.attrFloat(L"Y", 1.f);
	value.Set(x,y);
}

template<>
void LXPropertyT<vec2f>::SaveXML2(const TSaveContext& saveContext,  const LXString& strXMLName, const vec2f& v)
{
	fwprintf(saveContext.pXMLFile, L"<%s X=\"%f\" Y=\"%f\"/>\n", strXMLName.GetBuffer(), v.x, v.y);
}

//
// --- vec3f ---
//

template<>
void LXPropertyT<vec3f>::GetValueFromXML2(const TLoadContext& LoadContext, vec3f& value )
{
	const LXMSXMLNode& node = LoadContext.node;
	float x = node.attrFloat(L"X", 1.f);
	float y = node.attrFloat(L"Y", 1.f);
	float z = node.attrFloat(L"Z", 1.f);
	value.Set(x,y,z);
}

template<>
void LXPropertyT<vec3f>::SaveXML2(const TSaveContext& saveContext,  const LXString& strXMLName, const vec3f& v)
{
	fwprintf(saveContext.pXMLFile, L"<%s X=\"%f\" Y=\"%f\" Z=\"%f\"/>\n", strXMLName.GetBuffer(), v.x, v.y, v.z);
}

//
// --- vec4f ---
//

template<>
void LXPropertyT<vec4f>::GetValueFromXML2(const TLoadContext& LoadContext, vec4f& value )
{
	const LXMSXMLNode& node = LoadContext.node;
	float x = node.attrFloat(L"X", 1.f);
	float y = node.attrFloat(L"Y", 1.f);
	float z = node.attrFloat(L"Z", 1.f);
	float w = node.attrFloat(L"W", 1.f);
	value.Set(x,y,z,w);
}

template<>
void LXPropertyT<vec4f>::SaveXML2(const TSaveContext& saveContext,  const LXString& strXMLName, const vec4f& v)
{
	fwprintf(saveContext.pXMLFile, L"<%s X=\"%f\" Y=\"%f\" Z=\"%f\" W=\"%f\"/>\n", strXMLName.GetBuffer(), v.x, v.y, v.z, v.w);
}

//
// --- LXAssetPtr ---
//

template<>
void LXPropertyT<LXAssetPtr>::GetValueFromXML2(const TLoadContext& LoadContext, LXAssetPtr& value )
{
	const LXMSXMLNode& node = LoadContext.node;
	LXString strFilename;
	GetValueFromXML(node, strFilename);
	if (strFilename.IsEmpty())
	{
		value = NULL;
		return;
	}

	LXProject* Project = GetCore().GetProject();
	CHK(Project);
	if (Project)
	{
		LXAssetManager& mm = Project->GetAssetManager();
		value = mm.GetAsset(strFilename);
		CHK(value);
	}
}

template<>
void LXPropertyT<LXAssetPtr>::SaveXML2(const TSaveContext& saveContext,  const LXString& strXMLName, const LXAssetPtr& v)
{
	LXString strFilename;
	if (v)
		strFilename = v->GetRelativeFilename();
	::SaveXML(saveContext, strXMLName, strFilename);
}

template<>
void LXPropertyT<LXMaterialNodePtr>::GetValueFromXML2(const TLoadContext& LoadContext, LXMaterialNodePtr& value)
{
	const LXMSXMLNode& node = LoadContext.node;
	LXString XMLValue;
	GetValueFromXML(node, XMLValue);
 
	if (XMLValue.IsEmpty())
	{
		value = NULL;
		return;
	}

	LXMaterial* Material = dynamic_cast<LXMaterial*>(_Owner);
	CHK(Material);
	LXMaterialNode* MaterialNode = Material->GetMaterialNodeFromUID(XMLValue);
	value = MaterialNode;
}

template<>
void LXPropertyT<LXMaterialNodePtr>::SaveXML2(const TSaveContext& saveContext, const LXString& strXMLName, const LXMaterialNodePtr& v)
{
	LXString* pUID = nullptr;
	if (v)
		pUID = v->GetUID(true);
	fwprintf(saveContext.pXMLFile, L"<%s Value=\"%s\"/>\n", strXMLName.GetBuffer(), pUID?pUID->GetBuffer():L"");
}

map<LXString, int> groupPositions;

int LXProperty::GetGroupPosition(const LXString& strGroup)
{
	return groupPositions[strGroup];
}

void LXProperty::SetCurrentGroup(const LXString& strGroup)
{ 
	LXPropertyInfo::_CurrentGroup = strGroup;

	static int currentPosition = 0; 
	auto It = groupPositions.find(strGroup);
	if (It == groupPositions.end())
	{
		groupPositions[strGroup] = currentPosition;
		currentPosition++;
	}
}

void LXPropertyEnum::LoadXML(const TLoadContext& LoadContext)
{
	uint val;
	GetValueFromXML2(LoadContext, val);

	// We verify that the val is a existing choice
	// Else we set the first possible value
	
	for (uint i = 0; i < _arrayValues.size(); i++)
	{
		if (_arrayValues[i] == val)
		{
			SetValue(val, false);
			return;
		}
	}

	if (_arrayValues.size() > 0)
		val = _arrayValues[0];
	else
		CHK(0); 
	
	SetValue(val, false);
	
}

//
// --- ListSmartObjects ---
//

template<>
void LXPropertyT<ListSmartObjects>::GetValueFromXML2(const TLoadContext& LoadContext, ListSmartObjects& value)
{
	CHK(0);
}

template<>
void LXPropertyT<ListSmartObjects>::SaveXML2(const TSaveContext& saveContext, const LXString& strXMLName, const ListSmartObjects& v)
{
	fwprintf(saveContext.pXMLFile, L"<%s>\n", strXMLName.GetBuffer());
	for (LXSmartObject *SmartObject : v)
	{
		SmartObject->Save(saveContext);
	}
	fwprintf(saveContext.pXMLFile, L"</%s>\n", strXMLName.GetBuffer());
}

//
// --- ArraySmartObjects ---
//

template<>
void LXPropertyT<ArraySmartObjects>::GetValueFromXML2(const TLoadContext& LoadContext, ArraySmartObjects& value)
{
	const LXMSXMLNode& node = LoadContext.node;
	const LXString& name = node.name();
	for (LXMSXMLNode e = node.begin(); e != node.end(); e++)
	{
		if (e.name() == L"LXMaterialNodeTextureSampler")
		{
			TLoadContext loadContextChild(e);
			loadContextChild.pOwner = LoadContext.pOwner;
			loadContextChild.filepath = LoadContext.filepath;

			LXMaterial* Material = dynamic_cast<LXMaterial*>(LoadContext.pOwner);
			CHK(Material);

			LXMaterialNodeTextureSampler* p = new LXMaterialNodeTextureSampler(Material, nullptr, EShader::UndefinedShader, 0);
			p->Load(loadContextChild);
			value.push_back(p);
		}
		else if (e.name() == "LXMaterialNodeFloat")
		{
			TLoadContext loadContextChild(e);
			loadContextChild.pOwner = LoadContext.pOwner;
			loadContextChild.filepath = LoadContext.filepath;

			LXMaterial* Material = dynamic_cast<LXMaterial*>(LoadContext.pOwner);
			CHK(Material);

			LXMaterialNodeFloat* p = new LXMaterialNodeFloat(Material, 0.0f);
			p->Load(loadContextChild);
			value.push_back(p);
		}
		else if (e.name() == "LXMaterialNodeColor")
		{
			TLoadContext loadContextChild(e);
			loadContextChild.pOwner = LoadContext.pOwner;
			loadContextChild.filepath = LoadContext.filepath;

			LXMaterial* Material = dynamic_cast<LXMaterial*>(LoadContext.pOwner);
			CHK(Material);

			LXMaterialNodeColor* p = new LXMaterialNodeColor(Material, LXColor4f(0.0f, 0.0f, 0.0f, 0.0f));
			p->Load(loadContextChild);
			value.push_back(p);
		}
		else
			CHK(0);
	}
}

template<>
void LXPropertyT<ArraySmartObjects>::SaveXML2(const TSaveContext& saveContext, const LXString& strXMLName, const ArraySmartObjects& v)
{
	fwprintf(saveContext.pXMLFile, L"<%s>\n", strXMLName.GetBuffer());
	for (LXSmartObject* SmartObject : v)
	{
		SmartObject->Save(saveContext);
	}
	fwprintf(saveContext.pXMLFile, L"</%s>\n", strXMLName.GetBuffer());
}

//
// --- ArrayVec3f ---
//

template<>
void LXPropertyT<ArrayVec3f>::GetValueFromXML2(const TLoadContext& LoadContext, ArrayVec3f& value)
{
	const LXMSXMLNode& node = LoadContext.node;
	const LXString& name = node.name();
	for (LXMSXMLNode e = node.begin(); e != node.end(); e++)
	{
		if (e.name() == L"Vector3f")
		{
			vec3f v;
			GetValueFromXML(e, v);
			value.push_back(v);
		}
		else
			CHK(0);
	}
}

template<>
void LXPropertyT<ArrayVec3f>::SaveXML2(const TSaveContext& saveContext, const LXString& strXMLName, const ArrayVec3f& v)
{
	fwprintf(saveContext.pXMLFile, L"<%s>\n", strXMLName.GetBuffer());
	for (const vec3f elem : v)
	{
		fwprintf(saveContext.pXMLFile, L"<Vector3f X=\"%f\" Y=\"%f\" Z=\"%f\"/>\n", elem.x, elem.y, elem.z);
	}
	fwprintf(saveContext.pXMLFile, L"</%s>\n", strXMLName.GetBuffer());
}

//
// LXPropertyT<T>::GetTypeName Default & Specializations
//

template<> LXString LXPropertyT<bool>::GetTypeName() { return L"bool"; }
template<> LXString LXPropertyT<int>::GetTypeName() { return L"int"; }
template<> LXString LXPropertyT<float>::GetTypeName() { return L"float"; }

template<class T>
LXString LXPropertyT<T>::GetTypeName()
{
	LogE(Property, L"Missing specialization for property %s", _PropInfo->_Name.GetBuffer());
	return L"";
}

