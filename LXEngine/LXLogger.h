//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include <functional>
#include <map>

class LXMutex;
class LXFile;

enum class ELogType
{
	LogType_Info,
	LogType_Warning,
	LogType_Error,
	LogType_Debug
};

enum ELogMode
{
	LogMode_CoreConsole = LX_BIT(0),
	LogMode_OSConsole = LX_BIT(1),
	LogMode_DebuggerConsole = LX_BIT(2),
	LogMode_File = LX_BIT(3),
};

class LXCORE_API LXLogger : public LXObject
{

public:

	LXLogger();
	virtual ~LXLogger();

	static void DeleteSingleton();

	void SetMode(ELogMode InLogModes);
	ELogMode GetMode() const { return LogModes; }
	void LogConfigurationAndPlatform();
	void LogDateAndTime();
	void Register(void* Owner, std::function<void(ELogType, const wchar_t*)>);
	void Unregister(void* Owner);
	void CreateOSConsole();
	void PrintToConsoles(ELogType LogType, const LXString& msg);

private:

	map<void*, std::function<void(ELogType, const wchar_t*)>> MapCallbacks;
	ELogMode LogModes;
	LXMutex* Mutex;
	LXFile* File;

};

LXCORE_API LXLogger& GetLogger();

LXCORE_API void Log2(ELogType LogType, const wchar_t* section, const wchar_t* Format, ...);
LXCORE_API void Log2(ELogType LogType, const wchar_t* section, const char* Format, ...);
LXCORE_API void Output(ELogType LogType, const wchar_t* section, const wchar_t* Format, ...);



