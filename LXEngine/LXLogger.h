//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXCore/LXObject.h"

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
	LogMode_EngineConsole = LX_BIT(0),
	LogMode_OSConsole = LX_BIT(1),
	LogMode_DebuggerConsole = LX_BIT(2),
	LogMode_File = LX_BIT(3),
};

class LXENGINE_API LXLogger : public LXObject
{

public:

	LXLogger();
	virtual ~LXLogger();

	static void DeleteSingleton();

	void Tick();
	void SetMode(ELogMode InLogModes);
	ELogMode GetMode() const { return LogModes; }
	void LogConfigurationAndPlatform();
	void LogDateAndTime();
	void Register(void* Owner, std::function<void(ELogType, const wchar_t*)>);
	void Unregister(void* Owner);
	void CreateOSConsole();
	void PrintToConsoles(ELogType LogType, const LXString& msg);

private:

	std::map<void*, std::function<void(ELogType, const wchar_t*)>> MapCallbacks;
	ELogMode LogModes;
	LXMutex* Mutex;
	LXFile* _file = nullptr;
	std::list<LXString> _logs;

};

LXENGINE_API LXLogger& GetLogger();

LXENGINE_API void Log2(ELogType LogType, const wchar_t* section, const wchar_t* Format, ...);
LXENGINE_API void Log2(ELogType LogType, const wchar_t* section, const char* Format, ...);
LXENGINE_API void Output(ELogType LogType, const wchar_t* section, const wchar_t* Format, ...);

#define LogI(section, message, ...) Log2(ELogType::LogType_Info, L#section, message, __VA_ARGS__);
#define LogW(section, message, ...) Log2(ELogType::LogType_Warning, L#section, message, __VA_ARGS__);
#define LogE(section, message, ...) Log2(ELogType::LogType_Error, L#section, message, __VA_ARGS__);
#define LogD(section, message, ...) Log2(ELogType::LogType_Debug, L#section, message, __VA_ARGS__);
#define Trace(section, message, ...) Output(ELogType::LogType_Debug, L#section, message, __VA_ARGS__);