//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXColor.h"
#include "LXDelegate.h"
#include "LXPropertyIdentifiers.h"
#include "LXPropertyType.h"
#include "LXVariant.h"
#include "LXVec2.h"
#include "LXVec3.h"
#include "LXVec4.h"

typedef list<LXPropertyID> ListPropertyID;
typedef pair<LXPropertyID, LXVariant*> PairPropetyIDVariant;

class LXProperty;
class LXAsset;
class LXFilepath;
class LXMatrix;
typedef shared_ptr<LXAsset> LXAssetPtr;

struct TSaveContext;
struct TLoadContext;

typedef list<LXProperty*>	ListProperties;

//--------------------------------------------------------------------------
// LXPropertyInfo
//--------------------------------------------------------------------------

class LXPropertyInfo
{
public:

	LXPropertyInfo(){}

	// UI
	LXString				_Label;				
	LXString				_GroupName = _CurrentGroup;
	LXString				_Description;
	// ID
	LXString				_Name;
	LXPropertyID			_ID = LXPropertyID::Undefined;
	// Misc
	bool					_bReadOnly = false;
	bool					_bPersistent = true;
	bool					_clampToMinMax = false;
	void*					_MinValue = nullptr;
	void*					_MaxValue = nullptr;
	static LXString			_CurrentGroup;
	bool					_bAnimatable = false;
	bool*					_bEnable = nullptr;	// Dependency to another propertyBool
	bool					_bUserProperty = false;
	int64					_userData = -1;
};

//--------------------------------------------------------------------------
// LXProperty
//--------------------------------------------------------------------------

class LXCORE_API LXProperty// : public LXObject
{
	friend class LXCommandManager;
	friend class LXCommandProperty;

public:

	// Creates a new 'var' based property
	virtual LXProperty* Create(const LXString& name, void* var) const;

	LXProperty(EPropertyType type);
	virtual ~LXProperty(void);

	// UI
	const LXString&			GetLabel		( ) const							{ return _PropInfo->_Label; }
	void					SetLabel		( const LXString& strLabel)			{ _PropInfo->_Label = strLabel; }

	const LXString&			GetGroupName	( ) const							{ return _PropInfo->_GroupName; }
	void					SetGroupName	( const LXString& strGroupName)		{ _PropInfo->_GroupName = strGroupName; }

	LXSmartObject*			GetOwner		( ) const							{ return _Owner; }

	bool					IsEnable() const									{ return _PropInfo->_bEnable?*_PropInfo->_bEnable : true; }
	void					SetEnable(bool* pBool)								{ _PropInfo->_bEnable = pBool;  }

	// Animation
	bool					IsAnimatable	( ) const							{ return _PropInfo->_bAnimatable; }
	void					SetAnimatable	( bool b )							{ _PropInfo->_bAnimatable = b; }

	// ID
	const LXString&			GetName			( ) const							{ return _PropInfo->_Name;  }	
	void					SetName			( const LXString& strName)			{ _PropInfo->_Name = strName; }
	
	LXPropertyID 			GetID			( ) const							{ return _PropInfo->_ID; }
	void					SetID			( const LXPropertyID& eID)			{ _PropInfo->_ID = eID; }

	bool					GetReadOnly		( ) const							{ return _PropInfo->_bReadOnly; }
	void					SetReadOnly		( bool bReadOnly )					{ _PropInfo->_bReadOnly = bReadOnly; }

	bool					GetPersistent	( ) const							{ return _PropInfo->_bPersistent; }
	void					SetPersistent	( bool bPersistent )				{ _PropInfo->_bPersistent = bPersistent; }

	const LXString&			GetDescription	( )	const							{ return _PropInfo->_Description; }
	void					SetDescription	( const LXString& strDescription )	{ _PropInfo->_Description = strDescription; }

	bool					GetUserProperty ( ) const							{ return _PropInfo->_bUserProperty;}
	void					SetUserProperty ( bool UserProperty )				{ _PropInfo->_bUserProperty = UserProperty; }

	int64					GetUserData() const { return _PropInfo->_userData; }
	void					SetUserData(int64 userData) { _PropInfo->_userData = userData; }

	LXSmartObject*			GetLXObject		( ) const;
	void					SetObject		( LXSmartObject* pObject );
	
	virtual void			LoadXML			( const TLoadContext& LoadContext ) = 0;		
	virtual void			SaveXML			( const TSaveContext& SaveContext ) = 0;

	static void				SetCurrentGroup	( const LXString& strGroup );
	static int				GetGroupPosition( const LXString& strGroup );
	
	virtual	LXVariant*		CreateVariant	( ) = 0;
	virtual void			SetValue		( const LXVariant& variant, bool InvokeOnPropertyChanged ) = 0;
	virtual void*			GetVarPtr		( ) const = 0;

	// User property tools
	virtual	LXString		GetTypeName() = 0;
	virtual LXString		GetMinXMLAttribute() = 0;
	virtual LXString		GetMaxXMLAttribute() = 0;
	virtual int				GetDataSize() const = 0;

	template<class T>
	static EPropertyType	GetTemplateType();

