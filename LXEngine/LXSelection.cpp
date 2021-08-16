//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXSelection.h"
#include "LXProperty.h"
#include "LXCore.h"
#include "LXProject.h"
#include "LXMSXMLNode.h"

LXSelection::LXSelection()
{
	DefineProperties();
}

LXSelection::~LXSelection(void)
{
}

void LXSelection::DefineProperties()
{
	// Description
	DefinePropertyString(L"Description", LXPropertyID::DESCRIPTION, &_strDescription);

	// Size
// 	LXPropertyUint* pPropSize = new LXPropertyUint();
// 	pPropSize->SetName(L"Elements(s)");
// 	pPropSize->SetXMLName(L"Selection.Size");
// 	pPropSize->SetLambdaOnGet([this]() { return m_setSmartObjects.size(); });
// 	pPropSize->SetPersistent(false);
// 	pPropSize->SetReadOnly(true);
// 	pPropSize->SetDefault(0);
// 	AddProperty(pPropSize);
}

const SetSmartObjects& LXSelection::Get( )  
{ 
	if (m_listUID.size())
	{
		CHK(m_setSmartObjects.size()==0);
		GetCore().GetProject()->GetGroups(m_listUID, m_setSmartObjects);
		m_listUID.clear();
	}
	
	CHK(m_setSmartObjects.size());
	return m_setSmartObjects; 
}

/*virtual*/
bool LXSelection::OnSaveChild( const TSaveContext& saveContext  ) const 
{ 
	CHK(0); // TODO ASCII to UNICODE !
	fprintf(saveContext.pXMLFile,"<XMLProp>");
	for(auto It = m_setSmartObjects.begin(); It!=m_setSmartObjects.end(); It++)
	{
		LXString* pUID = (*It)->GetUID(true);
		CHK(pUID);
		if (pUID)
		{
			//LXStringA str = pUID->toString().toUtf8();
			//fprintf(saveContext.pXMLFile, "<REF Value=\"%s\"/>", str.GetBuffer());
			fwprintf(saveContext.pXMLFile, L"<REF Value=\"%s\"/>", pUID->GetBuffer());
		}
	}
	fprintf(saveContext.pXMLFile,"</XMLProp>");
	return true; 
}

/*virtual*/
bool LXSelection::OnLoadChild( const TLoadContext& loadContext )
{ 
	if (loadContext.node.name() == L"XMLProp")
	{

		for (LXMSXMLNode e = loadContext.node.begin(); e != loadContext.node.end(); e++)
		{
			if (loadContext.node.name() == L"REF")
			{
				LXString str = loadContext.node.attr(L"Value");
				m_listUID.push_back(str);
			}
			else
				CHK(0);
		}

		/*
		loadContext.node.m_pQXmlStreamReader->readNext();

		while(!(loadContext.node.m_pQXmlStreamReader->tokenType() == QXmlStreamReader::EndElement && loadContext.node.m_pQXmlStreamReader->name() == "XMLProp"))
		{
			if (loadContext.node.m_pQXmlStreamReader->isStartElement())
			{
				if (loadContext.node.name() == L"REF")
				{
					LXString str = loadContext.node.attr("Value");
					//QUuid uid(QString::fromUtf16(str.GetBuffer()));
					//m_listUID.push_back(uid);
					m_listUID.push_back(str);
				}
				else
					CHK(0);
			}
			loadContext.node.m_pQXmlStreamReader->readNext();
		}
		*/
	}
	else
	{
		auto str = loadContext.node.name();
		CHK(0);
	}
	return true;
}