//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#pragma warning( disable : 4251 )  

#include <string>
#include <list>
#include <algorithm>
#include <iostream>
#include <iosfwd>
#include <sstream>

typedef vector<char> ArrayChar;

// A max number of char
// Buffers used for conversions
#define LX_MAX_NUMBER_OF_CHAR 4096 

class LXCORE_API LXString
{

public:

	// Constructors
	LXString(void)														{ }
	LXString(const LXString& sz):m_str(sz.m_str.c_str())				{ }
	LXString(const wstring& sz):m_str(sz)								{ }
	LXString(const wchar_t* sz)											{ if (sz) m_str = sz; }
	LXString(const char* sz)											
	{ 
		CHK(strlen(sz) < LX_MAX_NUMBER_OF_CHAR);
		wchar_t l_wchart[LX_MAX_NUMBER_OF_CHAR];
		mbstowcs(l_wchart, sz, LX_MAX_NUMBER_OF_CHAR);
		m_str = l_wchart;
	}

	// Accessors
	operator wchar_t*() const											{ return (wchar_t*)m_str.c_str();  }		// WARNING const ?
	operator const wchar_t*() const										{ return m_str.c_str();  }

	// Operators
	wchar_t& operator[]( unsigned int i )								{ return (wchar_t&)m_str.c_str()[i]; }				// WARNING const ?
	
	LXString& operator=(const LXString& str)							{ assign(str.m_str.c_str()); return *this; }
	LXString& operator=(const wchar_t& str)								{ m_str.assign(&str); return *this; }
	LXString operator+(const wchar_t& str) const						{ LXString t  = *this; t.append(&str); return t;  	}
	LXString operator+(const wchar_t* str) const  						{ LXString t = *this; t.append(str); return t;  }

	bool operator==( const wchar_t* str ) const							{ return compare(str) == 0; }
	bool operator==( const LXString& str ) const						{ return compare(str) == 0; }
	bool operator==(const std::wstring& str) const						{ return compare(str.c_str()) == 0; }
	bool operator<( const LXString& str ) const							{ return compare(str) < 0; }
	bool operator>(const LXString& str) const							{ return compare(str) > 0; }
	bool operator!=( const wchar_t* str ) const							{ return compare(str) != 0; }
	bool operator!=( const LXString& str ) const						{ return compare(str) != 0; }

	LXString&	operator+=( LXString& v )								{ append(v.m_str.c_str()); return *this; }
	LXString&	operator+=(const wchar_t* v )							{ append(v); return *this; }
	LXString&	operator+=(  const LXString& v )						{ append(v.m_str.c_str()); return *this; 	}
	LXString operator+(const LXString& str ) const						{ LXString res = *this; res.append(str); return res; 	}

	int			Find( const wchar_t* pszSub ) const						{ return (int)m_str.find(pszSub); }
	LXString	Left( int nCount ) const								{ return LXString(m_str.substr (0, nCount ).c_str()) ; }
	LXString	Left(const wchar_t* pszSub) const;
	int			ReverseFind( wchar_t ch ) const							{ return (int)m_str.rfind(ch); }
	LXString	Right( int nCount ) const								{ return LXString(m_str.substr(m_str.size()-nCount, nCount).c_str()); 	}
	LXString	Right(const wchar_t* pszSub) const;

	int			GetLength( ) const										{ return (int)m_str.size(); }
	bool		IsEmpty( ) const 										{ return( GetLength() == 0 ); 	}
	LXString&	MakeLower( )											{ transform(m_str.begin(), m_str.end(), m_str.begin(), ::tolower); return( *this ); }
	LXString	ToLower( ) const 										{ LXString str = *this; return str.MakeLower(); }
	wchar_t*	GetBuffer( ) const										{ return (wchar_t*)m_str.c_str(); } // WARNING const
	ArrayChar	GetBufferA() const										
	{ 
		char l_char[256]; 
		auto size = wcstombs(l_char, m_str.c_str(), 256); 
		ArrayChar ac;
		ac.insert(ac.begin(), l_char, l_char+size);
		ac.push_back('\0');
		return ac;
	}

