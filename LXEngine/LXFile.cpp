//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LXFile.h"
#include <fstream>
#include <string>
#include <fileapi.h>
#include "LXMemory.h" // --- Must be the last included ---

LXFile::LXFile(void):
m_pFile(NULL)
{
}

LXFile::~LXFile(void)
{
	if (m_pFile)
		fclose(m_pFile);
}

bool LXFile::Open( const LXString& strFilename, const LXString& strOptions )
{
	errno_t err = _wfopen_s(&m_pFile, strFilename, strOptions);
	return m_pFile != NULL;
}

void LXFile::Close( )
{
	fclose(m_pFile);
	m_pFile = NULL;
}

size_t LXFile::Read( void* pBuffer, size_t size )
{
	CHK(m_pFile);
	if (!m_pFile)
		return 0;

	return fread(pBuffer, size, 1, m_pFile);
};

int LXFile::ReadInt( )
{
	int i;
	Read(&i, sizeof(int));
	return i;
}

vec3f LXFile::ReadVec3f( )
{
	vec3f v;
	Read(&v, sizeof(vec3f));
	return v;
}

short LXFile::ReadShort( )
{
	short s;
	Read(&s, sizeof(short));
	return s;
}

char* LXFile::GetString( char* str, int n )
{
	return fgets(str, n, m_pFile);
}

bool LXFile::Write( void* pBuffer, size_t size , bool Flush)
{
	CHK(m_pFile);
	if (!m_pFile)
		return 0;

	bool r = fwrite(pBuffer, size, 1, m_pFile) > 0;

	if (r && Flush)
		fflush(m_pFile);

	return true;
}

void LXFile::Seek( int nSeekMethod )
{
	fseek(m_pFile, 0, nSeekMethod);
}

void LXFile::Rewind()
{
	rewind(m_pFile);
}

ULONG LXFile::Tell()
{
	return (ULONG)ftell(m_pFile);
}

bool LXFileUtility::ReadTextFile(const wchar_t* strFilename, LXStringA& Text)
{
	std::string str;
	std::ifstream file(strFilename);
	while (std::getline(file, str))
	{
		Text += str + '\n';
	}
	file.close();
	return true;
}

bool LXFileUtility::WriteTextFile(const wchar_t* strFilename, LXStringA& Text)
{
	LXFile File;
	if (File.Open(strFilename, "wt") == true)
	{
		File.Write(Text.GetBuffer(), Text.size());
		File.Close();
		return true;
	}
	return false;
}

bool LXFileUtility::GetFileLastWriteTime(const wchar_t* strFilename)
{
	bool bResult = false;
	HANDLE hFile = ::CreateFile(strFilename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		FILETIME lastWriteTime;
		if (GetFileTime(hFile, nullptr, nullptr, &lastWriteTime))
		{
			bResult = true;
		}
		::CloseHandle(hFile);
	}

	return bResult;
}
