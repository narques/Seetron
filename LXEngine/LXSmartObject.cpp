//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXSmartObject.h"
#include "LXSettings.h"
#include "LXProperty.h"
#include "LXEngine.h"
#include "LXLogger.h"
#include "LXPerformance.h"
#include "LXPropertyManager.h"
#include "LXMatrix.h"
#include "LXXMLDocument.h"
#include "LXMSXMLNode.h"
#include "LXVariant.h"
#include "LXAssetMesh.h"
#include "LXPropertyType.h"
#include "LXThreadManager.h"

typedef std::list<LXSmartObject*> ListSmartObjects;
typedef std::set<LXSmartObject*> SetSmartObjects;

std::map<LXObject*, std::function<void(LXSmartObject*, LXProperty*)>> LXSmartObject::_MapCBOnPropertiesChanged;

LXSmartObject::LXSmartObject()
{
	DefineProperties();
}

LXSmartObject::LXSmartObject(const LXSmartObject& Object)
{
	throw "Function is not implemented";
}

LXSmartObject& LXSmartObject::operator=(const LXSmartObject& Object)
{
	throw "Function is not implemented";
	return *this;
}

bool LXSmartObject::operator==(const LXSmartObject& smartObject) const
{
	throw "Function is not implemented";
	return true;
}

LXSmartObject::~LXSmartObject(void)
{
	for (auto ItProp = _listProperties.begin(); ItProp != _listProperties.end(); ItProp++)
	{
		delete *ItProp;
	}

	for (LXVariant* pVariant : _listVariants)
	{
		delete pVariant;
	}

	CHK(_MapCBOnPropertyChanged.size() == 0);
	
	LX_SAFE_DELETE(_pUID);
}

ListProperties LXSmartObject::GetProperties() const
{
	return _listProperties;
}

ListProperties LXSmartObject::GetBranchProperties() const
{
	ListProperties listProperties = GetProperties();
	GetChildProperties(listProperties);
	return listProperties;
}

void LXSmartObject::GetChildProperties(ListProperties& properties) const
{
	for (const LXProperty* property : _listProperties)
	{
		// Hard reference
		if (const LXPropertySmartObject* propertySmartObject = dynamic_cast<const LXPropertySmartObject*>(property))
		{
			const ListProperties& propertiesToAdd = propertySmartObject->GetValue().GetProperties();
			properties.insert(properties.end(), propertiesToAdd.begin(), propertiesToAdd.end());

			// Continue with grandchildren
			propertySmartObject->GetValue().GetChildProperties(properties);
		}

		// List
		else if (const LXPropertyListSmartObjects* propertyListSmartObject = dynamic_cast<const LXPropertyListSmartObjects*>(property))
		{
			for (const LXSmartObject* smartObject : propertyListSmartObject->GetValue())
			{
				const ListProperties& propertiesToAdd = smartObject->GetProperties();
				properties.insert(properties.end(), propertiesToAdd.begin(), propertiesToAdd.end());

				// Continue with grandchildren
				smartObject->GetChildProperties(properties);
			}
		}
		
		// Array
		else if (const LXPropertyArraySmartObjects* propertyListSmartObject = dynamic_cast<const LXPropertyArraySmartObjects*>(property))
		{
			for (const LXSmartObject* smartObject : propertyListSmartObject->GetValue())
			{
				const ListProperties& propertiesToAdd = smartObject->GetProperties();
				properties.insert(properties.end(), propertiesToAdd.begin(), propertiesToAdd.end());

				// Continue with grandchildren
				smartObject->GetChildProperties(properties);
			}
		}
		
	}
}

void LXSmartObject::GetUserProperties(ListProperties& UserProperties) const
{
	for (LXProperty* Property : _listProperties)
	{
		if (Property->GetUserProperty())
		{
			UserProperties.push_back(Property);
		}
	}
}

void LXSmartObject::AttachPropertiesToThis()
{
	for (auto It = _listProperties.begin(); It != _listProperties.end(); It++)
		(*It)->SetObject(this);
}

LXProperty* LXSmartObject::GetProperty(const LXPropertyID& PID)
{
	for(ListProperties::const_iterator It = _listProperties.begin();  It!=_listProperties.end(); It++)
	{
		LXProperty* pProperty = *It;
		if (pProperty->GetID() == PID)
		{
			return pProperty; 
		}
	}

	LogE(LXSmartObject,L"Property %i not found in object %s", PID, _Name.GetBuffer());
	return nullptr;
}

