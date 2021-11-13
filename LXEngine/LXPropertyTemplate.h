//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) NicolasArques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXProperty.h"

// Seetron
#include "LXColor.h"
#include "LXVariant.h"
#include "LXVec2.h"
#include "LXVec3.h"
#include "LXVec4.h"

class LXMSXMLNode;

class LXPropertyHelper
{
public:
	static void GetValueFromXML(const LXMSXMLNode& node, vec2f& value);
	static void GetValueFromXML(const LXMSXMLNode& node, vec3f& value);
	static void GetValueFromXML(const LXMSXMLNode& node, LXString& value);
	static void SaveXML(const TSaveContext& saveContext, const LXString& strXMLName, const LXString& value);
};

template<class T>
class LXENGINE_API LXPropertyT : public LXProperty
{

public:

	// Types

	typedef void		(LXSmartObject::* LXSETFUNC)	(const T&);
	typedef const T& (LXSmartObject::* LXGETFUNC)	() const;

	// Constructors & Destructor

	virtual LXProperty* Create(const LXString& name, void* var) const override;

	LXPropertyT(EPropertyType type);
	LXPropertyT(const LXPropertyT& prop);
	virtual ~LXPropertyT();

	// Overridden from LXProperty

	virtual void		LoadXML(const TLoadContext& LoadContext);
	virtual void		SaveXML(const TSaveContext& saveContext);

	// Misc
	void				SetLambdaOnGet(std::function<T()> eval) { _funcOnGet = eval; }
	void				SetLambdaOnSet(std::function<void(const T&)> eval) { CHK(!_Var); _funcOnSet = eval; }
	
	void				SetVarPtr(T* pVar) { CHK(!_funcOnSet); _Var = pVar; }
	void* GetVarPtr() const override { return _Var; }

	const T& GetValue() const;
	void				SetValue(const T& value, bool InvokeOnPropertyChanged = true);

	bool				HasMinMax() const { return _PropInfo->_MaxValue && _PropInfo->_MinValue; }
	bool				HasMax() const { return _PropInfo->_MaxValue != nullptr; }
	bool				HasMin() const { return _PropInfo->_MinValue != nullptr; }
	T* GetMax() const { return (T*)_PropInfo->_MaxValue; }
	T* GetMin() const { return (T*)_PropInfo->_MinValue; }
	void				SetMinMax(const T& valueMin, const T& valueMax, bool clamp = false);
	void				SetMin(const T& valueMin);
	void				SetMax(const T& valueMax);
	T					CheckRange(const T& value);
	bool				ClampToMinMax() const { return _PropInfo->_clampToMinMax; }

	virtual	LXVariant* CreateVariant()
	{
		LXVariant* pVariant = new LXVariantT<T>(GetValue());
		return pVariant;
	}

	virtual void 		SetValue(const LXVariant& variant, bool InvokeOnPropertyChanged = true)
	{
		const LXVariantT<T>* pVariantTyped = dynamic_cast<const LXVariantT<T>*>(&variant);
		CHK(pVariantTyped);
		SetValue(pVariantTyped->GetValue(), InvokeOnPropertyChanged);
	}

	int GetDataSize() const override { return sizeof(T); }

	//
	// ListSmartObjects specifics
	//
	void GetChoices(ArrayStrings& arrayStrings);
	bool AddItem(const LXString& itemName);
	bool RemoveItem(LXSmartObject* item);
	LXDelegate<ArrayStrings&> GetChoiceNames;
	LXDelegate<const LXString&> OnAddItem;
	LXDelegate<LXSmartObject*> OnRemoveItem;

protected:

	void				SaveXML2(const TSaveContext& saveContext, const LXString& strXMLName, const T& value);
	LXString			GetTypeName() override;
	LXString			GetMinXMLAttribute() override;
	LXString			GetMaxXMLAttribute() override;
	void				GetValueFromXML2(const TLoadContext& LoadContext);

private:

	// Some property type could need an extra load, like assets.
	void				LoadValue(const T& value);

private:

	std::function<T()>					_funcOnGet;
	std::function<void(const T&)>		_funcOnSet;

	T* _Var;

};

//--------------------------------------------------------------------------
// LXPropertyEnum
//--------------------------------------------------------------------------

class LXENGINE_API LXPropertyEnum : public LXPropertyT<uint>
{

public:

	LXPropertyEnum() :LXPropertyT<uint>(EPropertyType::Enum) {}
	virtual ~LXPropertyEnum(void) {}

	// Optional callback for a dynamic choices filling
	// If serialized result is undetermined: ID loaded and no choice available.
	void SetBuildChoicesFunc(std::function<void(LXPropertyEnum*)> func) { _funcBuildChoices = func; }

	void AddChoice(const LXString& strChoice, uint nValue) { _arrayChoices.push_back(strChoice); _arrayValues.push_back(nValue); }

	const LXString& GetChoice() const
	{
		for (uint i = 0; i < _arrayValues.size(); i++)
		{
			if (_arrayValues[i] == GetValue())
			{
				return _arrayChoices[i];
			}
		}
		CHK(0);
		static const LXString strInvalidChoice = L"INVALID CHOICE";
		return strInvalidChoice;
	}

	const ArrayStrings& GetChoices()	const;
	const ArrayUint& GetValues()		const { return _arrayValues; }
	void SetBitmask(bool b) { _bBitmask = b; }
	bool IsBitmask() const { return _bBitmask; }

	virtual void LoadXML(const TLoadContext& LoadContext);

private:

	bool _bBitmask = false;
	mutable ArrayStrings _arrayChoices;
	mutable ArrayUint    _arrayValues;
	std::function<void(LXPropertyEnum*)> _funcBuildChoices;

};

typedef std::list<std::shared_ptr<LXSmartObject>>			ListSharedObjects;

typedef LXPropertyT<bool>						LXPropertyBool;
typedef LXPropertyT<int>						LXPropertyInt;
typedef LXPropertyT<uint>						LXPropertyUint;
typedef LXPropertyT<float>						LXPropertyFloat;
typedef LXPropertyT<double>						LXPropertyDouble;
typedef LXPropertyT<vec2f>						LXPropertyVec2f;
typedef LXPropertyT<vec3f>						LXPropertyVec3f;
typedef LXPropertyT<vec4f>						LXPropertyVec4f;
typedef LXPropertyT<LXColor4f>					LXPropertyLXColor4f;
typedef LXPropertyT<LXString>					LXPropertyString;
typedef LXPropertyT<LXFilepath>					LXPropertyFilepath;
typedef LXPropertyT<LXMatrix>					LXPropertyMatrix;
typedef LXPropertyT<std::shared_ptr<LXAsset>>		LXPropertyAssetPtr;
typedef LXPropertyT<ArraySmartObjects>			LXPropertyArraySmartObjects;
typedef LXPropertyT<ListSmartObjects>			LXPropertyListSmartObjects;
typedef LXPropertyT<ArrayVec3f>					LXPropertyArrayVec3f;
typedef LXPropertyT<LXSmartObject>				LXPropertySmartObject;
typedef LXPropertyT<std::shared_ptr<LXSmartObject>>	LXPropertySharedObject;
typedef LXPropertyT<LXReference<LXSmartObject>>	LXPropertyReferenceObject;
typedef LXPropertyT<ListSharedObjects>			LXPropertyListSharedObjects;
