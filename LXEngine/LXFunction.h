//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------
//
// Encapsulate a function pointer.
// Supports lambda and class member functions.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#define USE_STDFUNCTION 1

template <typename R, typename...Params>
class LXFunction
{
public:

	virtual R Invoke(Params...) = 0;
};

//------------------------------------------------------------------------------------------------------

template <typename R, typename T, typename...Params>
class LXFunctionT : public LXFunction<R, Params...>
{

public:

	virtual R Invoke(Params... params) override
	{
#if USE_STDFUNCTION
		return Func(Obj, params...);
#else
		return Obj->Func(params...);
#endif
	}

#if USE_STDFUNCTION
	std::function<R(T*, Params...)> Func;
#else
	R(T::* Func)(Params...);
#endif
	T* Obj;
};

//------------------------------------------------------------------------------------------------------

template <typename R, typename...Params>
class FunctionLambda : public LXFunction<R, Params...>
{

public:

	virtual R Invoke(Params... params) override
	{
		return Func(params...);
	}

#if USE_STDFUNCTION
	std::function<R(Params...)> Func;
#else
	R(*Func)(Params...);
#endif
};
