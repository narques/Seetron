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
	
	// Recycle Bin Management 
	virtual void 					OnTrashed(){};
	virtual void					OnRecycled(){};

	// UI Helper: Child objects
	virtual void					GetChildren(ListSmartObjects&) {}

	// Properties
	virtual const ListProperties&	GetProperties();
	virtual void					GetChildProperties(ListProperties& UserProperties){};
	void							GetUserProperties(ListProperties& UserProperties);
	void							AttachPropertiesToThis();
	LXProperty*						GetProperty(const LXPropertyID& PID);
	LXProperty*						GetProperty(const LXString& name);
	static int						GetPropertyPosition(const LXPropertyID& id);
	virtual void					OnPropertyChanged(LXProperty* pProperty);

	// Listeners / Callback
	void							RegisterCB_OnPropertyChanged(void* Listener, std::function<void(LXSmartObject*, LXProperty*)>);
	void							UnregisterCB_OnPropertyChanged(void* Listener);
	static void						RegisterCB_OnPropertiesChanged(LXObject* Listener, std::function<void(LXSmartObject*, LXProperty*)> func);
	static void						UnregisterCB_OnPropertiesChanged(LXObject* Listener);

	// Generic CallBack mechanism (Deprecated)
	void							RegisterCB(LXSmartObject* Listerner, const LXString& FunctionName, std::function<void(LXSmartObject*)>);
	void							UnregisterCB(LXSmartObject* Listener, const LXString& FunctionName);
	void							InvokeCB(const LXString& functionn);

	//------------------------------------------------------------------------------------------------------
	// Save & Load 
	// pName. By default the methods use the object className as the xml tag, 
	// but in some cases (template class gives name with chevron) it needs to manually set the xml tag.
	// pAttibute. offers the ability to set an attribute to the xml tag object. It's useful when we need to know something 
	// before to create the object (the type of the template class for example).
	//------------------------------------------------------------------------------------------------------
	bool							Save(const TSaveContext& saveContext, LXString* pName = nullptr, LXString* pAttribute = nullptr) const;
	bool         					Load(const TLoadContext& loadContext, LXString* pName = nullptr);
	bool							LoadWithMSXML(const LXFilepath& strFilename, bool bLoadChilds = true, bool bLoadViewStates = true);
			
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

	bool							IsNeedSave() const { return _bNeedSave; }

	// Misc 
	LXString*						GetUID(bool bBuild = false);
	
	template<class T>
	LXPropertyT<T>*					CreateUserProperty(const LXString& Name, const T& DefaultValue);
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

protected:

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
	LXPropertyAssetPtr*				DefinePropertyAsset(const LXString& name, const LXPropertyID& PID, LXAsset** pAsset);
	LXPropertyBool*					DefinePropertyBool(const LXString& name, const LXPropertyID& PID, bool* pBool);
	LXPropertyBool*					DefinePropertyBool(const LXString& name, const LXString& strID, const LXPropertyID& PID, bool* pBool);
	LXPropertyEnum*					DefinePropertyEnum(const LXString& name, const LXPropertyID& PID, uint* pEnum);
	LXPropertyFloat*				DefinePropertyFloat(const LXString& label, const LXString& name, const LXPropertyID& PID, float* pFloat);
	LXPropertyEnum*					DefinePropertyEnum(const LXString& label, const LXString& name, const LXPropertyID& propID, uint* pEnum);

private:

	virtual bool					OnSaveChild(const TSaveContext& saveContext) const { return true; }
	virtual bool					OnLoadChild(const TLoadContext& loadContext) { return true; }

	void							DefineProperties();
	bool							AddProperty(LXProperty* pProperty);
	void							DeleteProperty(LXProperty* pProperty);

protected:

	LXString						_Name;
	bool							_bPersistent = true;
	bool							_bSystem = false;
	bool							_bNeedSave = false;
	uint64							_nUserData = 0;
	
private:

	LXString*						_pUID = nullptr;
	ListProperties					_listProperties;		// Engine defined properties
	list<LXVariant*>				_listVariants;			// User defined variables

	//
	// Listeners / Callback
	//

	map<void*, std::function<void(LXSmartObject*, LXProperty*)>> _MapCBOnPropertyChanged;
	static map<LXObject*, std::function<void(LXSmartObject*, LXProperty*)>> _MapCBOnPropertiesChanged;
	TMapFunctionListeners _MapGenericCB;

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

