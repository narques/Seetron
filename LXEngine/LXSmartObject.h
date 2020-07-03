//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include "LXFilepath.h"
#include "LXProperty.h"
#include "LXPropertyManager.h"

class LXMSXMLNode;
class LXSmartObject;

#define GetSet(type, var, funcname)											\
	public: LX_INLINE const type& Get##funcname() const { return var; }		\
	public: LX_INLINE void Set##funcname(const type& v) { var = v; }		\
	protected: type var;													\

#define GetSetDef(type, var, funcname, def)									\
	public: LX_INLINE const type& Get##funcname() const { return var; }		\
	public: LX_INLINE void Set##funcname(const type& v) { var = v; }		\
	protected: type var = def;

#define GetSetDefPtr(type, var, funcname, def)								\
	public: LX_INLINE type* Get##funcname() const { return var; }			\
	public: LX_INLINE void Set##funcname(type* v) { var = v; }				\
	protected: type* var = def;

typedef map<LXString, LXProperty*> TMapStringProperty; 
typedef map<LXString, std::function<void(LXSmartObject*)>> TMapFunctions;
typedef map<LXSmartObject*, TMapFunctions> TMapFunctionListeners;

struct TSaveContext
{
	FILE* pXMLFile;
	mutable int Indent = 0;
	bool bSaveChilds;
	bool bSaveSystem;
	LXSmartObject* Owner;
};

struct TLoadContext
{
	TLoadContext(const LXMSXMLNode& xmlNode) :node(xmlNode){ }
	const LXMSXMLNode& node;
	LXFilepath filepath;
	LXSmartObject* pOwner;
};

LXPropertyID GetAutomaticPropertyID();

class LXCORE_API LXSmartObject : public virtual LXObject
{

public:

	LXSmartObject();
	virtual ~LXSmartObject(void);

	// LXSmartObject pointer can be used as Property so as Variant.
	LXSmartObject(const LXSmartObject& Object);
	LXSmartObject& operator=(const LXSmartObject& Object);
	bool operator== (const LXSmartObject& smartObject) const;

	// Recycle Bin Management 
	virtual void 					OnTrashed() {};
	virtual void					OnRecycled() {};

	// Properties
	virtual const ListProperties&	GetProperties() const;
	ListProperties					GetBranchProperties() const;
	virtual void					GetChildProperties(ListProperties& listProperties) const;

	void							GetUserProperties(ListProperties& UserProperties) const;
	void							AttachPropertiesToThis();
	virtual LXProperty*				GetProperty(const LXPropertyID& PID);
	virtual LXProperty*				GetProperty(const LXString& name) const;
	virtual void					OnPropertyChanged(LXProperty* pProperty);

	//
	// Listeners / Callback
	//

	// Register/Unregister object properties changes 
	void							RegisterCB_OnPropertyChanged(void* Listener, std::function<void(LXSmartObject*, LXProperty*)>);
	void							UnregisterCB_OnPropertyChanged(void* Listener);
	
	// Register/Unregister for global properties changes 
	static void						RegisterCB_OnPropertiesChanged(LXObject* Listener, std::function<void(LXSmartObject*, LXProperty*)> func);
	static void						UnregisterCB_OnPropertiesChanged(LXObject* Listener);

	// Generic CallBack mechanism (Deprecated)
	[[deprecated]]
	void							RegisterCB(LXSmartObject* Listerner, const LXString& FunctionName, std::function<void(LXSmartObject*)>);
	[[deprecated]]
	void							UnregisterCB(LXSmartObject* Listener, const LXString& FunctionName);
	[[deprecated]]
	void							InvokeCB(const LXString& functionn);

	//------------------------------------------------------------------------------------------------------
	// 
	// LXPropertyListSmartObjects & LXPropertyArraySmartObjects tools
	// 
	//------------------------------------------------------------------------------------------------------

	// For UI, returns choices/options to determine the object to create as the a list item.
	virtual void					GetNewListItemChoices(const LXPropertyListSmartObjects* property, list<LXString>& outStrings) { }

