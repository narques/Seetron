//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

template <typename T>
class LXSynchronisableSet
{
public:

	~LXSynchronisableSet()
	{
		CHK(_set0.size() == 0);
		CHK(_set1.size() == 0);
	}

	bool IsEmpty() const
	{
		return _set0.size() == 0 && _set1.size() == 0;
	}
	
	void Swap()
	{
		std::set<T>* tmp = In;
		In = Out;
		Out = tmp;
	}

	std::set<T>* In = &_set0;
	std::set<T>* Out = &_set1;

private:

	std::set<T> _set0;
	std::set<T> _set1;

};