	const wchar_t*	GetConstBuffer( ) const								{ return m_str.c_str(); }
	void		SetString( const LXString& str )						{ m_str.assign(str); }
	int			CompareNoCase( const wchar_t* str ) const						{ return compare(str); }

	int Replace( wchar_t* pszOld, wchar_t* pszNew )
	{
		LXString strToReplace(pszOld);
		int nSize = strToReplace.size();
		int nPos = (int)m_str.find(strToReplace);
		LXString strReplacement(pszNew);
		m_str.replace(nPos, nSize, strReplacement);
		return 0;
	}

	int ReplaceAll(const wchar_t* pszOld, const wchar_t* pszNew)
	{
		LXString strToReplace(pszOld);
		int nSize = strToReplace.size();
		int nPos = (int)m_str.find(strToReplace);
		while (nPos != -1)
		{
			LXString strReplacement(pszNew);
			m_str.replace(nPos, nSize, strReplacement);
			nPos = (int)m_str.find(strToReplace);
		}
		return 0;
	}

	LXString& TrimLeft( const wchar_t* pszTargets )
	{
		LXString strToReplace(pszTargets);
		int nSize = strToReplace.size();
		LXString strReplacement;
		m_str.replace(0, nSize, strReplacement);
		return *this;
	}

	void Split(vector<LXString>& ArrayString) const
	{
		wstringstream ss;
		ss.str(m_str);
		wstring s;
		while (std::getline(ss, s, L' '))
			ArrayString.push_back(LXString(s));
	}

	static LXString Number(int i);
	static LXString Number(unsigned __int64 ul);
	static LXString Number(double d);
	static LXString NumberToHexa(unsigned __int64 ul);

	static LXString Format(const wchar_t* Format, ...);



	void assign(const wchar_t* str)
	{
		m_str.assign(str);
	}
	

	void append(const wchar_t* str)
	{
		m_str.append(str);
	}

	int compare(const wchar_t* str) const
	{ 
		return m_str.compare(str);
	}

	int compare(wchar_t& str) const
	{ 
		return m_str.compare(&str);
	}

	int size()
	{
		return (int)m_str.size();
	}

	int find(const wchar_t* str)
	{
		return (int)m_str.find(str);
	}

	LXString substr(size_t offset, size_t count)
	{
		return m_str.substr(offset, count);
	}
	float ToFloat();

	wstring m_str;
};

//Explicit class Instantiation
template class basic_string < wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> > ;

class LXCORE_API LXStringA
{

public:

	// Constructors
	LXStringA(void) { }
	LXStringA(const LXStringA& sz) :m_str(sz.m_str.c_str()) { }
	LXStringA(const string& sz) :m_str(sz) { }
	LXStringA(const char* sz) { if (sz) m_str = sz; }

	// Accessors
	operator char*() const { return (char*)m_str.c_str(); }   // WARNING
	operator const char*() const { return m_str.c_str(); }

	// Operators
	char& operator[](unsigned int i) { return (char&)m_str.c_str()[i]; }   // WARNING

	LXStringA& operator=(const LXStringA& str) { assign(str.m_str.c_str()); return *this; }
	LXStringA& operator=(const char& str) { m_str.assign(&str); return *this; }
	LXStringA operator+(const char& str) const { LXStringA t = *this; t.append(&str); return t; }
	LXStringA operator+(const char* str) const { LXStringA t = *this; t.append(str); return t; }

	bool operator==(const char* str) const { return compare(str) == 0; }
	bool operator==(const LXStringA& str) const { return compare(str) == 0; }
	bool operator<(const LXStringA& str) const { return compare(str) < 0; }
	bool operator!=(const char* str) const { return compare(str) != 0; }
	bool operator!=(const LXStringA& str) const { return compare(str) != 0; }

	LXStringA&	operator+=(LXStringA& v) { append(v.m_str.c_str()); return *this; }
	LXStringA&	operator+=(const char* v) { append(v); return *this; }
	LXStringA&	operator+=(const LXStringA& v) { append(v.m_str.c_str()); return *this; }
	LXStringA operator+(const LXStringA& str) const { LXStringA res = *this; res.append(str); return res; }

