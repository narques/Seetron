//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

template<class T>
class LXReference
{
public:

	LXReference() 
	{

	};

	LXReference(T* x)
	{
		_ref = x;
	}
	
	~LXReference() 
	{

	};

	inline T& operator=(T* x)
	{
		_ref = x;
		return *_ref;
	}
	
	inline T* operator->() const
	{ 
		return _ref; 
	}

	inline T* get() const
	{
		return _ref;
	}

	inline bool	operator==(const T* x) const
	{
		return _ref == x;
	}

	inline bool	operator!=(const T* x) const 
	{ 
		return _ref != x;
	}
		
private:

	T* _ref = nullptr;
};

template<class T>
inline bool operator==(const LXReference<T>& a, const LXReference<T>& b)
{
	return a.get() == b.get();
}

template<class T>
inline bool operator!=(const LXReference<T>& a, const LXReference<T>& b)
{
	return a.get() != b.get();
}

template<class T>
inline bool operator==(const T* a, const LXReference<T>& b)
{
	return a == b.get();
}

template<class T>
inline bool operator!=(const T* a, const LXReference<T>& b)
{
	return a != b.get();
}