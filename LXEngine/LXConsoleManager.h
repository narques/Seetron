//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXCore/LXObject.h"

class LXConsoleCommand;

class LXENGINE_API LXConsoleManager : public LXObject
{
public:

	LXConsoleManager();
	virtual ~LXConsoleManager();

	static void DeleteSingleton();

	bool TryToExecute(const LXString& CommandLine);
	void GetNearestCommand(const LXString& Str, std::vector<LXString>& ListSuggestion);
	
	void AddCommand(LXConsoleCommand* command);
	void RemoveCommand(LXConsoleCommand* command);

	std::list<LXConsoleCommand*> ListCommands;
};

LXENGINE_API LXConsoleManager& GetConsoleManager();

//------------------------------------------------------------------------------------------------------
// ConsoleCommand base class
//------------------------------------------------------------------------------------------------------

class LXENGINE_API LXConsoleCommand : public LXObject
{

public:

	LXConsoleCommand(const LXString& name);
	virtual ~LXConsoleCommand();

	virtual void Execute(const std::vector<LXString>& Arguments) = 0;
	virtual bool CanExecute() { return true; }

public:
	
	LXString Name;
	uint MenuID = -1;
};

//------------------------------------------------------------------------------------------------------
// ConsoleCommand based on a existing variable 
//------------------------------------------------------------------------------------------------------

template<typename T>
class LXENGINE_API LXConsoleCommandT : public LXConsoleCommand
{
public:

	LXConsoleCommandT(const LXString& InName, T* inVar);
	LXConsoleCommandT(const LXString& InFile, const LXString& InSection, const LXString& InName, const LXString& InDefault);
	virtual ~LXConsoleCommandT();

	const T& GetValue();
	void Execute(const std::vector<LXString>& Arguments) override;

private:

	void SetValue(const T& value) { *Var = value; }
	void SetValueFromString(const LXString& Value);
	void ReadValueFromPrivateProfile();

private:

	T* Var = nullptr;

	struct LXPrivateProfile
	{
		LXString File;
		LXString Section;
		LXString Name;
		LXString DefaultValue;

	}* _privateProfile = nullptr;
	
};

//------------------------------------------------------------------------------------------------------
// ConsoleCommand based on method V2.0
//------------------------------------------------------------------------------------------------------

template<typename FunctionSignature>
class LXENGINE_API LXConsoleCommandCall2 : public LXConsoleCommand
{

public:

	LXConsoleCommandCall2(const LXString& InName);
	LXConsoleCommandCall2(const LXString& InName, std::function<FunctionSignature> InFunction);
	LXConsoleCommandCall2(const LXString& InName, std::function<FunctionSignature> InFunction, std::function<bool()> InOnCanExecute);

	void Execute(const std::vector<LXString>& Arguments) override;
	bool CanExecute() override;

	std::function<FunctionSignature> Function;
	std::function<bool()> OnCanExecute;
};

typedef LXConsoleCommandCall2<void()> LXConsoleCommandNoArg;
typedef LXConsoleCommandCall2<void(const LXString&)> LXConsoleCommand1S;
typedef LXConsoleCommandCall2<void(const LXString&, const LXString&)> LXConsoleCommand2S;
typedef LXConsoleCommandCall2<void(const LXString&, const LXString&, const LXString&)> LXConsoleCommand3S;