	int			Find(const char* pszSub) const { return (int)m_str.find(pszSub); }
	LXStringA	Left(int nCount) const { return LXStringA(m_str.substr(0, nCount).c_str()); }
	int			ReverseFind(char ch) const { return (int)m_str.rfind(ch); }
	LXStringA	Right(int nCount) const { return LXStringA(m_str.substr(m_str.size() - nCount, nCount).c_str()); }

	int			GetLength() const { return (int)m_str.size(); }
	bool		IsEmpty() const { return(GetLength() == 0); }
	LXStringA	MakeLower() { transform(m_str.begin(), m_str.end(), m_str.begin(), ::tolower); return(*this); }
	char*	GetBuffer() const { return (char*)m_str.c_str(); }   // WARNING
	const char*	GetConstBuffer() const { return m_str.c_str(); }
	void		SetString(const LXStringA& str) { m_str.assign(str); }
	int			CompareNoCase(char* str) const { return compare(str) == 0; }

	int Replace(const char* pszOld, const char* pszNew)
	{
		LXStringA strToReplace(pszOld);
		int nSize = strToReplace.size();
		int nPos = (int)m_str.find(strToReplace);
		if (nPos == -1)
			return 0;
		LXStringA strReplacement(pszNew);
		m_str.replace(nPos, nSize, strReplacement);
		return 0;
	}

	LXStringA& TrimLeft(char* pszTargets)
	{
		LXStringA strToReplace(pszTargets);
		int nSize = strToReplace.size();
		LXStringA strReplacement;//(L""); // TODO
		m_str.replace(0, nSize, strReplacement);
		return *this;
	}

	static LXStringA Format(const char* Format, ...);

	static LXStringA Number(int i);
	static LXStringA Number(unsigned int i);
#ifdef ANDROID
	static LXStringA NumberToHexa(int ul);
#else
	static LXStringA Number(double d);
	static LXStringA Number(unsigned __int64 ul);
	static LXStringA NumberToHexa(unsigned __int64 ul);
#endif


	void assign(const char* str)
	{
		m_str.assign(str);
	}

	void Erase(uint Pos, uint Len)
	{
		m_str.erase(Pos, Len);
	}
	
	void Insert(uint Pos, const char* Str)
	{
		m_str.insert(Pos, Str);
	}
	
	void append(const char* str)
	{
		m_str.append(str);
	}

	int compare(const char* str) const
	{ 
		return m_str.compare(str);
	}

	int compare(char& str) const
	{ 
		return m_str.compare(&str);
	}

	int size()
	{
		return (int)m_str.size();
	}

	int find(char* str)
	{
		return (int)m_str.find(str);
	}

	LXStringA substr(size_t offset, size_t count)
	{
		return m_str.substr(offset, count);
	}

	//basic_string<T> m_str;
	string m_str;
};




//typedef LXStringT<char>		LXStringA;	
//typedef LXStringT<wchar_t>	LXString;





















static LXString Number(int i)
{
#ifdef ANDROID
	char sza[256];
	sprintf(sza,"%d",i); // TODO wsprintf ?
	wchar_t sz[256];
	wcstombs(sza, sz, 256);
	return LXString(sz);
#else
	wchar_t sz[256];
	_itow(i, sz, 10);
	return LXString(sz);
#endif
}

 inline LXString operator+(const wchar_t* sz, const LXString& str )
 {
	LXString res(sz);
	res.append(str);
	return res;
 }

  inline LXStringA operator+(const char* sz, const LXStringA& str )
 {
	LXStringA res(sz);
	res.append(str);
	return res;
 }
  
 inline bool operator==(const std::wstring& str0, const LXString& str1)
 { 
	 return str1.compare(str0.c_str()) == 0; 
 }

 // Return a tab string "\t\t\t..."
 LXString GetTab(int Count);
 
 typedef vector<LXString> ArrayStrings;
 typedef list<LXString> ListStrings;