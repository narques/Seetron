//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

enum class LXPropertyID
{
	Undefined,
	DrawSelection,
	Hilightable,
	Pickable,
	AnimationDuration,
	BehaviorRotate,
	BehaviorRotateSpeed,
	BOX_HEIGHT,
	BOX_LENGTH,
	BOX_WITDH,
	BACKGROUND_BOTTOMCOLOR,
	BackgroundIntensity,
	BACKGROUND_TEXTURE,
	BACKGROUND_TEXTURE_IRRADIANCE,
	BACKGROUND_TOPCOLOR,
	BACKGROUND_TYPE,
	CAMERA_APERTURE,
	CAMERA_FOV,
	CAMERA_HEIGHT,
	CAMERA_ORTHO,
	CAMERA_TARGET,
	CORE_ANTIALIASING,
	CORE_IMPORTMATERIALS,
	CORE_IMPORTMERGEMATERIALS,
	CORE_IMPORTMERGEPRIMITIVES,
	CORE_IMPORTTEXTURES,
	CORE_MIRROR_TEXTURE_SIZE,
	CORE_SELECTIONMODE,
	CORE_SHADOW_TEXTURE_SIZE,
	CORE_SHOWBBOX,
	CORE_SHOWFBO,
	CORE_SHOWFBOBACK,
	CORE_SHOWFBOBLENDED,
	CORE_SHOWFBOSIZE,
	CORE_SHOWFBOFILTER,
	CORE_SHOWFBOMAIN,
	CORE_SHOWFBOMIRROR,
	CORE_SHOWFBOPICKING,
	CORE_SHOWFBOSHADOW,
	CORE_SHOWINFOS,
	CORE_SHOWNORMALS,
	CORE_UNDOSTACKSIZE,
	DEFERREDLIGHTING,
	VIEWSTATE_DEPTHOFFIELD,
	DEPTHOFFIELDLDEPTH,
	DESCRIPTION,
	DOC_MODELUNIT,
	DOC_MODELUNIT_METER,
	DOC_UPAXIS,
	DOC_HAND,
	DOCUMENT_HILIGHTING,
	DOCUMENT_PAGEHEIGHT,
	DOCUMENT_PAGEWITDH,
	DYNAMICLIGHT,
	ENTITY,
	Far,
	FXAA,
	GLOW,
	GROUND,
	GROUND_AUTOPOSTION,
	GROUND_AUTOSIZE,
	GROUND_AUTOSIZEFACTOR,
	GROUND_SIZE,
	HOUR,
	KEY_TIME,
	KEY_VALUE,
	VIEWSTATE_LAYERDEEPNESS,
	VIEWSTATE_LAYERED,
	LAYERMATERIAL0,
	LAYERMATERIAL1,
	VIEWSTATE_LAYEROPACITY,
	LAYERTRANSITION,
	VIEWSTATE_LAYERUSEMASK,
	LIGHT,
	LIGHT_COLOR,
	LIGHT_CASTSHADOW,
	LIGHT_INTENSITY,
	LIGHT_TYPE,
	LIGHTING,
	LISTPROPERTIES,
	MATERIAL_AMBIENT,
	MATERIAL_DIFFUSE,
	MATERIAL_EMISSION,
	MATERIAL_FRESNEL,
	MATERIAL_HEIGHTMAPBIAS,
	MATERIAL_HEIGHTMAPSCALE,
	MATERIAL_LIGHTING,
	MATERIAL_LIGHTINGMODEL,
	MATERIAL_NORMALMAPSPACE,
	MATERIAL_OPACITY,
	MATERIAL_ROUGHNESS,
	MATERIAL_ROUGHNESS2,
	MATERIAL_REFLECTANCE,
	MATERIAL_BRIGHTNESSFACTOR,
	MATERIAL_SHADER,
	MATERIAL_SHININESS,
	MATERIAL_SHININESS2,
	MATERIAL_SHINSTRENGHT,
	MATERIAL_SPECULAR,
	MATERIAL_SPECULAR2,
	MATERIAL_TEXTURE0,
	MATERIAL_TEXTURE1,
	MATERIAL_TEXTURE2,
	MATERIAL_TEXTURE3,
	MATERIAL_TEXTURE4,
	MATERIAL_TEXTURE5,
	MATERIAL_TEXTURE6,
	MATERIAL_TEXTURE7,
	MATERIAL_TEXTURE_AMOUNT0,
	MATERIAL_TEXTUREENBALED0,
	MATERIAL_TEXTUREENBALED1,
	MATERIAL_TEXTUREENBALED2,
	MATERIAL_TEXTUREENBALED3,
	MATERIAL_TEXTUREENBALED4,
	MATERIAL_TEXTUREENBALED5,
	MATERIAL_TEXTUREENBALED6,
	MATERIAL_TEXTUREENBALED7,
	MATERIAL_TEXTUREGEN0,
	MATERIAL_TEXTUREGEN1,
	MATERIAL_TEXTUREGEN2,
	MATERIAL_TEXTURE_TILING,
	MATERIAL_TWOSIDED,
	MATERIAL_VERSION,
	MESH_LAYER,
	MESH_OUTLINES,
	MIRROR,
	MSAA,
	NAME,
	Near,
	NEARFARMODE,
	OBJECT_UID,
	POSITION,
	PRIMITIVE_BINORMALS,
	PRIMITIVE_INDICES,
	PRIMITIVE_MODE,
	PRIMITIVE_NORMALS,
	PRIMITIVE_TANGENTS,
	PRIMITIVE_TEXCOORDS,
	PRIMITIVE_VISIBLE,
	PRIMITIVE_VERTICES,
	ROTATION,
	SCALE,
	PPL,
	PRIMITIVE_MATERIAL,
	RENDERBUFFER_TIF,
	RENDERING_MATERIAL,
	RENDERING_OUTLINES,
	SHADOWS,
	VIEWSTATE_SILHOUETTE,
	SIZEX,
	SIZEY,
	SIZEZ,
	SSAO_POWER,
	SSAO_RADIUS,
	SSOA,
	SSOA_SMOOTH,
	TERRAIN_BLADES,
	TERRAIN_BLADESTEP,
	TERRAIN_GRASS_HEIGHT,
	TERRAIN_GRASS_WIDTH,
	TERRAIN_LENGTH,
	TERRAIN_PATCHGRASS,
	TERRAIN_PATCHWIDTH,
	TERRAIN_SLICES,
	TERRAIN_THRESHOLDGEOEND,
	TERRAIN_THRESHOLDSLICESEND,
	TERRAIN_WIDTH,
	TRACK_INTERPOLATION,
	TRACK_OBJECT_UID,
	TRACK_PROPERTY,
	TRANSFORMATION,
	VIEWSTATE_BRIGHTNESS,
	VIEWSTATE_EXPOSURE,
	VIEWSTATE_GAMMA,
	VISIBILITY,
	LASTID, // Must be the last
};
