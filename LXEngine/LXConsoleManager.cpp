//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXConsoleManager.h"
#include "LXSettings.h"
#include "LXLogger.h"
#include "LXMemory.h" // --- Must be the last included ---

LXConsoleManager& GetConsoleManager()
{
	static LXConsoleManager ConsoleManager;
	return ConsoleManager;
}

LXConsoleManager::LXConsoleManager()
{
}

LXConsoleManager::~LXConsoleManager()
{
}

bool LXConsoleManager::TryToExecute(const LXString& CommandLine)
{
	LogI(ConsoleManager, L">%s", CommandLine.GetBuffer());
	vector<LXString> ArrayStrings;
	CommandLine.Split(ArrayStrings);
	for (auto Command : ListCommands)
	{
		if (Command->Name.CompareNoCase(ArrayStrings[0]) == 0)
		{
			Command->Execute(ArrayStrings);
			return true;
		}
	}

	LogI(ConsoleManager, L"'%s' is not a recognized command", CommandLine.GetBuffer());
	return false;
}

void LXConsoleManager::GetNearestCommand(const LXString& Str, vector<LXString>& ListSuggestion)
{
	ListSuggestion.clear();
	
	if (!Str.IsEmpty())
	{
		for (auto Command : ListCommands)
		{
			if (Command->Name.Find(Str) == 0)
			{
				ListSuggestion.push_back(Command->Name);
			}
		}
	}
}

void LXConsoleManager::AddCommand(LXConsoleCommand* Command)
{
	for (LXConsoleCommand* ItConsoleCommand : ListCommands)
	{
		CHK(ItConsoleCommand->Name != Command->Name);
	}

	ListCommands.push_back(Command); 
}

//------------------------------------------------------------------------------------------------------
// LXConsoleCommandT
//------------------------------------------------------------------------------------------------------

template<typename T>
LXConsoleCommandT<T>::LXConsoleCommandT(const LXString& InName, T* inVar) :LXConsoleCommand(InName), Var(inVar)
{
	
}

template<typename T>
LXConsoleCommandT<T>::LXConsoleCommandT(const LXString& File, const LXString& Section, const LXString& InName, const LXString& Default, T* inVar) :LXConsoleCommand(InName), Var(inVar)
{ 
	wchar_t str[256] = { 0 };
	LXString Filepath = GetSettings().GetCoreFolder() + File;
	GetPrivateProfileString(Section, InName, Default, str, 256, Filepath);
	SetValueFromString(str);
}

template<>
void LXConsoleCommandT<bool>::SetValueFromString(const LXString& Value)
{
	bool val;
	if (Value.CompareNoCase(L"true") == 0 || Value.CompareNoCase(L"1") == 0)
	{
		val = true;
	}
	else if (Value.CompareNoCase(L"false") == 0 || Value.CompareNoCase(L"0") == 0)
	{
		val = false;
	}
	else
	{
		CHK(0 && "Unknown argument value");
		return;
	}
	*Var = val;
}

// Specialization
template<>
void LXConsoleCommandT<bool>::Execute(const vector<LXString>& Arguments)
{
	*Var = !*Var;
}

// Explicit class Instantiation
template class LXCORE_API LXConsoleCommandT<bool>;

//------------------------------------------------------------------------------------------------------
// LXConsoleCommand2T
//------------------------------------------------------------------------------------------------------

template<typename T>
void LXConsoleCommand2T<T>::Execute(const vector<LXString>& Arguments)
{
}

template<typename T>
LXConsoleCommand2T<T>::LXConsoleCommand2T(const LXString& InName, const T& DefaultValue):LXConsoleCommand(InName), Var(DefaultValue)
{
}

//------------------------------------------------------------------------------------------------------
// LXConsoleCommandCall2
//------------------------------------------------------------------------------------------------------

template<typename FunctionSignature>
LXConsoleCommandCall2<FunctionSignature>::LXConsoleCommandCall2(const LXString& InName, std::function<FunctionSignature> InFunction) :LXConsoleCommand(InName), Function(InFunction)
{
}

// Tools
const LXString& GetArg(const vector<LXString>& Arguments, int Index)
{
	static LXString EmptyString;
	if (Index < Arguments.size())
		return Arguments[Index];
	else
		return EmptyString;
}

// Specializations
template<>
void LXConsoleCommandCall2<void()>::Execute(const vector<LXString>& Arguments)
{
	Function();
}

template<>
void LXConsoleCommandCall2<void(const LXString&)>::Execute(const vector<LXString>& Arguments)
{
	Function(GetArg(Arguments, 1));
}

template<>
void LXConsoleCommandCall2<void(const LXString&, const LXString&)>::Execute(const vector<LXString>& Arguments)
{
	Function(GetArg(Arguments,1), GetArg(Arguments,2));
}

template<>
void LXConsoleCommandCall2<void(const LXString&, const LXString&, const LXString&)>::Execute(const vector<LXString>& Arguments)
{
	Function(GetArg(Arguments, 1), GetArg(Arguments, 2), GetArg(Arguments, 3));
}

// Explicit class Instantiation
template class LXCORE_API LXConsoleCommandCall2<void()>;
template class LXCORE_API LXConsoleCommandCall2<void(const LXString&)>;
template class LXCORE_API LXConsoleCommandCall2<void(const LXString&, const LXString&)>;
template class LXCORE_API LXConsoleCommandCall2<void(const LXString&, const LXString&, const LXString&)>;