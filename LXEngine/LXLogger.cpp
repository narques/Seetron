//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXLogger.h"
#include "LXCore.h"
#include "LXConsoleManager.h"
#include "LXPlatform.h"
#include "LXFile.h"
#include "LXMutex.h"
#include <sstream>
#include <fstream>
#include <stdarg.h>
#include "LXMemory.h" // --- Must be the last included ---

namespace
{
	LXConsoleCommandT<bool> CSet_UseLogFile(L"Engine.ini", L"Log", L"UseLogFile", L"false");
	const LXString kLogFilename = L"seetron.log";
}

LXLogger& GetLogger()
{
	static LXLogger* logger = new LXLogger();
	return *logger;
}

void LXLogger::PrintToConsoles(ELogType LogType, const LXString& msg)
{
	Mutex->Lock();

	SYSTEMTIME st;
	::GetLocalTime(&st);
	LXString logEntry = L"[";
	logEntry += LXString::Format(L"%02d", st.wHour);
	logEntry += L":" + LXString::Format(L"%02d", st.wMinute);
	logEntry += L":" + LXString::Format(L"%02d", st.wSecond);
	logEntry += L"." + LXString::Format(L"%03d", st.wMilliseconds) + L"]";
	logEntry += L"[" + LXString::Number(GetCore().Frame) + L"] ";
	logEntry += msg;
	
	_logs.push_back(logEntry);

	if ((LogModes & ELogMode::LogMode_DebuggerConsole) && IsDebuggerPresent())
	{
		LXString msg2 = msg + L"\n";
		OutputDebugString(msg2.GetBuffer());
	}

	Mutex->Unlock();
}

void LXLogger::Tick()
{
	Mutex->Lock();

	if (_logs.size() > 0)
	{
		LXPerformance perf;
		const double logBudget = 2.;
		double time = 0.;

		bool logToFile = _file && _file->Open(LXCore::GetAppPath() + L"/" + kLogFilename, L"a");

		while (_logs.size() && time < logBudget)
		{
#if LX_LOG_OUTPUT_FRAME
			LXString logEntry = L"[" + LXString::Number(GetCore().Frame) + L"]";
			logEntry += _logs.front();
#else
			const LXString& logEntry = _logs.front();
#endif

			//if (LogModes & ELogMode::LogMode_OSConsole)
			wcout << logEntry.GetBuffer() << endl;

			if (LogModes & ELogMode::LogMode_CoreConsole)
			{
				for (auto It : MapCallbacks)
					It.second(/*LogType*/ELogType::LogType_Info, logEntry);
			}

			// File
			if (logToFile)
			{
				LXStringA msgA = logEntry.ToStringA();
				msgA += "\n";
				_file->Write(msgA.GetBuffer(), msgA.size(), true);
			
			}

			_logs.pop_front();
			time = perf.GetTime();
		}

		if (logToFile)
			_file->Close();

	}

	Mutex->Unlock();
}


void SetWhite()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hConsole)
		SetConsoleTextAttribute(hConsole, 7);
}

void SetYellow()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hConsole)
		SetConsoleTextAttribute(hConsole, 14);
}

void SetRed()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hConsole)
		SetConsoleTextAttribute(hConsole, 12);
}

void SetGray()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hConsole)
		SetConsoleTextAttribute(hConsole, 8);
}

void Log2(ELogType LogType, const wchar_t* section, const wchar_t* Format, ...)
{
	va_list arguments;
	va_start(arguments, Format);
	wchar_t out[LX_MAX_NUMBER_OF_CHAR];
	vswprintf(out, LX_MAX_NUMBER_OF_CHAR - 1, Format, arguments);
	va_end(arguments);

	if (GetLogger().GetMode() & LogMode_OSConsole)
	{
		switch (LogType)
		{
		case ELogType::LogType_Info: SetWhite(); break;
		case ELogType::LogType_Warning: SetYellow(); break;
		case ELogType::LogType_Error: SetRed(); break;
		default: CHK(0); break;
		}
	}

	LXString msg = LXString(section) + LXString(L": ") + LXString(out);
	GetLogger().PrintToConsoles(LogType, msg);

	if (GetLogger().GetMode() & LogMode_OSConsole)
	SetWhite();
}

