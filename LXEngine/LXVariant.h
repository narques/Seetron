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

class LXCORE_API LXVariant
{

public:

	//LXVariant();
	virtual ~LXVariant(); 
	virtual bool IsValueEqual(LXVariant*)= 0;
};

template <class T>
class LXCORE_API LXVariantT : public LXVariant
{

public:

	LXVariantT(const T& value){ m_value = value; }
	
	const T& GetValue() const { return m_value; }
	T* GetValuePtr() { return &m_value;  }
		
	bool IsValueEqual(LXVariant* v) override
	{
		LXVariantT<T>* p = dynamic_cast<LXVariantT<T>*>(v);   // TODO type sous forme d'enum pour un cast sec !
		if (!p)
			return false; // Type is different
		return p->GetValue() == m_value;
	}

private:

	T m_value;

};

class LXVariant2
{

public:

	enum Type
	{
		type_undefined,
		type_int,
		type_float,
		type_double,
		type_vec3f
	};
	
	LXVariant2(){};
	LXVariant2(Type t) :_type(t){}
	~LXVariant2(){};
	
	LXStringA ToString()
	{
		switch (_type)
		{
		case type_undefined:CHK(0); return "";
		case type_int: return LXStringA::Number(_nValue);
		case type_double: return LXStringA::Number(_dValue);
		case type_vec3f: return ("x:" + LXStringA::Number(_vec3fValue.x) + " y:" + LXStringA::Number(_vec3fValue.y) + " z:" + LXStringA::Number(_vec3fValue.z));
		default:CHK(0); return "";
		}
	}

	LXVariant2& operator=(int n) { SetInt(n); return *this; }
	void	SetInt(int n) { _type = type_int;  _nValue = n; }
	int		GetInt() const { CHK(_type == type_int);  return _nValue; }
	
	LXVariant2& operator=(double d) { SetDouble(d); return *this; }
	void	SetDouble(double d) { _type = type_double; _dValue = d; }
	double	GetDouble() const { CHK(_type == type_double); return _dValue; }

	LXVariant2& operator=(vec3f d) { SetVec3f(d); return *this; }
	void	SetVec3f(vec3f d) { _type = type_vec3f; _vec3fValue = d; }
	vec3f	GetVec3f() const { CHK(_type == type_vec3f); return _vec3fValue; }
	
	Type	GetType() const { return _type; }

private:

	Type _type = type_undefined;

	union 
	{
		struct  { int _nValue; };
		struct	{ float _fValue; };
		struct  { double _dValue; };
		struct  { vec3f _vec3fValue; };
	};
};


typedef std::list<LXVariant2*> ListVariants;
