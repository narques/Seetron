//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXAsset.h"
#include "LXAssetManager.h"
#include "LXCore.h"
#include "LXGraphMaterial.h"
#include "LXLogger.h"
#include "LXMSXMLNode.h"
#include "LXMaterial.h"
#include "LXMaterialNode.h"
#include "LXMatrix.h"
#include "LXNode.h"
#include "LXObjectFactory.h"
#include "LXProject.h"
#include "LXProperty.h"
#include "LXSettings.h"
#include "LXMemory.h" // --- Must be the last included ---

#define LX_DECLARE_GETTEMPLATETYPE(nativeType, enumType)			\
template<>													\
EPropertyType LXProperty::GetTemplateType<##nativeType>()	\
{															\
	return enumType;										\
}															\

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
	fwprintf(saveContext.pXMLFile, L"%s", GetTab(saveContext.Indent).GetBuffer());
	fwprintf(saveContext.pXMLFile,L"<%s Value=\"%s\"/>\n", strXMLName.GetBuffer(), value.GetBuffer());
}

//--------------------------------------------------------------------------
// LXProperty
//--------------------------------------------------------------------------

LXString LXPropertyInfo::_CurrentGroup = L"MISC";

LXProperty* LXProperty::Create(const LXString& name, void* var) const
{
	// Must be implemented in child classes
	CHK(0);
	return nullptr;
}

LXProperty::LXProperty(EPropertyType type):
_Owner(nullptr),
_Type(type)
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

LX_DECLARE_GETTEMPLATETYPE(LXMatrix, EPropertyType::Matrix)
LX_DECLARE_GETTEMPLATETYPE(bool, EPropertyType::Bool)
LX_DECLARE_GETTEMPLATETYPE(double, EPropertyType::Double)
LX_DECLARE_GETTEMPLATETYPE(float, EPropertyType::Float)
LX_DECLARE_GETTEMPLATETYPE(vec2f, EPropertyType::Float2)
LX_DECLARE_GETTEMPLATETYPE(vec3f, EPropertyType::Float3)
LX_DECLARE_GETTEMPLATETYPE(vec4f, EPropertyType::Float4)
LX_DECLARE_GETTEMPLATETYPE(int, EPropertyType::Int)
LX_DECLARE_GETTEMPLATETYPE(uint, EPropertyType::Uint)
LX_DECLARE_GETTEMPLATETYPE(LXString, EPropertyType::String)
LX_DECLARE_GETTEMPLATETYPE(LXFilepath, EPropertyType::Filepath)
LX_DECLARE_GETTEMPLATETYPE(LXAssetPtr, EPropertyType::AssetPtr)
LX_DECLARE_GETTEMPLATETYPE(LXColor4f, EPropertyType::Color)
LX_DECLARE_GETTEMPLATETYPE(vector<LXSmartObject*>, EPropertyType::ArraySmartObject)
LX_DECLARE_GETTEMPLATETYPE(list<LXSmartObject*>, EPropertyType::ListSmartObject)
LX_DECLARE_GETTEMPLATETYPE(ListSharedObjects, EPropertyType::ListSmartObject)
LX_DECLARE_GETTEMPLATETYPE(vector<vec3f>, EPropertyType::ArrayFloat3f)
LX_DECLARE_GETTEMPLATETYPE(LXSmartObject, EPropertyType::SmartObject)
LX_DECLARE_GETTEMPLATETYPE(shared_ptr<LXSmartObject>, EPropertyType::SharedObject)
LX_DECLARE_GETTEMPLATETYPE(LXReference<LXSmartObject>, EPropertyType::ReferenceObject)

//--------------------------------------------------------------------------
// LXPropertyT 
//--------------------------------------------------------------------------

template<class T>
LXProperty* LXPropertyT<T>::Create(const LXString& name, void* var) const
{
	LXPropertyT<T>* property = new LXPropertyT<T>(LXProperty::GetTemplateType<T>());
	property->SetName(name);
	property->SetID(GetAutomaticPropertyID());
	property->SetVarPtr((T*)var);
	return property;
}

template <class T>
LXPropertyT<T>::LXPropertyT(EPropertyType type):LXProperty(type), _Var(nullptr)
{
};