	// For UI, Create and add a new list item according the previous choice.
	virtual LXSmartObject*			AddItemToPropertyList(const LXPropertyListSmartObjects* property, const LXString& id) { CHK(0); return nullptr; };
	
	//------------------------------------------------------------------------------------------------------
	// Save & Load 
	// pName. By default the methods use the object className as the xml tag, 
	// but in some cases (template class gives name with chevron) it needs to manually set the xml tag.
	// pAttibute. offers the ability to set an attribute to the xml tag object. It's useful when we need to know something 
	// before to create the object (the type of the template class for example).
	//------------------------------------------------------------------------------------------------------
	bool							Save(const TSaveContext& saveContext, LXString* pName = nullptr, LXString* pAttribute = nullptr, bool saveAsProperty = false) const;
	bool         					Load(const TLoadContext& loadContext, LXString* pName = nullptr);
	bool							LoadWithMSXML(const LXFilepath& strFilename, bool bLoadChilds = true, bool bLoadViewStates = true);
	bool							IsLoading() const { return _isLoading; }
	
	void							SetName(const LXString& strName) { _Name = strName; }
	const LXString&					GetName() const { return _Name; }

	void							SetPersistent(bool bPersistent) { _bPersistent = bPersistent; }
	bool							GetPersistent() const { return _bPersistent; }

	// "System" The object is managed by the application. And can't be delete by the user
	void							SetSystem(bool val) { _bSystem = val; }
	bool							IsSystem() const { return _bSystem; }

	// For UI purpose
	void							SetUserData(uint64 nUserData) { _nUserData = nUserData; }
	const uint64&					GetUserData() const { return _nUserData; }

	// Local references
	shared_ptr<LXSmartObject>		GetObject(const LXString& uid);
	LXReference<LXSmartObject>		GetObjectAsRef(const LXString& uid);
	void							AddObject(const LXString& uid, LXSmartObject* smartObject);
	void							RemoveObject(const LXString& uid, LXSmartObject* smartObject);

	// Misc 
	bool							IsNeedSave() const { return _bNeedSave; }
	LXString*						GetUID(bool bBuild = false) const;
		
	template<class T>
	LXPropertyT<T>*					CreateUserProperty(const LXString& Name, const T& DefaultValue);
	template<class T>
	LXPropertyT<T>*					CreateUserProperty(const LXString& Name, LXPropertyID propertyID, const T& DefaultValue);
	template <class T>
	LXPropertyT<T>*					DefineProperty(const LXString& Name, T* var) { return DefineProperty(Name, GetAutomaticPropertyID(), var); }
	template <class T>				
	LXPropertyT<T>*					DefineProperty(const char* name, T* var) { return DefineProperty(name, GetAutomaticPropertyID(), var); }
	template<class T>
	LXPropertyT<T>*					DefineProperty(const char* name, T* var, T min, T max) { return DefineProperty(name, GetAutomaticPropertyID(), var, min, max); }
	template<class T>
	LXPropertyT<T>*					DefineProperty(const LXString& name, const LXPropertyID& PID, T* var);
	template<class T>
	LXPropertyT<T>*					DefineProperty(const LXString& name, const LXPropertyID& PID, T* var, T Min, T Max);
	template<class T>
	LXPropertyListSmartObjects*		DefinePropertyList(const LXString &name, T* var) { return DefineProperty(name, (ListSmartObjects*)var); }

	bool							AddProperty(LXProperty* pProperty);
	bool							RemoveProperty(LXProperty* property);
	
protected:

	virtual void					OnLoaded() {};

