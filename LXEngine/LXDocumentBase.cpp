//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXDocumentBase.h"
#include "LXFile.h"
#include "LXFile.h"
#include "LXSettings.h"
#include "LXLogger.h"

LXDocumentBase::LXDocumentBase()
{
}

LXDocumentBase::LXDocumentBase(const LXDocumentBase& documentBase) :
m_strFilepath(documentBase.m_strFilepath),
m_bManageDefaultProperties(documentBase.m_bManageDefaultProperties),
m_bManageFile(documentBase.m_bManageFile)
{

}

LXDocumentBase::~LXDocumentBase()
{
}

bool LXDocumentBase::SaveDefaultProperties()
{
	CHK(m_bManageDefaultProperties);
	if (!m_bManageDefaultProperties)
		return false;

	LXFilepath strFilepath = GetObjectName() + L"." + LX_DEFAULT_EXT;
	return SaveFile(GetSettings().GetEngineFolder() + strFilepath, false);
}

bool LXDocumentBase::LoadDefaultProperties()
{
	CHK(m_bManageDefaultProperties);
	if (!m_bManageDefaultProperties)
		return false;

	LXFilepath strFilepath = GetObjectName() + L"." + LX_DEFAULT_EXT;

	return Load(GetSettings().GetEngineFolder() + strFilepath, false);
}

bool LXDocumentBase::SaveFile(bool bSaveSystem /*=false*/)
{
	CHK(!m_strFilepath.IsEmpty());
	if (m_strFilepath.IsEmpty())
		return false;

	return SaveFile(m_strFilepath, true, bSaveSystem);
}

bool LXDocumentBase::SaveFile(const LXFilepath& strFilename, bool bSaveChilds /*=true*/, bool bSaveSystem /*=false*/)
{
	CHK(m_bManageFile || m_bManageDefaultProperties);
	if (!m_bManageFile && !m_bManageDefaultProperties)
		return false;

	TSaveContext saveContext;
	saveContext.bSaveChilds = bSaveChilds;
	saveContext.bSaveSystem = bSaveSystem;

	// XML file

	FILE* pFile = NULL;
	errno_t err = _wfopen_s(&pFile, strFilename, L"wt");
	CHK(pFile && !err);
	if (!pFile || err)
		return false;
	fwprintf(pFile, L"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
	saveContext.pXMLFile = pFile;

	Save(saveContext);

	fclose(pFile);

	_bNeedSave = false;

	LogI(Project, L"Sucessfulled saved project %s", strFilename.GetBuffer());
	
	return true;
}

bool LXDocumentBase::Load(const LXFilepath& strFilename, bool bLoadChilds /* = true */, bool bLoadViewStates/* = true*/)
{
	m_strFilepath = strFilename;

	if (!strFilename.IsFileExist())
		return false;

	return LoadWithMSXML(strFilename, bLoadChilds, bLoadViewStates);
}