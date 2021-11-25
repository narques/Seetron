//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXDelegate.h"
#include "LXPropertyIdentifiers.h"
#include "LXPropertyType.h"
#include "LXVariant.h"

typedef std::list<LXPropertyID> ListPropertyID;
typedef std::pair<LXPropertyID, LXVariant*> PairPropetyIDVariant;

class LXProperty;
class LXAsset;
class LXFilepath;
class LXMatrix;
typedef std::shared_ptr<LXAsset> LXAssetPtr;

struct TSaveContext;
struct TLoadContext;

typedef std::list<LXProperty*>	ListProperties;

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

class LXENGINE_API LXProperty// : public LXObject
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
	
	virtual	LXVariant*		CreateVariant() { CHK(0); return nullptr; }
	virtual void*			GetVarPtr		( ) const = 0;

	// User property tools
	virtual	LXString		GetTypeName() = 0;
	virtual LXString		GetMinXMLAttribute() { CHK(0); return L""; }
	virtual LXString		GetMaxXMLAttribute() { CHK(0); return L""; }
	
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


