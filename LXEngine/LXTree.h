//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include <list>

template <class T>
class LXTreeNode
{

public:

	virtual void AddChild(T* child)
	{
		CHK(child);
		CHK(!child->GetParent());
		child->SetParent((T*)this);
		_children.push_back(child);
	}

	virtual void RemoveChild(T* child)
	{
		CHK(child);
		if (!child)
			return;

		CHK(child->GetParent());
		_children.remove(child);
		child->_lastKnownParent = child->_parent;
		child->SetParent(nullptr);
	}

	void SetParent(T* parent) { _parent = parent; };
	T* GetParent() const { return _parent; }
	T* GetPreviousParent() const { return _lastKnownParent; }
	std::list<T*>& GetChildren() { return _children; }

protected:
	
	T* _parent = nullptr;
	T* _lastKnownParent = nullptr;
	std::list<T*> _children;
};
