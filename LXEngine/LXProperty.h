//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXVariant.h"
#include "LXColor.h"
#include "LXVec2.h"
#include "LXVec3.h"
#include "LXVec4.h"
#include "LXPropertyIdentifiers.h"
#include "LXPropertyType.h"

typedef list<LXPropertyID> ListPropertyID;
typedef pair<LXPropertyID, LXVariant*> PairPropetyIDVariant;

class LXProperty;
class LXAsset;
class LXFilepath;
class LXMatrix;
typedef LXAsset* LXAssetPtr;

struct TSaveContext;
struct TLoadContext;

typedef list<LXProperty*>	ListProperties;

//--------------------------------------------------------------------------
// LXPropertyInfo
//--------------------------------------------------------------------------

class LXPropertyInfo
{
public:

	LXPropertyInfo() :
		_bReadOnly(false),
		_bPersistent(true),
		_GroupName(_CurrentGroup),
		_MinValue(nullptr),
		_MaxValue(nullptr),
		_ID(LXPropertyID::Undefined),
		_bAnimatable(false),
		_bEnable(nullptr),
		_bUserProperty(false)
	{
	}

	// UI
	LXString				_Label;				
	LXString				_GroupName;
	LXString				_Description;
	// ID
	LXString				_Name;
	LXPropertyID			_ID;
	// Misc
	bool					_bReadOnly;
	bool					_bPersistent;
	void*					_MinValue;
	void*					_MaxValue;
	static LXString			_CurrentGroup;
	bool					_bAnimatable;
	bool*					_bEnable;	// Dependency to another propertyBool
	bool					_bUserProperty;
};

//--------------------------------------------------------------------------
// LXProperty
//--------------------------------------------------------------------------

class LXCORE_API LXProperty// : public LXObject
{
	friend class LXCommandManager;
	friend class LXCommandProperty;

public:

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

	LXSmartObject*			GetLXObject		( ) const;
	void					SetObject		( LXSmartObject* pObject );
	
	virtual void			LoadXML			( const TLoadContext& LoadContext ) = 0;		
	virtual void			SaveXML			( const TSaveContext& SaveContext ) = 0;

	static void				SetCurrentGroup	( const LXString& strGroup );
	static int				GetGroupPosition( const LXString& strGroup );
	
	virtual	LXVariant*		CreateVariant	( ) = 0;
	virtual void			SetValue		( const LXVariant& variant, bool InvokeOnPropertyChanged ) = 0;
	virtual void*			GetVarPtr		( ) = 0;

	// User property tools
	virtual	LXString		GetTypeName() = 0;
	virtual LXString		GetMinXMLAttribute() = 0;
	virtual LXString		GetMaxXMLAttribute() = 0;
	virtual int				GetDataSize() const = 0;

	template<class T>
	static EPropertyType	GetTemplateType();

	EPropertyType			GetType() const { return _Type; }

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

	void				SetVarPtr		( T* pVar )									{ CHK(!_funcOnSet) _Var = pVar; }
	void*				GetVarPtr		( ) override								{ return _Var; }
	
	const T&			GetValue		( ) const;
	void				SetValue		( const T& value, bool InvokeOnPropertyChanged = true);
	
	bool				HasMinMax		( ) const									{ return _PropInfo->_MaxValue && _PropInfo->_MinValue; }
	bool				HasMax			( ) const									{ return _PropInfo->_MaxValue != nullptr; }
	bool				HasMin			( ) const									{ return _PropInfo->_MinValue != nullptr; }
	T*					GetMax			( ) const									{ return (T*)_PropInfo->_MaxValue; }
	T*					GetMin			( ) const									{ return (T*)_PropInfo->_MinValue; }
	void				SetMinMax		( const T& valueMin, const T& valueMax );
	void				SetMin			( const T& valueMin );
	void				SetMax			( const T& valueMax );
	bool				CheckRange		( const T& value );
	
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
		
protected:

	void				SaveXML2		 ( const TSaveContext& saveContext, const LXString& strXMLName, const T& value );
	LXString			GetTypeName() override;
		LXString		GetMinXMLAttribute() override;
	LXString			GetMaxXMLAttribute() override;
	void				GetValueFromXML2 ( const TLoadContext& LoadContext );

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
typedef LXPropertyT<LXAssetPtr>					LXPropertyAssetPtr;
typedef LXPropertyT<ArraySmartObjects>			LXPropertyArraySmartObjects;
typedef LXPropertyT<ListSmartObjects>			LXPropertyListSmartObjects;
typedef LXPropertyT<ArrayVec3f>					LXPropertyArrayVec3f;	
typedef LXPropertyT<LXSmartObject>				LXPropertySmartObject;
typedef LXPropertyT<shared_ptr<LXSmartObject>>	LXPropertySharedObject;
typedef LXPropertyT<LXReference<LXSmartObject>>	LXPropertyReferenceObject;