LXProperty* LXSmartObject::GetProperty(const LXString& name) const
{
	for (ListProperties::const_iterator It = _listProperties.begin(); It != _listProperties.end(); It++)
	{
		LXProperty* pProperty = *It;
		if (pProperty->GetName() == name)
		{
			return pProperty;
		}
	}
	return nullptr;
}

bool LXSmartObject::Save(const TSaveContext& saveContext, LXString* pName, LXString* pAttribute, bool saveAsProperty) const
{
	if (!_bPersistent)
		return true;

	if (_bSystem && !saveContext.bSaveSystem)
		return true;

	CHK(_bPersistent && !_bSystem);

	LXString strClassName;
	LXString strAttribute;

	if (pAttribute)
		strAttribute = *pAttribute;

	if (pName)
		strClassName = *pName;
	else
		strClassName = GetObjectName();

	if (!saveAsProperty)
	{
		fwprintf(saveContext.pXMLFile, L"%s", GetTab(saveContext.Indent).GetBuffer());
		fwprintf(saveContext.pXMLFile, L"<%s%s>\n", strClassName.GetBuffer(), strAttribute.GetBuffer());
	}

	saveContext.Indent++;
	
	for (ListProperties::const_iterator It = _listProperties.begin(); It!= _listProperties.end(); It++)
	{
		LXProperty* pProperty = *It;

		CHK(pProperty);

		if (pProperty && pProperty->GetPersistent())
		{
			pProperty->SaveXML(saveContext);
		}
	}

	if (saveContext.bSaveChilds)
	{
		OnSaveChild(saveContext);
	}

	saveContext.Indent--;

	if (!saveAsProperty)
	{
		fwprintf(saveContext.pXMLFile, L"%s", GetTab(saveContext.Indent).GetBuffer());
		fwprintf(saveContext.pXMLFile, L"</%s>\n", strClassName.GetBuffer());
	}

	return true;
}

bool LXSmartObject::LoadWithMSXML(const LXFilepath& strFilename, bool bLoadChilds /*= true*/, bool bLoadViewStates /*= true*/)
{
	bool ret = false;
	LXXMLDocument xml;
	if (xml.Load(strFilename))
	{
		LXMSXMLNode* node = xml.GetRoot();
		TLoadContext loadContext(*node);
		loadContext.pOwner = this;
		loadContext.filepath = strFilename;
		ret = Load(loadContext);
	}
	else
	{
		LogE(LXSmartObject, L"Unable to load %s", strFilename.GetBuffer());
	}
	return ret;
}

