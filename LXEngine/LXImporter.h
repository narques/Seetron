//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include "LXFilepath.h"

class LXProject;
class LXMesh;

#define LX_DECLARE_IMPORTER_METHODS \
	virtual std::shared_ptr<LXMesh> Load ( const LXFilepath& strFilename ) override; \
	virtual void GetExtensions( ListStrings& listExtensions ) override; \

class LXCORE_API LXImporter : public LXObject
{

public:

	// Constructor
	LXImporter(LXProject* pDocument);

	// Destructor
	virtual ~LXImporter(void);

	// Misc
	virtual std::shared_ptr<LXMesh> Load(const LXFilepath& strFilename) = 0;
	virtual void GetExtensions(ListStrings& listExtensions) = 0;

	LXProject* GetDocument() { return _Project; }
	void SetDocument(LXProject* Project) { _Project = Project; }

protected:

	LXProject* _Project;

};