LXCORE_API void Log2(ELogType LogType, const wchar_t* section, const char* Format, ...)
{
	va_list arguments;
	va_start(arguments, Format);
	char out[1024];
	vsprintf(out, Format, arguments);
	va_end(arguments);

	if (GetLogger().GetMode() & LogMode_OSConsole)
	{
		switch (LogType)
		{
		case ELogType::LogType_Info: SetWhite(); break;
		case ELogType::LogType_Warning: SetYellow(); break;
		case ELogType::LogType_Error: SetRed(); break;
		default: CHK(0); break;
		}
	}

	LXString msg = LXString(section) + LXString(L": ") + LXString(out);
	GetLogger().PrintToConsoles(LogType, msg);

	if (GetLogger().GetMode() & LogMode_OSConsole)
		SetWhite();
}

void Output(ELogType LogType, const wchar_t* section, const wchar_t* Format, ...)
{
	va_list arguments;
	va_start(arguments, Format);
	wchar_t out[LX_MAX_NUMBER_OF_CHAR];
	vswprintf(out, LX_MAX_NUMBER_OF_CHAR - 1, Format, arguments);
	va_end(arguments);

	LXString msg = LXString(section) + LXString(L": ") + LXString(out);

	SYSTEMTIME st;
	::GetLocalTime(&st);
	LXString logEntry = L"[";
	logEntry += LXString::Format(L"%02d", st.wHour);
	logEntry += L":" + LXString::Format(L"%02d", st.wMinute);;
	logEntry += L":" + LXString::Format(L"%02d", st.wSecond);
	logEntry += L"." + LXString::Format(L"%03d", st.wMilliseconds) + L"]" + msg;

	wcout << logEntry.GetBuffer() << endl;

	if (IsDebuggerPresent())
	{
		LXString msg2 = msg + L"\n";
		OutputDebugString(msg2.GetBuffer());
	}
}

LXLogger::LXLogger()
{
	Mutex = new LXMutex();

	if (CSet_UseLogFile.GetValue() == true)
	{
		_file = new LXFile();
		// Clear the existing file.
		_file->Open(LXCore::GetAppPath() + L"/" + kLogFilename, L"wt");
		_file->Close();
	}

	LogModes = (ELogMode)(LogMode_DebuggerConsole | LogMode_CoreConsole);
}

LXLogger::~LXLogger()
{
	LX_SAFE_DELETE(_file);
	CHK(MapCallbacks.size() == 0);
	delete Mutex;
}

void LXLogger::DeleteSingleton()
{
	LXLogger* logger = &GetLogger();
	delete logger;
}

void LXLogger::SetMode(ELogMode InLogModes)
{
	LogModes = InLogModes;
	if (LogModes & LogMode_File)
	{

	}
}

void LXLogger::Register(void* Owner, std::function<void(ELogType,  const wchar_t*)> func)
{
	MapCallbacks[Owner] = func;
}

void LXLogger::Unregister(void* Owner)
{
	MapCallbacks.erase(Owner);
}

void LXLogger::CreateOSConsole()
{
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);
}

void LXLogger::LogConfigurationAndPlatform()
{
 #ifdef _WIN64
 #ifdef _DEBUG
	LogI(Core, L"x64 Debug");
 #else
	LogI(Core, L"x64 Release");
#endif
#else
#ifdef _DEBUG
	LogI(Core, L"win32 Debug");
#else
	LogI(Core, L"win32 Release");
#endif
#endif
}

void LXLogger::LogDateAndTime()
{
	LXString str = LXPlatform::GetCurrentDate() + L" " + LXPlatform::GetCurTime();
	LogI(Core, L"%s", str.GetBuffer());
}