bool LXSmartObject::Load(const TLoadContext& loadContext, LXString* pName)
{
	_isLoading = true;

	// Create the std::map XMLName to Property
	TMapStringProperty mapProperties;
	
	for (ListProperties::const_iterator It = _listProperties.begin(); It!= _listProperties.end(); It++)
	{
		LXProperty* pProp = *It;
		mapProperties[pProp->GetName()] = pProp;
	}

	LXString strClassName = GetObjectName();
	LXString strNodeName = loadContext.node.name();
		
	if (pName)
		strClassName = *pName;

	if (strClassName == strNodeName)
	{
		for (LXMSXMLNode e = loadContext.node.begin(); e != loadContext.node.end(); e++)
		{
			TLoadContext loadContextChild(e);
			loadContextChild.pOwner = loadContext.pOwner;
			loadContextChild.filepath = loadContext.filepath;

			LXString strName = e.name();
			TMapStringProperty::const_iterator it = mapProperties.find(strName);
			LXProperty* property = nullptr;
			if (it != mapProperties.end())
			{
				property = it->second;
				property->LoadXML(loadContextChild);
				//OnPropertyLoaded(property); 

				// If the object contains a UID 
				if (property->GetID() == LXPropertyID::OBJECT_UID)
				{
					LXPropertyString* propertyString = dynamic_cast<LXPropertyString*>(property);
					if (propertyString->GetValue().IsEmpty() == false)
					{
						loadContext.pOwner->AddObject(propertyString->GetValue(), this);
					}
					else
					{
						// Warning empty uid
					}
				}
			}
			else
			{
				if (strName == L"UserProperty")
				{
					// Create the property
					LXString name = e.attr(L"Name");
					LXString type = e.attr(L"Type");
					LXString min = e.attr(L"Min");
					LXString max = e.attr(L"Max");
					
					if (type == "float")
					{
						property = CreateUserProperty<float>(name, 0.f);
						property->LoadXML(e);
						if (!min.IsEmpty())
						{ 
							((LXPropertyFloat*)property)->SetMin(min.ToFloat());

						}
						if (!max.IsEmpty())
						{
							((LXPropertyFloat*)property)->SetMax(max.ToFloat());
						}
					}
					else if (type == "float2")
					{
						property = CreateUserProperty<vec2f>(name, LX_VEC2F_NULL);
						property->LoadXML(e);
					}
					else if (type == "float3")
					{
						property = CreateUserProperty<vec3f>(name, LX_VEC3F_NULL);
						property->LoadXML(e);
					}
					else if (type == "float4")
					{
						property = CreateUserProperty<vec4f>(name, LX_VEC4F_NULL);
						property->LoadXML(e);
					}
					else if (type == "LXAssetPtr")
					{
						property = CreateUserProperty<LXAssetPtr>(name, nullptr);
						property->LoadXML(e);
					}
					else if (type == "LXString")
					{
						property = CreateUserProperty<LXString>(name, L"");
						property->LoadXML(e);
					}
					else
					{
						CHK(0);
						LogW(SmartObject, L"UserProperty format %s not supported. Property %s Object name %s", type.GetBuffer(), name.GetBuffer(), GetName().GetBuffer());
					}
				}

				if (!OnLoadChild(loadContextChild))
				{
					LogW(SmartObject, L"Unknown xml node %s in object %s", strName.GetBuffer(), GetObjectName().GetBuffer());
				}
			}
		}
	}
	else
	{
		CHK(0);
		_isLoading = false;
		return false;
	}

	_isLoading = false;
	_isLoaded = true;
	
	if (IsMainThread())
	{
		OnLoaded();
	}
	else
	{
		LXTask* task = new LXTaskCallBack([this]()
		{
			OnLoaded();
		});
		GetEngine().EnqueueTask(task);
	}

	return true;
}

void LXSmartObject::DefineProperties() 
{
	//--------------------------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Debug");
	//--------------------------------------------------------------------------------------------------------------------------------
	
	// ClassName
	{
		LXPropertyString* pProp = DefinePropertyString(L"ClassName", GetAutomaticPropertyID(), NULL);
		pProp->SetPersistent(false);
		pProp->SetReadOnly(true);
		pProp->SetLambdaOnGet([this] { return GetObjectName(); });
	}
	
	// Persistent
	{
		LXPropertyBool* pBool = DefinePropertyBool(L"Persistent", GetAutomaticPropertyID(), &_bPersistent);
		pBool->SetPersistent(false);
		pBool->SetReadOnly(true);
	}

	// System
	{
		LXPropertyBool* pBool = DefinePropertyBool(L"System", GetAutomaticPropertyID(), &_bSystem);
		pBool->SetPersistent(false);
		pBool->SetReadOnly(true);
	}

	// NeedSave
	{
		LXPropertyBool* pProp = DefinePropertyBool(L"NeedSave", GetAutomaticPropertyID(), &_bNeedSave);
		pProp->SetReadOnly(true);
		pProp->SetPersistent(false);
	}

	//--------------------------------------------------------------------------------------------------------------------------------
	LXProperty::SetCurrentGroup(L"Object");
	//--------------------------------------------------------------------------------------------------------------------------------
	
	// Name
	DefinePropertyName();

	// UID
	LXPropertyString* pPropUID = DefinePropertyString(L"UID", LXPropertyID::OBJECT_UID, NULL);
	pPropUID->SetReadOnly(true);
	pPropUID->SetLambdaOnGet([this]
	{
		if (_pUID)
			return *_pUID;
		else
			return LXString();
	});

	pPropUID->SetLambdaOnSet([this](const LXString& strUID)
	{
		if (!strUID.IsEmpty())
		{
			if (!_pUID)
				_pUID = new LXString(strUID);
			else
				*_pUID = strUID;
		}
	});
};

void LXSmartObject::OnPropertyChanged(LXProperty* Property)
{
	if (Property->GetPersistent() && _bPersistent)
	{
		_bNeedSave = true;
	}

	for (auto It : _MapCBOnPropertyChanged)
	{
		It.second(this, Property);
	}

	for (auto It : _MapCBOnPropertiesChanged)
	{
		It.second(this, Property);
	}

	PropertyChanged.Invoke(Property);
}

