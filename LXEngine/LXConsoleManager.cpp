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
#include "LXCore.h"
#include "LXEventManager.h"
#include "LXEvent.h"
#include "LXMemory.h" // --- Must be the last included ---

LXConsoleManager& GetConsoleManager()
{
	static LXConsoleManager* ConsoleManager = new LXConsoleManager();
	return *ConsoleManager;
}

LXConsoleManager::LXConsoleManager()
{
}

LXConsoleManager::~LXConsoleManager()
{
}

void LXConsoleManager::DeleteSingleton()
{
	LXConsoleManager* consoleManager = &GetConsoleManager();
	delete consoleManager;
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

void LXConsoleManager::AddCommand(LXConsoleCommand* command)
{
	for (const LXConsoleCommand* ItConsoleCommand : ListCommands)
	{
		CHK(ItConsoleCommand->Name != command->Name);
	}

	ListCommands.push_back(command); 

	GetEventManager()->PostEvent(new LXEventObjectCreated(EEventType::ConsoleCommandAdded, command));
}

void LXConsoleManager::RemoveCommand(LXConsoleCommand* command)
{
	ListCommands.remove(command);
	GetEventManager()->PostEvent(new LXEventObjectDeleted(EEventType::ConsoleCommandDeleted, command));
}

//------------------------------------------------------------------------------------------------------
// LXConsoleCommand
//------------------------------------------------------------------------------------------------------

LXConsoleCommand::LXConsoleCommand(const LXString& name):Name(name)
{
	GetConsoleManager().AddCommand(this);
}

LXConsoleCommand::~LXConsoleCommand()
{
}

//------------------------------------------------------------------------------------------------------
// LXConsoleCommandT
//------------------------------------------------------------------------------------------------------

template<typename T>
LXConsoleCommandT<T>::LXConsoleCommandT(const LXString& InName, T* inVar) :LXConsoleCommand(InName), Var(inVar)
{
	
}

template<typename T>
LXConsoleCommandT<T>::LXConsoleCommandT(const LXString& file, const LXString& section, const LXString& name, const LXString& defaultValue) :LXConsoleCommand(name)
{ 
	_privateProfile = new LXPrivateProfile();
	_privateProfile->File = file;
	_privateProfile->Section = section;
	_privateProfile->Name = name;
	_privateProfile->DefaultValue = defaultValue;
}

template<typename T>
LXConsoleCommandT<T>::~LXConsoleCommandT()
{
	if (_privateProfile)
	{
		delete _privateProfile;
		delete Var;
	}
}

template<typename T>
void LXConsoleCommandT<T>::ReadValueFromPrivateProfile()
{
	CHK(_privateProfile);
	wchar_t str[256] = { 0 };
	LXString Filepath = GetSettings().GetCoreFolder() + _privateProfile->File;
	CHK(GetPrivateProfileString(_privateProfile->Section, _privateProfile->Name, _privateProfile->DefaultValue, str, 256, Filepath));
	SetValueFromString(str);
}

template<typename T>
const T& LXConsoleCommandT<T>::GetValue()
{
	if (!Var && _privateProfile)
	{
		Var = new T();
		ReadValueFromPrivateProfile();
	}
	
	return *Var;
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
// LXConsoleCommandCall2
//------------------------------------------------------------------------------------------------------

template<typename FunctionSignature>
LXConsoleCommandCall2<FunctionSignature>::LXConsoleCommandCall2(const LXString& InName) :LXConsoleCommand(InName)
{
}

template<typename FunctionSignature>
LXConsoleCommandCall2<FunctionSignature>::LXConsoleCommandCall2(const LXString& InName, std::function<FunctionSignature> InFunction) :LXConsoleCommand(InName), Function(InFunction)
{
}

template<typename FunctionSignature>
LXConsoleCommandCall2<FunctionSignature>::LXConsoleCommandCall2(const LXString& InName, std::function<FunctionSignature> InFunction, std::function<bool()> InOnCanExecute):
	LXConsoleCommand(InName), 
	Function(InFunction), 
	OnCanExecute(InOnCanExecute)
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

template<typename FunctionSignature>
bool LXConsoleCommandCall2<FunctionSignature>::CanExecute()
{
	if (OnCanExecute)
		return OnCanExecute();
	else
		return true;
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


