//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include "LXFilepath.h"

class LXCORE_API LXSettings : public LXObject
{
public:

	LXSettings(void);
	virtual ~LXSettings(void);

	void				Load                          ( );
	const LXFilepath&	GetMaterialsFolder            ( ) { return _MaterialsFolder; }
	const LXFilepath&	GetModelsFolder               ( ) { return _ModelsFolder; }
	const LXFilepath&	GetTexturesFolder             ( ) { return _TexturesFolder; }
	const LXFilepath&	GetShadersFolder			  ( ) { return _ShadersFolder; }
	const LXFilepath&   GetDataFolder				  ( ) { return _DataFolder; }
	const LXFilepath&	GetProjectsFolder			  ( ) { return _ProjectsFolder; }
	const LXFilepath&	GetCoreFolder				  ( ) { return _CoreFolder; }
	const LXFilepath&	GetPluginsFolder			  ( ) { return _PluginsFolder; }
	const LXFilepath&   GetScriptsFolder			  ( ) { return _Scripts; }

private:

	bool				IsFolder	 		   		  ( const LXString& strPath );
	void				InitPaths                     ( );

private:

	LXFilepath			_DataFolder;              
	LXFilepath			_TexturesFolder;          
	LXFilepath			_MaterialsFolder;         
	LXFilepath			_ModelsFolder;            
	LXFilepath			_ShadersFolder;			
	LXFilepath			_ProjectsFolder;
	LXFilepath			_CoreFolder;
	LXFilepath			_PluginsFolder;
	LXFilepath			_Scripts;
};

LXCORE_API LXSettings& GetSettings();