	EPropertyType			GetType() const { return _Type; }

public:

	LXDelegate<LXProperty*>	ValueChanging;
	LXDelegate<LXProperty*>	ValueChanged;

protected:

	EPropertyType			_Type;
	LXPropertyInfo*			_PropInfo;
	LXSmartObject*			_Owner;
};

//--------------------------------------------------------------------------
// LXPropertyT
//--------------------------------------------------------------------------

template<class T> 
class LXCORE_API LXPropertyT : public LXProperty
{

public:

	// Types

	typedef void		( LXSmartObject::* LXSETFUNC )	( const T& );
	typedef const T&	( LXSmartObject::* LXGETFUNC )	( ) const;
	
	// Constructors & Destructor

	virtual LXProperty* Create(const LXString& name, void* var) const override;

	LXPropertyT			( EPropertyType type );
	LXPropertyT			( const LXPropertyT& prop );
	virtual ~LXPropertyT( );
	
	// Overridden from LXProperty
	
	virtual void		LoadXML			( const TLoadContext& LoadContext ) ;
	virtual void		SaveXML			( const TSaveContext& saveContext );

	// Misc
	void				SetLambdaOnGet	(std::function<T()> eval)					{ _funcOnGet = eval; }
	void				SetLambdaOnSet	(std::function<void(const T&)> eval)		{ CHK(!_Var); _funcOnSet = eval; }
	void				SetLambdaOnChange(std::function<void(LXPropertyT*)> eval)   { _funcOnChange = eval; }

	void				SetVarPtr		( T* pVar )									{ CHK(!_funcOnSet); _Var = pVar; }
	void*				GetVarPtr		( ) const override							{ return _Var; }
	
	const T&			GetValue		( ) const;
	void				SetValue		( const T& value, bool InvokeOnPropertyChanged = true);

	bool				HasMinMax		( ) const									{ return _PropInfo->_MaxValue && _PropInfo->_MinValue; }
	bool				HasMax			( ) const									{ return _PropInfo->_MaxValue != nullptr; }
	bool				HasMin			( ) const									{ return _PropInfo->_MinValue != nullptr; }
	T*					GetMax			( ) const									{ return (T*)_PropInfo->_MaxValue; }
	T*					GetMin			( ) const									{ return (T*)_PropInfo->_MinValue; }
	void				SetMinMax		( const T& valueMin, const T& valueMax, bool clamp = false );
	void				SetMin			( const T& valueMin );
	void				SetMax			( const T& valueMax );
	T					CheckRange		( const T& value );
	bool				ClampToMinMax	( ) const { return _PropInfo->_clampToMinMax; }
		
	virtual	LXVariant*	CreateVariant()
	{
		LXVariant* pVariant = new LXVariantT<T>(GetValue());
		return pVariant;
	}

	virtual void 		SetValue(const LXVariant& variant, bool InvokeOnPropertyChanged = true )
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

	void				SaveXML2		 ( const TSaveContext& saveContext, const LXString& strXMLName, const T& value );
	LXString			GetTypeName() override;
	LXString			GetMinXMLAttribute() override;
	LXString			GetMaxXMLAttribute() override;
	void				GetValueFromXML2 ( const TLoadContext& LoadContext );

private:

	// Some property type could need an extra load, like assets.
	void				LoadValue(const T& value);

private:

	std::function<void(LXPropertyT*)>	_funcOnChange;  
	std::function<T()>					_funcOnGet;
	std::function<void(const T&)>		_funcOnSet;
	
	T*									_Var;
	
};

//--------------------------------------------------------------------------
// LXPropertyEnum
//--------------------------------------------------------------------------

class LXCORE_API LXPropertyEnum : public LXPropertyT<uint>
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
		for (uint i=0; i<_arrayValues.size(); i++)
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
	
	const ArrayStrings&		GetChoices()	const;
	const ArrayUint&		GetValues()		const { return _arrayValues; }
	void SetBitmask			(bool b) { _bBitmask = b; }
	bool IsBitmask			() const { return _bBitmask;}

	virtual void LoadXML(const TLoadContext& LoadContext);

private:

	bool _bBitmask = false;
	mutable ArrayStrings _arrayChoices;
	mutable ArrayUint    _arrayValues;
	std::function<void(LXPropertyEnum*)> _funcBuildChoices;
	
};

typedef list<shared_ptr<LXSmartObject>>			ListSharedObjects;

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
typedef LXPropertyT<shared_ptr<LXAsset>>		LXPropertyAssetPtr;
typedef LXPropertyT<ArraySmartObjects>			LXPropertyArraySmartObjects;
typedef LXPropertyT<ListSmartObjects>			LXPropertyListSmartObjects;
typedef LXPropertyT<ArrayVec3f>					LXPropertyArrayVec3f;	
typedef LXPropertyT<LXSmartObject>				LXPropertySmartObject;
typedef LXPropertyT<shared_ptr<LXSmartObject>>	LXPropertySharedObject;
typedef LXPropertyT<LXReference<LXSmartObject>>	LXPropertyReferenceObject;
typedef LXPropertyT<ListSharedObjects>			LXPropertyListSharedObjects;