void LXSmartObject::RegisterCB_OnPropertyChanged(void* Listener, std::function<void(LXSmartObject*, LXProperty*)> func)
{
	_MapCBOnPropertyChanged[Listener] = func;
}

void LXSmartObject::UnregisterCB_OnPropertyChanged(void* Listener)
{
	_MapCBOnPropertyChanged.erase(Listener);
}

void LXSmartObject::RegisterCB_OnPropertiesChanged(LXObject* Listener, std::function<void(LXSmartObject*, LXProperty*)> func)
{
	_MapCBOnPropertiesChanged[Listener] = func;
}

void LXSmartObject::UnregisterCB_OnPropertiesChanged(LXObject* Listener)
{
	_MapCBOnPropertiesChanged.erase(Listener);
}

void LXSmartObject::RegisterCB(LXSmartObject* Listener, const LXString& FunctionName, std::function<void(LXSmartObject*)> func)
{
	TMapFunctions& MapFunctions = _MapGenericCB[Listener];
	MapFunctions[FunctionName] = func;
}

void LXSmartObject::UnregisterCB(LXSmartObject* Listener, const LXString& FunctionName)
{
	auto It = _MapGenericCB.find(Listener);
	if (It != _MapGenericCB.end())
	{
		TMapFunctions& MapFunctions = It->second;
		MapFunctions.erase(FunctionName);
		if (MapFunctions.size() == 0)
			_MapGenericCB.erase(Listener);
	}
	else
	{
		// Try to unregister a non registered object.
		CHK(0); 
	}
}

void LXSmartObject::InvokeCB(const LXString& FunctionName)
{
	bool Found = false;
	for (auto& It : _MapGenericCB)
	{
		auto It2 = It.second.find(FunctionName);
		if (It2 != It.second.end())
		{
			It2->second(this);
			Found = 0;
		}
	}
}

template<class T>
LXPropertyT<T>* LXSmartObject::DefineProperty(const LXString& name, const LXPropertyID& PID, T* var)
{
	LXPropertyT<T>* pProperty = new LXPropertyT<T>(LXProperty::GetTemplateType<T>());

	pProperty->SetName(name);
	pProperty->SetID(PID);
	pProperty->SetVarPtr(var);
	AddProperty(pProperty);
	return pProperty;
}

template<class T>
LXPropertyT<T>* LXSmartObject::DefineProperty(const LXString& name, const LXPropertyID& PID, T* var, T Min, T Max)
{
	LXPropertyT<T>* pProperty = new LXPropertyT<T>(LXProperty::GetTemplateType<T>());

	pProperty->SetName(name);
	pProperty->SetID(PID);
	pProperty->SetVarPtr(var);
	pProperty->SetMinMax(Min, Max);
	AddProperty(pProperty);
	return pProperty;
}

template<class T>
LXPropertyT<T>* LXSmartObject::DefineProperty(const LXString& label, const LXString& name, const LXPropertyID& PID, T* var)
{
	LXPropertyT<T>* pProperty = new LXPropertyT<T>(LXProperty::GetTemplateType<T>());

	pProperty->SetID(PID);
	pProperty->SetName(name);
	pProperty->SetLabel(label);
	pProperty->SetVarPtr(var);
	AddProperty(pProperty);
	return pProperty;
}

bool LXSmartObject::RemoveProperty(LXProperty* property)
{
	auto it = find(_listProperties.begin(), _listProperties.end(), property);
	if (it != _listProperties.end())
	{
		_listProperties.remove(property);
		return true;
	}
	return false;
}

template<class T>
LXPropertyT<T>*	LXSmartObject::DefinePropertyEval(const LXString& name, const LXPropertyID& id, std::function<int()> eval, T def)
{
	LXPropertyT<T>* pProperty = new LXPropertyT<T>();

	pProperty->SetName(name);
	pProperty->SetID(id);
	pProperty->SetLambdaOnGet(eval);
	AddProperty(pProperty);
	return pProperty;
}

