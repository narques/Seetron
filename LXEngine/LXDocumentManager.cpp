//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXCommandManager.h"
#include "LXCore.h"
#include "LXDocumentManager.h"
#include "LXEventManager.h"
#include "LXLogger.h"
#include "LXProject.h"
#include "LXQueryTransform.h"
#include "LXThread.h"
#include "LXWindow.h"
#include "LXMemory.h" // --- Must be the last included ---

LXDocumentManager::LXDocumentManager(void)
{
	GetEventManager()->RegisterEvent(EEventType::ProjectLoaded, [this](LXEvent* Event)
	{
		LXEventResult* EventResult = static_cast<LXEventResult*>(Event);

		if (EventResult->Success)
		{

		}
		else
		{
			GetCore().CloseProject();
		}
	});
}

LXDocumentManager::~LXDocumentManager(void)
{
	LX_SAFE_DELETE(_LoadingThread);
	CHK(_FilesToLoad.size() == 0);
	LX_SAFE_DELETE(_Project);
	LX_SAFE_DELETE(_pWindow);
}

int OpenFunc(void* pData)
{
	LoadingThread = GetCurrentThreadId();

	LXCore& core = GetCore();
	
	LXDocumentManager* pDocumentManager = (LXDocumentManager*)pData;
	LXProject* Project = pDocumentManager->GetDocument();
	Project->SetLoading();

	bool bRet = false;

	while(1)
	{
		StackFilepath& stackFile = pDocumentManager->GetStackFile();
		LXFilepath strFilepath = stackFile.top();
		stackFile.pop();
		bRet = Project->LoadFile(strFilepath);
		if (stackFile.size() == 0)
			break;
	}

	Project->SetLoaded();
	Project->OnFilesLoaded(bRet);
	
	GetEventManager()->PostEvent(new LXEventResult(EEventType::ProjectLoaded, bRet));
					
	LoadingThread = 0;

	return TRUE;
}

LXProject* LXDocumentManager::LoadFile(const LXFilepath& strFilepath, bool bThreaded)
{
	if (!strFilepath.IsFileExist())
	{
		LogW(DocumentManager, L"File not found : " + strFilepath);
		return nullptr;
	}

	CHK(_Project == nullptr);
		
	_Project = new LXProject(strFilepath);
	
	_FilesToLoad.push(strFilepath);
	
	if (bThreaded)
	{
		LX_SAFE_DELETE(_LoadingThread);
		_LoadingThread = new LXThread();
		_LoadingThread->Run(&OpenFunc, this);
	}
	else
	{
		OpenFunc(this);
	}
	
	return _Project;
}

void LXDocumentManager::SetWindow(HWND hWnd)
{ 
	if (!_pWindow)
		_pWindow = new LXWindow(hWnd);
}
