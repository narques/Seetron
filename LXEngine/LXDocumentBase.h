//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXSmartObject.h"

class LXCORE_API LXDocumentBase : public virtual LXSmartObject
{

public:

	LXDocumentBase();
	LXDocumentBase(const LXDocumentBase& documentBase);
	virtual ~LXDocumentBase();

public:

	bool			SaveDefaultProperties();
	bool			SaveFile(bool bSaveSystem = false);
protected:

	bool			SaveFile(const LXFilepath& strFilename, bool bSaveChilds = true, bool bSaveSystem = false);

	bool			LoadDefaultProperties();
	bool			Load(const LXFilepath& strFilename, bool bLoadChilds = true, bool bLoadViewStates = true);

	void			SetManageDefaultProperties(bool bManageDefaultProperties) { m_bManageDefaultProperties = bManageDefaultProperties; }
public:
	bool			GetManageDefaultProperties() const { return m_bManageDefaultProperties; }
protected:
	
	void			SetManageFile(bool bManageFile) { m_bManageFile = bManageFile; }
	
protected:

	GetSet(LXFilepath, m_strFilepath, Filepath);

private:

	bool			m_bManageDefaultProperties=false;
	bool			m_bManageFile=false;		

};