LXPropertyEnum* LXSmartObject::DefinePropertyEnum(const LXString& name, const LXPropertyID& propID, uint* pEnum)
{
	LXPropertyEnum* pPropEnum = new LXPropertyEnum();

	pPropEnum->SetID(propID);
	pPropEnum->SetName(name);
	pPropEnum->SetVarPtr(pEnum);
	AddProperty(pPropEnum);
	return pPropEnum;
}

LXPropertyEnum* LXSmartObject::DefinePropertyEnum(const LXString& label, const LXString& name, const LXPropertyID& propID, uint* pEnum)
{
	LXPropertyEnum* pPropEnum = DefinePropertyEnum(label, propID, pEnum);
	pPropEnum->SetName(name);
	return pPropEnum;
}

LXPropertyEnum* LXSmartObject::DefinePropertyEnum(const LXString& name, uint* pEnum)
{
	return DefinePropertyEnum(name, GetAutomaticPropertyID(), pEnum);
}

void LXSmartObject::DefinePropertyName()
{
	DefineProperty<LXString>(L"Name", LXPropertyID::NAME, &_Name);
}

LXPropertyString* LXSmartObject::DefinePropertyString(const LXString& label, const LXPropertyID& PID, LXString* pString ) 
{
	return DefineProperty<LXString>(label, PID, pString);
}

LXPropertyFilepath* LXSmartObject::DefinePropertyFilepath(const LXString& label, const LXPropertyID& PID, LXFilepath* pFilename)
{
	return DefineProperty<LXFilepath>(label, PID, pFilename);
}

LXPropertyBool* LXSmartObject::DefinePropertyBool(const LXString& label, const LXPropertyID& PID, bool* pBool)
{
	return DefineProperty(label, PID, pBool);
}

LXPropertyBool*	LXSmartObject::DefinePropertyBool(const LXString& label, const LXString& strID, const LXPropertyID& PID, bool* pBool)
{
	return DefineProperty(label, strID, PID, pBool);
}

LXPropertyInt* LXSmartObject::DefinePropertyInt(const LXString& label, const LXPropertyID& PID, int* pInt)
{
	return DefineProperty(label, PID, pInt);
}

LXPropertyUint* LXSmartObject::DefinePropertyUint(const LXString& label, const LXPropertyID& PID, uint* pUint)
{
	return DefineProperty(label, PID, pUint);
}

LXPropertyFloat* LXSmartObject::DefinePropertyFloat(const LXString& label, const LXPropertyID& PID, float* pFloat)
{
	return DefineProperty(label, PID, pFloat);
}

LXPropertyFloat* LXSmartObject::DefinePropertyFloat(const LXString& label, const LXString& name, const LXPropertyID& PID, float* pFloat)
{
	LXPropertyFloat* p = DefineProperty(name, PID, pFloat);
	p->SetLabel(label);
	return p;
}

LXPropertyFloat* LXSmartObject::DefinePropertyFloat(const LXString& label, const LXPropertyID& PID, float* pFloat, float fMin, float fMax)
{
	LXPropertyFloat*  pPropFloat = DefineProperty(label, PID, pFloat);
	pPropFloat->SetMinMax(fMin, fMax);
	return pPropFloat;
}

LXPropertyVec4f* LXSmartObject::DefinePropertyVec4f(const LXString& label, const LXPropertyID& PID, vec4f* pVec4f)
{
	return DefineProperty(label, PID, pVec4f);
}

LXPropertyVec3f* LXSmartObject::DefinePropertyVec3f(const LXString& label, const LXPropertyID& PID, vec3f* pVec3f)
{
	return DefineProperty(label, PID, pVec3f);
}

LXPropertyLXColor4f* LXSmartObject::DefinePropertyColor4f(const LXString& label, const LXPropertyID& PID, LXColor4f* pLXColor4f)
{
	return DefineProperty(label, PID, pLXColor4f);
}

LXPropertyMatrix* LXSmartObject::DefinePropertyMatrix(const LXString& label, const LXPropertyID& PID, LXMatrix* pMatrix)
{
	return DefineProperty(label, PID, pMatrix);
}

LXPropertyAssetPtr* LXSmartObject::DefinePropertyAsset(const LXString& label, const LXPropertyID& PID, std::shared_ptr<LXAsset>* pAsset)
{
	return DefineProperty(label, PID, pAsset);
}

