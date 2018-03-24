//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXCommand.h"
#include "LXActor.h"
#include "LXMaterial.h"

class LXCORE_API LXCommandDeleteActor : public LXCommand
{

public:

	LXCommandDeleteActor(const SetActors& setActors);
	virtual ~LXCommandDeleteActor(void);

	// Overridden from LXCommand
	virtual bool	 Do()override;
	virtual bool	 Undo()override;
	
	const SetActors& GetActors( ){ return _setActors; }
	void			 ClearActors( ){ _setActors.clear(); }

private:

	SetActors _setActors;

};

class LXCORE_API LXCommandDeleteMaterial : public LXCommand
{

public:

	LXCommandDeleteMaterial(const SetMaterials& setMaterials);
	virtual ~LXCommandDeleteMaterial(void);

	// Overridden from LXCommand
	virtual bool	 Do()override;
	virtual bool	 Undo()override;
	
	const SetMaterials& GetMaterials( ){ return _setMaterials; }
	void			 ClearMaterials( ){ _setMaterials.clear(); }

private:

	SetMaterials _setMaterials;

};


class LXCORE_API LXCommandDeleteKey : public LXCommand
{

public:

	LXCommandDeleteKey(const SetKeys& setKeys);
	virtual ~LXCommandDeleteKey(void);

	// Overridden from LXCommand
	virtual bool	 Do()override;
	virtual bool	 Undo()override;
	
	const SetKeys& GetKeys(){ return _setKeys; }
	void			 ClearKeys(){ _setKeys.clear(); }

private:

	SetKeys _setKeys;

};