template <class T>
LXPropertyT<T>::LXPropertyT(const LXPropertyT& prop):LXProperty(prop._Type)
{
	_Var = prop._Var;
	*_PropInfo = *prop._PropInfo;
	_Owner = prop._Owner;

	_funcOnChange = prop._funcOnChange;
	ValueChanging = prop.ValueChanging;
	ValueChanged = prop.ValueChanged;
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
void LXPropertyT<T>::SetMinMax( const T& valueMin, const T& valueMax, bool clamp)
{ 
	LX_SAFE_DELETE(_PropInfo->_MinValue);
	LX_SAFE_DELETE(_PropInfo->_MaxValue);
	_PropInfo->_clampToMinMax = clamp;
	_PropInfo->_MinValue = new T(valueMin); 
	_PropInfo->_MaxValue = new T(valueMax); 
}

template <class T>
void LXPropertyT<T>::SetMin(const T& valueMin)
{
	LX_SAFE_DELETE(_PropInfo->_MinValue);
	_PropInfo->_MinValue = new T(valueMin);
}

template <class T>
void LXPropertyT<T>::SetMax(const T& valueMax)
{
	LX_SAFE_DELETE(_PropInfo->_MaxValue);
	_PropInfo->_MaxValue = new T(valueMax);
}

/*virtual*/
template <class T>
void LXPropertyT<T>::LoadXML(const TLoadContext& LoadContext)
{ 
	GetValueFromXML2(LoadContext);
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

		if (GetMin())
		{
			TagName += L" " + GetMinXMLAttribute();
		}

		if (GetMax())
		{
			TagName += L" " + GetMaxXMLAttribute();
		}
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
T LXPropertyT<T>::CheckRange(const T& value)
{
	return value;
}

template<typename T>
T CheckRange(const LXPropertyT<T>* property, const T& value)
{
	if (property->GetMin())
	{
		if (value < *property->GetMin())
		{
			if (property->ClampToMinMax())
			{
				LogI(LXProperty, L"'%s' property value %s clamped to min: %s", property->GetName().GetBuffer(), Number(value).GetBuffer(), Number(*property->GetMin()).GetBuffer());
				return *property->GetMin();
			}
			else
			{
				LogW(LXProperty, L"'%s' property value was out of range: %s min: %s", property->GetName().GetBuffer(), Number(value).GetBuffer(), Number(*property->GetMin()).GetBuffer());
				CHK(0);
			}
		}
	}
	if (property->GetMax())
	{
		if (value > *property->GetMax())
		{
			if (property->ClampToMinMax())
			{
				LogI(LXProperty, L"'%s' property value %s clamped to max: %s", property->GetName().GetBuffer(), Number(value).GetBuffer(), Number(*property->GetMax()).GetBuffer());
				return *property->GetMax();
			}
			else
			{
				LogW(LXProperty, L"'%s' property value was out of range: %s max: %s", property->GetName().GetBuffer(), Number(value).GetBuffer(), Number(*property->GetMax()).GetBuffer());
				CHK(0);
			}
		}
	}
	return value;
}

template<>
float LXPropertyT<float>::CheckRange(const float& value)
{
	return ::CheckRange(this, value);
}

template<>
double LXPropertyT<double>::CheckRange(const double& value)
{
	return ::CheckRange(this, value);
}

template<>
int LXPropertyT<int>::CheckRange(const int& value)
{
	return ::CheckRange(this, value);
}

template<>
uint LXPropertyT<uint>::CheckRange(const uint& value)
{
	return ::CheckRange(this, value);
}

template <class T>
void LXPropertyT<T>::SetValue( const T& value, bool InvokeOnProperyChanged )
{ 
	if (value == GetValue())
	{
		return;
	}
	 
	ValueChanging.Invoke(this);

	T clampedValue = CheckRange(value);

	LoadValue(clampedValue);

	if (_Var)
		*_Var = clampedValue;
	else if (_funcOnSet)
		_funcOnSet(clampedValue);
	else
	{
		// No way to set the Value to the property
		CHK(0);
	}

	// Event (By Property)
	if(_funcOnChange)
		_funcOnChange(this);

	ValueChanged.Invoke(this);
	
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
template class LXCORE_API LXPropertyT<ArraySmartObjects>;
template class LXCORE_API LXPropertyT<ListSmartObjects>;
template class LXCORE_API LXPropertyT<ArrayVec3f>;
template class LXCORE_API LXPropertyT<LXSmartObject>;
template class LXCORE_API LXPropertyT<shared_ptr<LXSmartObject>>;
template class LXCORE_API LXPropertyT<LXReference<LXSmartObject>>;
template class LXCORE_API LXPropertyT<ListSharedObjects>;

//
// --- float ---
//

template<>
void LXPropertyT<float>::GetValueFromXML2(const TLoadContext& LoadContext)
{
	const LXMSXMLNode& node = LoadContext.node;
	float value =  node.attrFloat(L"Value",1.f);
	SetValue(value, false);
}

template<>
void LXPropertyT<float>::SaveXML2(const TSaveContext& saveContext, const LXString& strXMLName, const float& value)
{
	CHK(!strXMLName.IsEmpty());
	fwprintf(saveContext.pXMLFile, L"%s", GetTab(saveContext.Indent).GetBuffer());
	fwprintf(saveContext.pXMLFile, L"<%s Value=\"%f\"/>\n", strXMLName.GetBuffer(), value);
}

//
// --- double ---
//

template<>
void LXPropertyT<double>::GetValueFromXML2(const TLoadContext& LoadContext)
{
	const LXMSXMLNode& node = LoadContext.node;
	double value = node.attrDouble(L"Value", 1.);
	SetValue(value, false);
}

template<>
void LXPropertyT<double>::SaveXML2(const TSaveContext& saveContext, const LXString& strXMLName, const double& value)
{
	CHK(!strXMLName.IsEmpty());
	fwprintf(saveContext.pXMLFile, L"%s", GetTab(saveContext.Indent).GetBuffer());
	fwprintf(saveContext.pXMLFile, L"<%s Value=\"%f\"/>\n", strXMLName.GetBuffer(), value);
}

//
// --- LXString ---
//

template<>
void LXPropertyT<LXString>::GetValueFromXML2(const TLoadContext& LoadContext)
{
	const LXMSXMLNode& node = LoadContext.node;
	LXString value = node.attr(L"Value");
	SetValue(value, false);
}

template<>
void LXPropertyT<LXString>::SaveXML2(const TSaveContext& saveContext, const LXString& strXMLName, const LXString& value )
{ 
	if (!value.IsEmpty() || _PropInfo->_bUserProperty)
	{
		fwprintf(saveContext.pXMLFile, L"%s", GetTab(saveContext.Indent).GetBuffer());
		fwprintf(saveContext.pXMLFile, L"<%s Value=\"%s\"/>\n", strXMLName.GetBuffer(), value.GetBuffer());
	}
}

//
// --- LXFilepath ---
//

template<>
void LXPropertyT<LXFilepath>::GetValueFromXML2(const TLoadContext& LoadContext)
{
	const LXMSXMLNode& node = LoadContext.node;
	LXFilepath value = LXFilepath(node.attr(L"Value"));
	SetValue(value, false);
}

template<>
void LXPropertyT<LXFilepath>::SaveXML2(const TSaveContext& saveContext, const LXString& strXMLName, const LXFilepath& value )
{ 
	fwprintf(saveContext.pXMLFile, L"%s", GetTab(saveContext.Indent).GetBuffer());
	fwprintf(saveContext.pXMLFile,L"<%s Value=\"%s\"/>\n", strXMLName.GetBuffer(), value.GetBuffer());
}

//
// --- LXColor4f ---
//

template<>
void LXPropertyT<LXColor4f>::GetValueFromXML2(const TLoadContext& LoadContext)
{
	const LXMSXMLNode& node = LoadContext.node;
	float r = node.attrFloat(L"R", 1.f);
	float g = node.attrFloat(L"G", 1.f);
	float b = node.attrFloat(L"B", 1.f);
	float a = node.attrFloat(L"A", 1.f);
	LXColor4f value;
	value.Set(r,g,b,a);
	SetValue(value, false);
}

template<>
void LXPropertyT<LXColor4f>::SaveXML2(const TSaveContext& saveContext,  const LXString& strXMLName, const LXColor4f& color)
{
	fwprintf(saveContext.pXMLFile, L"%s", GetTab(saveContext.Indent).GetBuffer());
	fwprintf(saveContext.pXMLFile, L"<%s R=\"%f\" G=\"%f\" B=\"%f\" A=\"%f\"/>\n", strXMLName.GetBuffer(), color.r, color.g, color.b, color.a);
}

//
// --- Int ---
//

template<>
void LXPropertyT<int>::GetValueFromXML2(const TLoadContext& LoadContext)
{
	const LXMSXMLNode& node = LoadContext.node;
	int value = node.attrInt(L"Value", 1);
	SetValue(value, false);
}

template<>
void LXPropertyT<int>::SaveXML2(const TSaveContext& saveContext,  const LXString& strXMLName, const int& value )
{
	fwprintf(saveContext.pXMLFile, L"%s", GetTab(saveContext.Indent).GetBuffer());
	fwprintf(saveContext.pXMLFile,L"<%s Value=\"%i\"/>\n", strXMLName.GetBuffer(), value);
}

//
// --- Uint ---
//

template<>
void LXPropertyT<uint>::GetValueFromXML2(const TLoadContext& LoadContext)
{
	const LXMSXMLNode& node = LoadContext.node;
	uint value =  node.attrUint(L"Value", 1);
	SetValue(value, false);
}

template<>
void LXPropertyT<uint>::SaveXML2(const TSaveContext& saveContext,  const LXString& strXMLName, const uint& value )
{
	fwprintf(saveContext.pXMLFile, L"%s", GetTab(saveContext.Indent).GetBuffer());
	fwprintf(saveContext.pXMLFile,L"<%s Value=\"%u\"/>\n", strXMLName.GetBuffer(), value);
}

//
// --- Bool ---
//

template<>
void LXPropertyT<bool>::GetValueFromXML2(const TLoadContext& LoadContext)
{
	const LXMSXMLNode& node = LoadContext.node;
	bool value = node.attrInt(L"Value", 0)!=0;
	SetValue(value, false);
}

template<>
void LXPropertyT<bool>::SaveXML2(const TSaveContext& saveContext,  const LXString& strXMLName, const bool& value )
{
	fwprintf(saveContext.pXMLFile, L"%s", GetTab(saveContext.Indent).GetBuffer());
	fwprintf(saveContext.pXMLFile,L"<%s Value=\"%s\"/>\n", strXMLName.GetBuffer(), value?L"1":L"0");
}

//
// --- LXMatrix --- 
//

template<>
void LXPropertyT<LXMatrix>::GetValueFromXML2(const TLoadContext& LoadContext)
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
	
	LXMatrix value;
	value.SetOrigin(vo);
	value.SetXYZ(vx, vy, vz);
	SetValue(value, false);
}

template<>
void LXPropertyT<LXMatrix>::SaveXML2(const TSaveContext& saveContext,  const LXString& strXMLName, const LXMatrix& value )
{
	vec3f vo = value.GetOrigin();
	vec3f vx = value.GetVx();
	vec3f vy = value.GetVy();
	vec3f vz = value.GetVz();

	fwprintf(saveContext.pXMLFile, L"%s", GetTab(saveContext.Indent).GetBuffer());
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
void LXPropertyT<vec2f>::GetValueFromXML2(const TLoadContext& LoadContext)
{
	const LXMSXMLNode& node = LoadContext.node;
	float x = node.attrFloat(L"X", 1.f);
	float y = node.attrFloat(L"Y", 1.f);
	vec2f value;
	value.Set(x,y);
	SetValue(value, false);
}

template<>
void LXPropertyT<vec2f>::SaveXML2(const TSaveContext& saveContext,  const LXString& strXMLName, const vec2f& v)
{
	fwprintf(saveContext.pXMLFile, L"%s", GetTab(saveContext.Indent).GetBuffer());
	fwprintf(saveContext.pXMLFile, L"<%s X=\"%f\" Y=\"%f\"/>\n", strXMLName.GetBuffer(), v.x, v.y);
}

//
// --- vec3f ---
//

template<>
void LXPropertyT<vec3f>::GetValueFromXML2(const TLoadContext& LoadContext)
{
	const LXMSXMLNode& node = LoadContext.node;
	float x = node.attrFloat(L"X", 1.f);
	float y = node.attrFloat(L"Y", 1.f);
	float z = node.attrFloat(L"Z", 1.f);
	vec3f value;
	value.Set(x,y,z);
	SetValue(value, false);
}

template<>
void LXPropertyT<vec3f>::SaveXML2(const TSaveContext& saveContext,  const LXString& strXMLName, const vec3f& v)
{
	fwprintf(saveContext.pXMLFile, L"%s", GetTab(saveContext.Indent).GetBuffer());
	fwprintf(saveContext.pXMLFile, L"<%s X=\"%f\" Y=\"%f\" Z=\"%f\"/>\n", strXMLName.GetBuffer(), v.x, v.y, v.z);
}

//
// --- vec4f ---
//

template<>
void LXPropertyT<vec4f>::GetValueFromXML2(const TLoadContext& LoadContext)
{
	const LXMSXMLNode& node = LoadContext.node;
	float x = node.attrFloat(L"X", 1.f);
	float y = node.attrFloat(L"Y", 1.f);
	float z = node.attrFloat(L"Z", 1.f);
	float w = node.attrFloat(L"W", 1.f);
	vec4f value;
	value.Set(x,y,z,w);
	SetValue(value, false);
}

template<>
void LXPropertyT<vec4f>::SaveXML2(const TSaveContext& saveContext,  const LXString& strXMLName, const vec4f& v)
{
	fwprintf(saveContext.pXMLFile, L"%s", GetTab(saveContext.Indent).GetBuffer());
	fwprintf(saveContext.pXMLFile, L"<%s X=\"%f\" Y=\"%f\" Z=\"%f\" W=\"%f\"/>\n", strXMLName.GetBuffer(), v.x, v.y, v.z, v.w);
}

//
// --- LXAssetPtr ---
//

template<>
void LXPropertyT<LXAssetPtr>::GetValueFromXML2(const TLoadContext& LoadContext)
{
	const LXMSXMLNode& node = LoadContext.node;
	LXString strFilename;
	LXAssetPtr value = nullptr;

	GetValueFromXML(node, strFilename);
	if (!strFilename.IsEmpty())
	{
		LXProject* Project = GetCore().GetProject();
		CHK(Project);
		if (Project)
		{
			LXAssetManager& mm = Project->GetAssetManager();
			value = mm.GetAsset(strFilename);
			CHK(value);
		}
	}

	SetValue(value, false);
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
void LXPropertyT<LXAssetPtr>::LoadValue(const LXAssetPtr& value)
{
	if (value)
	{
		value->Load();
	}
}

//
// --- LXAssetPtr ---
//

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

const ArrayStrings& LXPropertyEnum::GetChoices() const
{
	if (_funcBuildChoices)
	{
		_arrayChoices.clear();
		_arrayValues.clear();
		_funcBuildChoices(const_cast<LXPropertyEnum*>(this));
	}

	return _arrayChoices; 
}

void LXPropertyEnum::LoadXML(const TLoadContext& LoadContext)
{
	CHK(!_funcBuildChoices);

	GetValueFromXML2(LoadContext);
	uint val = GetValue();

	// Check if the value is a existing choice
	// Else we set the first possible value
	
	for (uint i = 0; i < _arrayValues.size(); i++)
	{
		if (_arrayValues[i] == val)
		{
			return;
		}
	}

	if (_arrayValues.size() > 0)
		val = _arrayValues[0];
	else
		CHK(0); 
}

//
// --- ArraySmartObjects ---
//

template<>
void LXPropertyT<ArraySmartObjects>::GetValueFromXML2(const TLoadContext& LoadContext)
{
	const LXMSXMLNode& node = LoadContext.node;
	const LXString& name = node.name();

	ArraySmartObjects value;

	for (LXMSXMLNode e = node.begin(); e != node.end(); e++)
	{
		LXSmartObject* smartObject = LXObjectFactory::CreateObject(e.name(), LoadContext.pOwner);
		if (smartObject)
		{
			TLoadContext loadContextChild(e);
			loadContextChild.pOwner = LoadContext.pOwner;
			loadContextChild.filepath = LoadContext.filepath;
			smartObject->Load(loadContextChild);
			value.push_back(smartObject);
		}
		else
		{
			LogE(LXProperty, L"Unknonw object type (%s) for array", e.name());
		}
	}

	SetValue(value, false);
}

template<>
void LXPropertyT<ArraySmartObjects>::SaveXML2(const TSaveContext& saveContext, const LXString& strXMLName, const ArraySmartObjects& v)
{
	if (v.size() > 0)
	{
		fwprintf(saveContext.pXMLFile, L"%s", GetTab(saveContext.Indent).GetBuffer());
		fwprintf(saveContext.pXMLFile, L"<%s>\n", strXMLName.GetBuffer());
		saveContext.Indent++;
		for (LXSmartObject* SmartObject : v)
		{
			SmartObject->Save(saveContext);
		}
		saveContext.Indent--;
		fwprintf(saveContext.pXMLFile, L"%s", GetTab(saveContext.Indent).GetBuffer());
		fwprintf(saveContext.pXMLFile, L"</%s>\n", strXMLName.GetBuffer());
	}
}

//
// --- ListSmartObjects ---
//

template<>
void LXPropertyT<ListSmartObjects>::GetValueFromXML2(const TLoadContext& LoadContext)
{
	const LXMSXMLNode& node = LoadContext.node;
	const LXString& name = node.name();

	ListSmartObjects value;

	for (LXMSXMLNode e = node.begin(); e != node.end(); e++)
	{
		LXSmartObject* smartObject = LXObjectFactory::CreateObject(e.name(), _Owner/*LoadContext.pOwner*/);
		if (smartObject)
		{
			TLoadContext loadContextChild(e);
			loadContextChild.pOwner = LoadContext.pOwner;
			loadContextChild.filepath = LoadContext.filepath;
			smartObject->Load(loadContextChild);
			value.push_back(smartObject);
		}
		else
		{
			LogE(LXProperty, L"Unknonw object type (%s) for List", e.name());
		}
	}

	SetValue(value, false);
}

template<>
void LXPropertyT<ListSmartObjects>::SaveXML2(const TSaveContext& saveContext, const LXString& strXMLName, const ListSmartObjects& v)
{
	if(v.size() > 0)
	{
		fwprintf(saveContext.pXMLFile, L"%s", GetTab(saveContext.Indent).GetBuffer());
		fwprintf(saveContext.pXMLFile, L"<%s>\n", strXMLName.GetBuffer());
		saveContext.Indent++;
		for (LXSmartObject* SmartObject : v)
		{
			SmartObject->Save(saveContext);
		}
		saveContext.Indent--;
		fwprintf(saveContext.pXMLFile, L"%s", GetTab(saveContext.Indent).GetBuffer());
		fwprintf(saveContext.pXMLFile, L"</%s>\n", strXMLName.GetBuffer());
	}
}

template <>
void LXPropertyT<ListSmartObjects>::GetChoices(ArrayStrings& arrayStrings)
{
	GetChoiceNames.Invoke(arrayStrings);
}

template <>
bool LXPropertyT<ListSmartObjects>::AddItem(const LXString& itemName)
{
	OnAddItem.Invoke(itemName);
	return true;
}

template <>
bool LXPropertyT<ListSmartObjects>::RemoveItem(LXSmartObject* item)
{
	OnRemoveItem.Invoke(item);
	return true;
}

//
// ListSharedObjects
//

template<>
void LXPropertyListSharedObjects::GetValueFromXML2(const TLoadContext& LoadContext)
{
	const LXMSXMLNode& node = LoadContext.node;
	const LXString& name = node.name();

	ListSharedObjects value;

	for (LXMSXMLNode e = node.begin(); e != node.end(); e++)
	{
		shared_ptr<LXSmartObject> smartObject = LXObjectFactory::CreateSharedObject(e.name(), _Owner/*LoadContext.pOwner*/);
		
		if (smartObject)
		{
			TLoadContext loadContextChild(e);
			loadContextChild.pOwner = LoadContext.pOwner;
			loadContextChild.filepath = LoadContext.filepath;
			smartObject->Load(loadContextChild);
			value.push_back(smartObject);
		}
		else
		{
			LogE(LXProperty, L"Unknonw object type (%s) for List", e.name());
		}
	}

	SetValue(value, false);
}

template<>
void LXPropertyListSharedObjects::SaveXML2(const TSaveContext& saveContext, const LXString& strXMLName, const ListSharedObjects& v)
{
	if (v.size() > 0)
	{
		fwprintf(saveContext.pXMLFile, L"%s", GetTab(saveContext.Indent).GetBuffer());
		fwprintf(saveContext.pXMLFile, L"<%s>\n", strXMLName.GetBuffer());
		saveContext.Indent++;
		for (const shared_ptr<LXSmartObject>& SmartObject : v)
		{
			SmartObject->Save(saveContext);
		}
		saveContext.Indent--;
		fwprintf(saveContext.pXMLFile, L"%s", GetTab(saveContext.Indent).GetBuffer());
		fwprintf(saveContext.pXMLFile, L"</%s>\n", strXMLName.GetBuffer());
	}
}

template <>
void LXPropertyListSharedObjects::GetChoices(ArrayStrings& arrayStrings)
{
	GetChoiceNames.Invoke(arrayStrings);
}

template <>
bool LXPropertyListSharedObjects::AddItem(const LXString& itemName)
{
	OnAddItem.Invoke(itemName);
	return true;
}

template <>
bool LXPropertyListSharedObjects::RemoveItem(LXSmartObject* item)
{
	OnRemoveItem.Invoke(item);
	return true;
}

//
// --- ArrayVec3f ---
//

template<>
void LXPropertyT<ArrayVec3f>::GetValueFromXML2(const TLoadContext& LoadContext)
{
	const LXMSXMLNode& node = LoadContext.node;
	const LXString& name = node.name();
	ArrayVec3f value;
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

	SetValue(value, false);
}

template<>
void LXPropertyT<ArrayVec3f>::SaveXML2(const TSaveContext& saveContext, const LXString& strXMLName, const ArrayVec3f& v)
{
	fwprintf(saveContext.pXMLFile, L"%s", GetTab(saveContext.Indent).GetBuffer());
	fwprintf(saveContext.pXMLFile, L"<%s>\n", strXMLName.GetBuffer());
	saveContext.Indent++;
	for (const vec3f elem : v)
	{
		fwprintf(saveContext.pXMLFile, L"%s", GetTab(saveContext.Indent).GetBuffer());
		fwprintf(saveContext.pXMLFile, L"<Vector3f X=\"%f\" Y=\"%f\" Z=\"%f\"/>\n", elem.x, elem.y, elem.z);
	}
	saveContext.Indent--;
	fwprintf(saveContext.pXMLFile, L"%s", GetTab(saveContext.Indent).GetBuffer());
	fwprintf(saveContext.pXMLFile, L"</%s>\n", strXMLName.GetBuffer());
}

//
// --- LXSmartObject ---
//

template<>
void LXPropertyT<LXSmartObject>::GetValueFromXML2(const TLoadContext& LoadContext)
{
	// To load the object as a property, set the className with 
	// the property name to pass the verification
	LXString className = LoadContext.node.name();
	_Var->Load(LoadContext, &className);
}

template<>
void LXPropertyT<LXSmartObject>::SaveXML2(const TSaveContext& saveContext, const LXString& strXMLName, const LXSmartObject& v)
{
	fwprintf(saveContext.pXMLFile, L"%s", GetTab(saveContext.Indent).GetBuffer());
	fwprintf(saveContext.pXMLFile, L"<%s>\n", strXMLName.GetBuffer());
	v.Save(saveContext, nullptr, nullptr, true);
	fwprintf(saveContext.pXMLFile, L"%s", GetTab(saveContext.Indent).GetBuffer());
	fwprintf(saveContext.pXMLFile, L"</%s>\n", strXMLName.GetBuffer());
}

//
// --- shared_ptr<LXSmartObject> ---
//

template<>
void LXPropertyT<shared_ptr<LXSmartObject>>::GetValueFromXML2(const TLoadContext& LoadContext)
{
	const LXMSXMLNode& node = LoadContext.node;
	LXString value = node.attr(L"Value");
	shared_ptr<LXSmartObject> smartObject = LoadContext.pOwner->GetObject(value);

	if (smartObject == nullptr)
	{
		LogE(LXProperty, L"%s %s property value not found: Referenced object does not exist", _Owner->GetName().GetBuffer(), GetName().GetBuffer());
	}

	SetValue(smartObject, false);
}

template<>
void LXPropertyT<shared_ptr<LXSmartObject>>::SaveXML2(const TSaveContext& saveContext, const LXString& strXMLName, const shared_ptr<LXSmartObject>& v)
{
	LXString* pUID = nullptr;
	if (v)
		pUID = v->GetUID(true);
	fwprintf(saveContext.pXMLFile, L"%s", GetTab(saveContext.Indent).GetBuffer());
	fwprintf(saveContext.pXMLFile, L"<%s Value=\"%s\"/>\n", strXMLName.GetBuffer(), pUID ? pUID->GetBuffer() : L"");
}

//
// --- LXReference<LXSmartObject> ---
//

template<>
void LXPropertyT<LXReference<LXSmartObject>>::GetValueFromXML2(const TLoadContext& LoadContext)
{
	const LXMSXMLNode& node = LoadContext.node;
	LXString value = node.attr(L"Value");
	LXReference<LXSmartObject> smartObject = LoadContext.pOwner->GetObjectAsRef(value);

	if (smartObject == nullptr)
	{
		LogE(LXProperty, L"%s %s property value not found: Referenced object does not exist", _Owner->GetName().GetBuffer(), GetName().GetBuffer());
	}

	SetValue(smartObject, false);
}

template<>
void LXPropertyT<LXReference<LXSmartObject>>::SaveXML2(const TSaveContext& saveContext, const LXString& strXMLName, const LXReference<LXSmartObject>& v)
{
	LXString* pUID = nullptr;
	if (v.get())
		pUID = v->GetUID(true);
	fwprintf(saveContext.pXMLFile, L"%s", GetTab(saveContext.Indent).GetBuffer());
	fwprintf(saveContext.pXMLFile, L"<%s Value=\"%s\"/>\n", strXMLName.GetBuffer(), pUID ? pUID->GetBuffer() : L"");
}

//
// --- Default ---
//

template <class T>
void LXPropertyT<T>::GetChoices(ArrayStrings& arrayStrings)
{
}

template <class T>
bool LXPropertyT<T>::AddItem(const LXString& itemName)
{
	return true;
}

template <class T>
bool LXPropertyT<T>::RemoveItem(LXSmartObject* item)
{
	return true;
}

//
// --- Specializations ---
//

// GetTypeName

template<> LXString LXPropertyT<bool>::GetTypeName() { return L"bool"; }
template<> LXString LXPropertyT<int>::GetTypeName() { return L"int"; }
template<> LXString LXPropertyT<float>::GetTypeName() { return L"float"; }
template<> LXString LXPropertyT<vec2f>::GetTypeName() { return L"float2"; }
template<> LXString LXPropertyT<vec3f>::GetTypeName() { return L"float3"; }
template<> LXString LXPropertyT<vec4f>::GetTypeName() { return L"float4"; }
template<> LXString LXPropertyT<LXAssetPtr>::GetTypeName() { return L"LXAssetPtr"; }
template<> LXString LXPropertyT<LXString>::GetTypeName() { return L"LXString"; }
template<class T>
LXString LXPropertyT<T>::GetTypeName()
{
	CHK(0);
	LogE(LXProperty, L"Missing GetTypeName() specialization for property %s", _PropInfo->_Name.GetBuffer());
	return L"";
}

// GetMinXMLAttribute

template<> LXString LXPropertyT<float>::GetMinXMLAttribute() { return L"Min=\"" + LXString::Number(*GetMin()) + "\""; }
template<class T>
LXString LXPropertyT<T>::GetMinXMLAttribute()
{
	CHK(0);
	LogE(LXProperty, L"Missing GetMinXMLAttribute() specialization for property %s", _PropInfo->_Name.GetBuffer());
	return L"";
}

// GetMaxXMLAttribute

template<> LXString LXPropertyT<float>::GetMaxXMLAttribute() { return L"Max=\"" + LXString::Number(*GetMax()) +"\""; }
template<class T>
LXString LXPropertyT<T>::GetMaxXMLAttribute()
{
	CHK(0);
	LogE(LXProperty, L"Missing GetMaxXMLAttribute() specialization for property %s", _PropInfo->_Name.GetBuffer());
	return L"";
}

//
// --- Additional non-specialized --- 
//

template<class T>
void LXPropertyT<T>::LoadValue(const T& value)
{
}