	template<class T>
	LXPropertyT<T>*					DefinePropertyEval(const LXString& name, const LXPropertyID& PID, std::function<int()> eval, T def);
	template<class T>
	LXPropertyT<T>*					DefineProperty(const LXString& label, const LXString& name, const LXPropertyID& PID, T* var);
	void							DefinePropertyName();
	LXPropertyString*				DefinePropertyString(const LXString& name, const LXPropertyID& PID, LXString* pString);
	LXPropertyFilepath*				DefinePropertyFilepath(const LXString& name, const LXPropertyID& PID, LXFilepath* pFilename);
	LXPropertyInt*					DefinePropertyInt(const LXString& name, const LXPropertyID& PID, int* pInt);
	LXPropertyUint*					DefinePropertyUint(const LXString& name, const LXPropertyID& PID, uint* pUInt);
	LXPropertyFloat*				DefinePropertyFloat(const LXString& name, const LXPropertyID& PID, float* pFloat);
	LXPropertyFloat*				DefinePropertyFloat(const LXString& name, const LXPropertyID& PID, float* pFloat, float fMin, float fMax);
	LXPropertyVec3f*				DefinePropertyVec3f(const LXString& name, const LXPropertyID& PID, vec3f* pVec3f);
	LXPropertyVec4f*				DefinePropertyVec4f(const LXString& name, const LXPropertyID& PID, vec4f* pVec4f);
	LXPropertyLXColor4f*			DefinePropertyColor4f(const LXString& name, const LXPropertyID& PID, LXColor4f* pLXColor4f);
	LXPropertyMatrix*				DefinePropertyMatrix(const LXString& name, const LXPropertyID& PID, LXMatrix* pMatrix);
	LXPropertyAssetPtr*				DefinePropertyAsset(const LXString& name, const LXPropertyID& PID, shared_ptr<LXAsset>* pAsset);
	LXPropertyBool*					DefinePropertyBool(const LXString& name, const LXPropertyID& PID, bool* pBool);
	LXPropertyBool*					DefinePropertyBool(const LXString& name, const LXString& strID, const LXPropertyID& PID, bool* pBool);
	LXPropertyEnum*					DefinePropertyEnum(const LXString& name, uint* pEnum);
	LXPropertyEnum*					DefinePropertyEnum(const LXString& name, const LXPropertyID& PID, uint* pEnum);
	LXPropertyFloat*				DefinePropertyFloat(const LXString& label, const LXString& name, const LXPropertyID& PID, float* pFloat);
	LXPropertyEnum*					DefinePropertyEnum(const LXString& label, const LXString& name, const LXPropertyID& propID, uint* pEnum);

private:

	virtual bool					OnSaveChild(const TSaveContext& saveContext) const { return true; }
	virtual bool					OnLoadChild(const TLoadContext& loadContext) { return true; }
	
	void							DefineProperties();
	void							DeleteProperty(LXProperty* pProperty);

public:

	LXDelegate<LXProperty*>			PropertyChanged;
	bool							EditableUserProperties = false;


protected:

	LXString						_Name;
	bool							_bPersistent = true;
	bool							_bSystem = false;
	bool							_bNeedSave = false;
	uint64							_nUserData = 0;

	std::atomic<bool>				_isLoading = false;
	bool							_isLoaded = false;
	
private:

	mutable LXString*				_pUID = nullptr;
	ListProperties					_listProperties;		// Engine defined properties
	list<LXVariant*>				_listVariants;			// User defined variables

	//
	// Listeners / Callback
	//

	map<void*, std::function<void(LXSmartObject*, LXProperty*)>> _MapCBOnPropertyChanged;
	static map<LXObject*, std::function<void(LXSmartObject*, LXProperty*)>> _MapCBOnPropertiesChanged;
	TMapFunctionListeners _MapGenericCB;

	//
	// Private local references,
	// used to resolve the links between the child objects (Should be in TLoadContext)
	// More useful than a ReferenceManager, here we can safely duplicate the object regardless of the child UIDs values.
	// ex: Duplicate a material asset file.
	//

	map<LXString, LXSmartObject*> _objects;

};

// For UI Purpose
class LXCORE_API LXSmartObjectContainer : public virtual LXObject
{

public:

	LXSmartObjectContainer();
	virtual ~LXSmartObjectContainer();

	void						AddSmartObject	(LXSmartObject* pSmartObject); 
	const ArraySmartObjects&	GetSmartObjects	() const { return m_arraySmartObject; }
	
protected:

	ArraySmartObjects m_arraySmartObject;

};

