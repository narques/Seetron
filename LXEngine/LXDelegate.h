//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXFunction.h"
#include <assert.h>
#include <vector>

typedef unsigned long long LXDelegateHandle;

template <typename R, typename...Params>
class LXDelegateBase
{

public:

	LXDelegateBase()
	{
	}

	~LXDelegateBase()
	{
		CHK(_callbacks.size() == 0);
	}

	bool FunctionExist(size_t hash)
	{
		return _callbacks.find(hash) != _callbacks.end();
	}

	//
	// For member function
	//

	template <typename T>
	void Attach(T* obj, R(T::* func)(Params...))
	{
		LXFunctionT<R, T, Params...>* mf = new LXFunctionT<R, T, Params...>();
		mf->Func = func;
		mf->Obj = obj;
		void* funcPtr = (void*&)func;
		size_t hash = GetHash(obj, funcPtr);
		assert(!FunctionExist(hash));
		_callbacks[hash] = mf;
	}

	template <typename T>
	bool Detach(T* obj, R(T::* func)(Params...))
	{
		void* funcPtr = (void*&)func;
		size_t hash = GetHash(obj, funcPtr);
		return Detach(hash);
	}

	//
	// For Lambda
	//


	template<typename F>
	LXDelegateHandle AttachMemberLambda(const F& func)
	{
		FunctionLambda<R, Params...>* lf = new FunctionLambda<R, Params...>();
		lf->Func = func;
		auto funcPtr = &func;
		size_t hash = GetHash(nullptr, funcPtr);
		assert(!FunctionExist(hash));
		_callbacks[hash] = lf;
		return hash;
	}

	void AttachMemberLambda2(std::function<R(Params...)> func)
	{
		FunctionLambda<R, Params...>* lf = new FunctionLambda<R, Params...>();
		lf->Func = func;
		auto funcPtr = &func;
		size_t hash = GetHash(nullptr, funcPtr);
		assert(!FunctionExist(hash));
		_callbacks[hash] = lf;
	}

	// 
	// For Captureless Lambda or function
	//

	void AttachLambda(R(*func)(Params...))
	{
		FunctionLambda<R, Params...>* lf = new FunctionLambda<R, Params...>();
		lf->Func = func;
		size_t hash = GetHash(nullptr, func);
		assert(!FunctionExist(hash));
		_callbacks[hash] = lf;
	}

	LXDelegateBase& operator+=(R(*func)(Params...))
	{
		AttachLambda(func);
		return *this;
	}

	bool DetachLambda(R(*func)(Params...))
	{
		size_t hash = GetHash(nullptr, func);
		return Detach(hash);
	}

	//
	// Common
	//

	bool Detach(LXDelegateHandle handle)
	{
		auto it = _callbacks.find((size_t)handle);
		if (it != _callbacks.end())
		{
			delete it->second;
			_callbacks.erase((size_t)handle);
			return true;
		}
		else
		{
			return false;
		}
	}

	//
	// ...
	//

	R Invoke(Params... params)
	{
		std::vector<LXFunction<R, Params...>*> invokes;
		
		for (auto it = _callbacks.begin(); it != _callbacks.end(); it++)
		{
			invokes.push_back(it->second);
		}
		
		for (int i = 0; i < invokes.size(); i++)
		{
			if (i == invokes.size() - 1)
				return invokes[i]->Invoke(params...);
			else
				invokes[i]->Invoke(params...);
		}
		return R();
	}
	

private:

	size_t GetHash(const void* obj, const void* func) const
	{
		unsigned long long objAddress = (unsigned long long)obj;
		unsigned long long funcAddress = (unsigned long long)func;
		std::hash<unsigned long long> h1f;
		size_t h1 = h1f(objAddress);
		std::hash<unsigned long long> h2f;
		size_t h2 = h2f(funcAddress);
		return h1 ^ (h2 << 1);
	}

	std::map<size_t, LXFunction<R, Params...>*> _callbacks;
};

template <typename...Params>
class LXDelegate : public LXDelegateBase<void, Params...>
{
};