template<class T>
LXPropertyT<T>* LXSmartObject::CreateUserProperty(const LXString& Name, const T& DefaultValue)
{
	LXVariantT<T>* Var = new LXVariantT<T>(DefaultValue);
	_listVariants.push_back(Var);
	LXPropertyT<T>* Property = DefineProperty(Name, Var->GetValuePtr());
	Property->SetUserProperty(true);
	return Property;
}

template<class T>
LXPropertyT<T>* LXSmartObject::CreateUserProperty(const LXString& Name, LXPropertyID propertyID, const T& DefaultValue)
{
	LXVariantT<T>* Var = new LXVariantT<T>(DefaultValue);
	_listVariants.push_back(Var);
	LXPropertyT<T>* Property = DefineProperty(Name, propertyID, Var->GetValuePtr());
	Property->SetUserProperty(true);
	return Property;
}

bool LXSmartObject::AddProperty(LXProperty* pProperty)
{
	CHK(pProperty);
	if (!pProperty)
		return false;

	LXString xmlName = pProperty->GetName();
	LXPropertyID propID = pProperty->GetID();
	LXString xmlLabel = pProperty->GetLabel();
	
	// No name and no ID
	if (xmlName.IsEmpty() && propID == LXPropertyID::Undefined)
	{
		CHK(0);
		return false;
	}

	if (xmlName.find(L" ") != -1)
	{
		CHK(0);
		return false;
	}

	if (!xmlName.IsEmpty() && propID == LXPropertyID::Undefined)
	{
		static std::map<LXString, uint> mapXMLNameID;
		static uint ID = int(LXPropertyID::LASTID) + 1000; // +1000 Sinon risque de doublon d'ID avec ceux generes par la macro LX_PROPERTYID_AUTO
		
		auto It = mapXMLNameID.find(xmlName);
		if (It != mapXMLNameID.end())
		{
			propID = (LXPropertyID)It->second;
		}
		else
		{
			mapXMLNameID[xmlName] = ID;
			propID = (LXPropertyID)ID;
			ID++;
		}
 
		pProperty->SetID(propID);
	}

	CHK(propID != LXPropertyID::Undefined);

	for (auto It = _listProperties.begin(); It != _listProperties.end(); It++)
	{
		if ((*It)->GetID() == pProperty->GetID())
		{	
			// ID already used
			LogE(LXSmartObject, L"%s and %s have the same ID", pProperty->GetName().GetBuffer(), (*It)->GetName().GetBuffer());
			CHK(0); 
			return false;
		}

		if ((*It)->GetName() == pProperty->GetName())
		{
			LogE(LXSmartObject, L"Property name %s already used", pProperty->GetName().GetBuffer());
			CHK(0);
			return false;
		}
	}

	// Label
	if (xmlLabel.IsEmpty())
	{
		xmlLabel = LXPropertyManager::GetPropertyLabel(propID);
		if (!xmlLabel.IsEmpty())
			pProperty->SetLabel(xmlLabel);
		else
			pProperty->SetLabel(xmlName);
	}

	pProperty->SetObject(this);
	_listProperties.push_back(pProperty);
	
	return true;
}

void LXSmartObject::DeleteProperty(LXProperty* pProperty)
{
}

LXString* LXSmartObject::GetUID(bool bBuild) const
{
	if (bBuild && !_pUID)
	{
		_pUID = new LXString();
		*_pUID = LXPlatform::CreateUuid();
		// Use only data1 (ulong)
		*_pUID = _pUID->Left(L"-");
		//LogI(Engine, L"Generated UID for object \"%s\" (%s)", GetName().GetBuffer(), GetObjectName().GetBuffer());
	}
	return _pUID;
}

std::shared_ptr<LXSmartObject> LXSmartObject::GetObject(const LXString& uid)
{
	auto It = _objects.find(uid);
	if (It != _objects.end())
	{
		LXSmartObject* smartObject = It->second;
		return std::shared_ptr<LXSmartObject>(smartObject);
	}
	else
	{
		return nullptr;
	}
}

LXReference<LXSmartObject> LXSmartObject::GetObjectAsRef(const LXString& uid)
{
	auto It = _objects.find(uid);
	if (It != _objects.end())
	{
		LXSmartObject* smartObject = It->second;
		return LXReference<LXSmartObject>(smartObject);
	}
	else
	{
		return nullptr;
	}
}

void LXSmartObject::AddObject(const LXString& uid, LXSmartObject* smartObject)
{
	auto It = _objects.find(uid);
	// CHK commented as RemoveObject is still not used.
	//CHK(It == _objects.end()); 
	_objects[uid] = smartObject;
}

