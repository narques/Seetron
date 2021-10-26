//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXFilepath.h"
#include <shlwapi.h>

LXString LXFilepath::GetExtension() const
{
	int nPoint = ReverseFind('.');
	int nSizeExt = GetLength() - nPoint - 1;
	return Right(nSizeExt).MakeLower();
}

LXString LXFilepath::GetFilename() const
{
	int nPos = ReverseFind('\\');
	if (nPos == -1)
		nPos = ReverseFind('/');
	return Right(GetLength() - nPos - 1);
}

LXString LXFilepath::GetFilenameNoExt() const
{
	int nPos = ReverseFind('\\');
	if (nPos == -1)
		nPos = ReverseFind('/');
	LXString str = Right(GetLength() - nPos - 1);
	nPos = str.ReverseFind('.');
	str = str.Left(nPos);
	return str;
}

bool LXFilepath::IsFileExist() const
{
	FILE *f = NULL;
	errno_t err = _wfopen_s(&f, *this,L"rb");
	if (err!=0)
		return false;
	if (f) 
	{
		fclose(f);
		return true;
	}
	else
		return false;
}

bool LXFilepath::IsFolderExist() const
{
	DWORD dwAttrib = GetFileAttributes(m_str.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}


bool LXFilepath::IsFolderEmpty() const
{
	return PathIsDirectoryEmpty(m_str.c_str()) == TRUE;
}

bool LXFilepath::IsAbsolute() const
{
	LXString str = GetFilepath();
	return !str.IsEmpty();
}

LXString LXFilepath::GetFilepath( ) const
{
	int nPos = ReverseFind('\\');
	if (nPos == -1)
		nPos = ReverseFind('/');
	return Left(nPos+1);
}

bool LXFilepath::ChangeExtensionTo(const LXString& extention)
{
	int nPoint = ReverseFind('.');
	int nSizeExt = GetLength() - nPoint -1;
	LXString strOldExtension = Right(nSizeExt);
	Replace(strOldExtension, extention);
	return true;
}

void LXFilepath::RemoveExtension()
{
	int nPos = ReverseFind('.');
	if (nPos != -1)
		*this = Left(nPos);
}

void LXFilepath::RemoveLastSlash()
{
	int nPos = ReverseFind('/');
	if (nPos != -1)
		*this = Left(nPos);
}

LXString LXFilepath::GetFontDirectory()
{
	wchar_t buffer[256];
	GetWindowsDirectory((wchar_t*)&buffer, 256);
	return LXFilepath(buffer) + L"\\fonts\\";
}

LXFilepath LXFilepath::GetRelativeFilepath(const LXFilepath& AbsoluteFilepath) const
{
	// Already Relative
	if (::PathIsRelative(AbsoluteFilepath.GetBuffer()) == TRUE)
	{
		return AbsoluteFilepath;
	}

	if (AbsoluteFilepath.Find(GetBuffer()) == -1)
	{
		return L"";
	}
		
	LXString RelativePath;
	int Lenght = AbsoluteFilepath.GetLength() - GetLength();

	if (Lenght > 0)
	{
		// Check if the given absolute filepath is in the folder (this) else return an empty string
		LXString Toto = AbsoluteFilepath.Left(GetLength());

		if (ToLower() == Toto.ToLower())
		{
			RelativePath = AbsoluteFilepath.Right(Lenght);
			return RelativePath;
		}
	}

	return  AbsoluteFilepath;
}
