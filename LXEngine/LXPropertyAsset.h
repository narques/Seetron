//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXPropertyTemplate.h"

class LXPropertyAsset : public LXProperty
{

public:

	static LXPropertyAsset* Create(LXSmartObject* owner, const LXString& name, const LXPropertyID& PID, std::shared_ptr<LXAsset>* var);
	static LXPropertyAsset* Create(LXSmartObject* owner, const LXString& name, std::shared_ptr<LXAsset>* var);
	static LXPropertyAsset* CreateUserProperty(LXSmartObject* owner, const LXString& name, const LXAssetPtr& var);

	LXPropertyAsset();

	std::shared_ptr<LXAsset>& GetValue() const { return *_Var; }

	virtual void LoadXML(const TLoadContext& loadContext) override;
	virtual void SaveXML(const TSaveContext& saveContext) override;

	// Deprecated
	virtual LXProperty* Create(const LXString& name, void* var) const override;
	
	virtual void* GetVarPtr() const override { return _Var; }

	virtual LXString GetTypeName() override { return L"LXAssetPtr"; }

	void SetValue(const std::shared_ptr<LXAsset> value, bool invokeOnProperyChanged = true);

private:

	void GetValueFromXML2(const TLoadContext& LoadContext);
	void SaveXML2(const TSaveContext& saveContext, const LXString& strXMLName, const LXAssetPtr& v);

private:

	std::shared_ptr<LXAsset>* _Var = nullptr;
};

typedef LXPropertyAsset LXPropertyAssetPtr;