void LXSmartObject::RemoveObject(const LXString& uid, LXSmartObject* smartObject)
{
	auto It = _objects.find(uid);
	CHK(It != _objects.end());
	_objects.erase(uid);
}

LXSmartObjectContainer::LXSmartObjectContainer()
{
}

LXSmartObjectContainer::~LXSmartObjectContainer()
{
}

void LXSmartObjectContainer::AddSmartObject(LXSmartObject* pSmartObject)
{
	CHK(pSmartObject);
	if (!pSmartObject)
		return;

	// Check
	for (uint i=0; i<m_arraySmartObject.size(); i++)
	{
		// Already listed
		CHK(m_arraySmartObject[i] != pSmartObject);
		if (m_arraySmartObject[i] == pSmartObject)
			return;

		// If two (or more) instances of the same class, we need to use UID !
		if (m_arraySmartObject[i]->GetObjectName() == pSmartObject->GetObjectName())
		{
			if (!m_arraySmartObject[i]->GetUID(false) || !pSmartObject->GetUID(false))
			{
				CHK(0);
				return;
			}
		}
	}
	
	m_arraySmartObject.push_back(pSmartObject);

}

LXPropertyID GetAutomaticPropertyID()
{
	static uint i = 0;
	LXPropertyID id = (LXPropertyID)(int(LXPropertyID::LASTID) + i++);
	return id;
}

// Explicit method Instantiation

template LXENGINE_API LXPropertyInt* LXSmartObject::DefineProperty(const LXString& name, const LXPropertyID& PID, int* pInt, int min, int max);
template LXENGINE_API LXPropertyFloat* LXSmartObject::DefineProperty(const LXString& name, const LXPropertyID& PID, float* pFloat, float, float);
template LXENGINE_API LXPropertyDouble* LXSmartObject::DefineProperty(const LXString& name, const LXPropertyID& PID, double* pDouble);
template LXENGINE_API LXPropertyArraySmartObjects* LXSmartObject::DefineProperty(const LXString& name, const LXPropertyID& PID, ArraySmartObjects* pArraySmartObjects);
template LXENGINE_API LXPropertyListSmartObjects* LXSmartObject::DefineProperty(const LXString& name, const LXPropertyID& PID, ListSmartObjects* pListSmartObjects);
template LXENGINE_API LXPropertyListSharedObjects* LXSmartObject::DefineProperty(const LXString& name, const LXPropertyID& PID, ListSharedObjects* pListSmartObjects);
template LXENGINE_API LXPropertyAssetPtr* LXSmartObject::DefineProperty(const LXString& name, const LXPropertyID& PID, LXAssetPtr* pMaterialInput);
template LXENGINE_API LXPropertySmartObject* LXSmartObject::DefineProperty(const LXString& name, const LXPropertyID& PID, LXSmartObject* smartObject);
template LXENGINE_API LXPropertySharedObject* LXSmartObject::DefineProperty(const LXString& name, const LXPropertyID& PID, std::shared_ptr<LXSmartObject>* smartObject);
template LXENGINE_API LXPropertyReferenceObject* LXSmartObject::DefineProperty(const LXString& name, const LXPropertyID& PID, LXReference<LXSmartObject>* smartObject);

template LXENGINE_API LXPropertyInt* LXSmartObject::CreateUserProperty(const LXString& name, const int& var);
template LXENGINE_API LXPropertyFloat* LXSmartObject::CreateUserProperty(const LXString& name, const float& var);
template LXENGINE_API LXPropertyVec2f* LXSmartObject::CreateUserProperty(const LXString& name, const vec2f& var);
template LXENGINE_API LXPropertyVec3f* LXSmartObject::CreateUserProperty(const LXString& name, const vec3f& var);
template LXENGINE_API LXPropertyVec4f* LXSmartObject::CreateUserProperty(const LXString& name, const vec4f& var);
template LXENGINE_API LXPropertyArrayVec3f* LXSmartObject::CreateUserProperty(const LXString& name, const ArrayVec3f& var);
template LXENGINE_API LXPropertyAssetPtr* LXSmartObject::CreateUserProperty(const LXString& name, const LXAssetPtr& var);
template LXENGINE_API LXPropertyString* LXSmartObject::CreateUserProperty(const LXString& name, const LXString& var);

