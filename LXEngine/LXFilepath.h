//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXString.h"

class LXCORE_API LXFilepath : public LXString
{

public:

	LXFilepath( ) { }
	LXFilepath( const wchar_t* str ):LXString(str){ ReplaceAll(L"\\", L"/"); }
	LXFilepath( const LXString& str ):LXString(str) { ReplaceAll(L"\\", L"/"); }

	LXString	GetExtension		( ) const;
	LXString	GetFilename			( ) const;
	LXString	GetFilepath			( ) const;
	LXString	GetFilenameNoExt	( ) const;
	bool		IsFileExist			( ) const;
	bool		IsFolderExist		( ) const;
	bool		IsFolderEmpty		( ) const;
	bool		IsAbsolute			( ) const;
	bool		ChangeExtensionTo	( const LXString& extention );
	void		RemoveExtension		( );
	void		RemoveLastSlash		( );
	static LXString GetFontDirectory();
	LXFilepath GetRelativeFilepath(const LXFilepath& AbsoluteFilepath) const;

	
};
