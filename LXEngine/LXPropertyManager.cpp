//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXPropertyManager.h"
#include "LXMaterial.h"
#include "LXActor.h"

LXPropertyManager::LXPropertyManager()
{
}

LXPropertyManager::~LXPropertyManager()
{
}

const LXString LXPropertyManager::GetPropertyLabel(LXPropertyID id)
{
	switch (id)
	{
	//
	// Background
	//
	case LXPropertyID::BACKGROUND_BOTTOMCOLOR: return L"Bottom color";
	//
	// Engine
	//
	case LXPropertyID::ENGINE_MIRROR_TEXTURE_SIZE: return L"Mirror Texture Size";
	case LXPropertyID::ENGINE_SELECTIONMODE: return L"Selection mode";
	case LXPropertyID::ENGINE_UNDOSTACKSIZE: return L"UndoStackSize";
	case LXPropertyID::ENGINE_IMPORTMATERIALS: return L"Import materials";
	case LXPropertyID::ENGINE_IMPORTTEXTURES: return L"Import textures";
	case LXPropertyID::ENGINE_IMPORTMERGEMATERIALS: return L"Merge materials";
	case LXPropertyID::ENGINE_IMPORTMERGEPRIMITIVES: return L"Merge primitives";
	case LXPropertyID::MSAA: return L"MSAA";
	case LXPropertyID::ENGINE_SHOWBBOX: return L"Show BBox";
	//
	// ViewState
	//
	case LXPropertyID::GROUND: return L"Ground";
	case LXPropertyID::MIRROR: return L"Mirror";
	case LXPropertyID::DYNAMICLIGHT: return L"Dynamic light";
	case LXPropertyID::LIGHTING: return L"Lighting";
	case LXPropertyID::SHADOWS: return L"Shadows";
	case LXPropertyID::SSOA: return L"Ambient occlusion";
	case LXPropertyID::SSAO_RADIUS: return L"Radius";
	case LXPropertyID::SSAO_POWER: return L"Power";
	case LXPropertyID::SSOA_SMOOTH: return L"Smooth";
	case LXPropertyID::VIEWSTATE_SILHOUETTE: return L"Silhouette";
	case LXPropertyID::GLOW: return L"High-light glow";
	case LXPropertyID::VIEWSTATE_DEPTHOFFIELD: return L"Depth of field";
	case LXPropertyID::FXAA: return L"FXAA";
	case LXPropertyID::NEARFARMODE: return L"Near & far mode";
	case LXPropertyID::HOUR:return L"Hour";
	case LXPropertyID::VIEWSTATE_LAYERDEEPNESS: return L"Deepness";
	case LXPropertyID::VIEWSTATE_LAYEROPACITY: return L"Opacity";
	//
	// Material
	//
	case LXPropertyID::MATERIAL_SHINSTRENGHT: return L"Shininess strength";
	case LXPropertyID::MATERIAL_SHININESS: return	L"Shininess";
	case LXPropertyID::MATERIAL_AMBIENT: return L"Ambient";
	case LXPropertyID::MATERIAL_DIFFUSE: return L"Diffuse";
	case LXPropertyID::MATERIAL_EMISSION: return L"Emission";
	case LXPropertyID::MATERIAL_HEIGHTMAPBIAS: return L"Bias";
	case LXPropertyID::MATERIAL_HEIGHTMAPSCALE: return L"Scale";
	case LXPropertyID::MATERIAL_LIGHTING: return L"Lighting";
	case LXPropertyID::MATERIAL_NORMALMAPSPACE: return L"Normal std::map space";
	case LXPropertyID::MATERIAL_OPACITY: return L"Opacity";
	case LXPropertyID::MATERIAL_SHADER: return L"Shader";
	case LXPropertyID::MATERIAL_SPECULAR: return L"Specular";
	case LXPropertyID::MATERIAL_TEXTURE0: return L"Normal Map";
	case LXPropertyID::MATERIAL_TEXTURE1: return L"Reflection Map";
	case LXPropertyID::MATERIAL_TEXTURE2: return L"Color Map";
	case LXPropertyID::MATERIAL_TEXTURE3: return L"Opacity Map";
	case LXPropertyID::MATERIAL_TEXTURE4: return L"Height Map";
	case LXPropertyID::MATERIAL_TEXTUREENBALED0: return L"Normal Map";
	case LXPropertyID::MATERIAL_TEXTUREENBALED1: return L"Reflection Map";
	case LXPropertyID::MATERIAL_TEXTUREENBALED2: return L"Color Map";
	case LXPropertyID::MATERIAL_TEXTUREENBALED3: return L"Opacity Map";
	case LXPropertyID::MATERIAL_TEXTUREENBALED4: return L"Height Map";
	case LXPropertyID::MATERIAL_TEXTUREGEN1: return L"Gen 1";
	case LXPropertyID::MATERIAL_TWOSIDED: return L"Two-sided";

	default: return L"";
	}
}

/*static*/
void LXPropertyManager::GetProperties(const SetActors& setActor, const LXPropertyID& PID, ListProperties& olistProperties)
{
	for (auto It = setActor.begin(); It != setActor.end(); It++)
	{
		if (LXProperty* pProperty = (*It)->GetProperty(PID))
			olistProperties.push_back(pProperty);
	}
}

typedef std::map<LXString, int> MapStringID;

int LXPropertyManager::CreatePropertyID(const LXString& label)
{
	static MapStringID mapStringID;
	// +1000 to avoid collision with ID generated with LX_PROPERTYID_AUTO
	static int lastID = (int)(LXPropertyID::LASTID) + 1000; 

	auto It = mapStringID.find(label);
	if (It != mapStringID.end())
	{
		CHK(0);
		return -1;
	}

	int id = lastID++;
	mapStringID[label] = id;
	return id;
}

int LXPropertyManager::GetPropertyID(const LXString& label)
{
	static MapStringID mapStringID;
	// +1000 to avoid collision with ID generated with LX_PROPERTYID_AUTO
	static int lastID = int(LXPropertyID::LASTID) + 1000;

	auto It = mapStringID.find(label);
	if (It != mapStringID.end())
	{
		return (*It).second;
	}

	int id = lastID++;
	mapStringID[label] = id;
	return id;
}