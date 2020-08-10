//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include "LXVec3.h"

class LXCORE_API LXFile : public LXObject
{

public:

	LXFile(void);
	virtual ~LXFile(void);

	bool	Open		( const LXString& strFilename, const LXString& strOptions );
	void	Close		( );
	
	size_t	Read		( void* pBuffer, size_t size );
	int		ReadInt		( );
	vec3f	ReadVec3f	( );
	short	ReadShort	( );

	char*	GetString	( char* str, int n );
	
	bool	Write		( const void* pBuffer, size_t size, bool Flush = false);

	void	Seek		( int nSeekMethod );
	void	Rewind		( );
	ULONG	Tell		( );
	
private:

	FILE* m_pFile;

};

class LXFileUtility
{

public:
	
	static bool ReadTextFile(const wchar_t* strFilename, LXStringA& Text);
	static bool WriteTextFile(const wchar_t* strFilename, LXStringA& Text);
	static bool GetFileLastWriteTime(const wchar_t* strFilename);

};

