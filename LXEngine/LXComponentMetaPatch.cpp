//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXComponentMetaPatch.h"

// Seetron
#include "LXActorRenderToTexture.h"
#include "LXAssetManager.h"
#include "LXBitmap.h"
#include "LXEngine.h"
#include "LXComponentTerrainGenerator.h"
#include "LXLogger.h"
#include "LXMaterial.h"
#include "LXMaterialInstance.h"
#include "LXMath.h"
#include "LXMeshBase.h"
#include "LXPrimitive.h"
#include "LXPrimitiveInstance.h"
#include "LXScene.h"
#include "LXTerrain.h"
#include "LXTexture.h"
#include "LXTextureD3D11.h"
#include "LXRenderData.h"

namespace TerrainMetaPatch
{
	const bool Erode = true;
}

LXComponentMetaPatch::LXComponentMetaPatch(LXActor* actor):
	LXComponentMesh(actor)
{
}

LXComponentMetaPatch::~LXComponentMetaPatch()
{

}

void LXComponentMetaPatch::CreateMainResources(const LXTerrainInfo& terrainInfo, const vec2f& metaPatchPosition)
{
	//const int textureSize = Terrain->GetHeightMapTextureSize();
	const int textureSize = terrainInfo.GetHeightMapTextureSize();

	// Creates the HeightMap texture
	HeightMap = LXTexture::Create(textureSize, textureSize, ETextureFormat::LX_R16_USHORT);
	HeightMap->SetName(L"HeihgtMap_" + LXString::Number(00));

	// All MetaPatch will use the same material base but a different HeightMap, so use the MaterialInstance feature.
	std::shared_ptr<LXMaterialInstance> materialInstance = LXMaterialInstance::Create(terrainInfo.Material);
	materialInstance->SetName(terrainInfo.Material->GetName() + L"_XX");
	materialInstance->Compile();

	Material = materialInstance;

	// Set the HeightMap texture property
	materialInstance->SetPropertyTextureByName(L"HeightMap", HeightMap);
	materialInstance->SetProperty<vec2f>(L"MetaPatchPosition", metaPatchPosition);

	LocalPosition = metaPatchPosition;
}

#if LX_CMP_USEGENRESOURCES
void LXComponentMetaPatch::CreateBuildResources(const LXTerrainInfo& terrainInfo, const vec2f& offset)
{
	const int textureSize = terrainInfo.GetHeightMapTextureSize();

	// A temp texture to generate the terrain R: height G: water
	std::shared_ptr<LXTexture> textureTmp0 = LXTexture::Create(textureSize, textureSize, ETextureFormat::LX_R16G16_USHORT);
	textureTmp0->SetName("textureTmp0");

	//
	// Noise textureTmp0
	//

	if (1)
	{
		_materialNoise = GetAssetManager()->GetMaterial(L"Materials/GenerateHeightMap_Noise.smat");
		std::shared_ptr<LXMaterialInstance> materialNoiseInstance = LXMaterialInstance::Create(_materialNoise);
		//materialNoiseInstance->SetName(L"MaterialNoiseInstance_" + LXString::Number(materialNoiseInstance->InstanceID));
		materialNoiseInstance->SetProperty<vec2f>(L"Offset", offset);
		// 		materialNoiseInstance->Compiled.AttachMemberLambda([this]()
		// 			{
		// 				if (_renderToTextureNoise)
		// 					_renderToTextureNoise->Render(1);
		// 			});


		materialNoiseInstance->Compile();
		std::shared_ptr<LXMaterialBase> materialBase = std::static_pointer_cast<LXMaterialBase>(materialNoiseInstance);

		_renderToTextureNoise = new LXActorRenderToTexture(nullptr);
		_renderToTextureNoise->SetPersistent(false);
		_renderToTextureNoise->SetName("_renderToTextureNoise");
		_renderToTextureNoise->SetMaterial(materialBase);
		_renderToTextureNoise->SetTexture(textureTmp0);
		_renderToTextureNoise->SetRenderingDrive(ERenderingDrive::OnDemand);
		GetScene()->AddChild(_renderToTextureNoise);
	}

	//
	// Erode textureTmp0
	//

	if (TerrainMetaPatch::Erode)
	{
		// Erode material texture input is empty, retrieve the property to set it.
		_materialErode = GetAssetManager()->GetMaterial(L"Materials/GenerateHeightMap_Erode.smat");
		std::shared_ptr<LXMaterialInstance> materialErodeInstance = LXMaterialInstance::Create(_materialErode);
		materialErodeInstance->Compile();
		materialErodeInstance->SetPropertyTextureByName(L"TextureToErode", textureTmp0);
		std::shared_ptr<LXMaterialBase> materialBase = std::dynamic_pointer_cast<LXMaterialBase>(materialErodeInstance);

		std::shared_ptr<LXTexture> textureTmp1 = LXTexture::Create(textureSize, textureSize, ETextureFormat::LX_R16G16_USHORT);
		textureTmp1->SetName("textureTmp1");

		_renderToTextureErode = new LXActorRenderToTexture(nullptr);
		_renderToTextureErode->SetPersistent(false);
		_renderToTextureErode->SetName("_renderToTextureErode");
		_renderToTextureErode->SetRenderingDrive(ERenderingDrive::OnDemand);
		_renderToTextureErode->SetMaterial(materialBase);
		_renderToTextureErode->SetTexture(textureTmp1);

		LXTexture* textureTmp0Ptr = textureTmp0.get();

		_renderToTextureErode->Rendered_RT.AttachMemberLambda([this, textureTmp0Ptr]()
			{
				LXTexture* a = textureTmp0Ptr;
				LXTexture* b = (LXTexture*)_renderToTextureErode->GetTexture().get();
				LXTextureD3D11* da = (LXTextureD3D11*)a->GetDeviceTexture();
				LXTextureD3D11* db = (LXTextureD3D11*)b->GetDeviceTexture();
				LXTextureD3D11::SwapDeviceObjects(da, db);
			});

		GetScene()->AddChild(_renderToTextureErode);
	}

	//
	// Copy textureTmp0 R Component (Height) to textureHeightMap
	//

	if (1)
	{

		_materialFinalize = GetAssetManager()->GetMaterial(L"Materials/GenerateHeightMap_Finalize.smat");

		//LXPropertyAssetPtr* propertyTextureToCopy = _materialFinalize->GetPropertyTextureByName("TextureToCopy");

		std::shared_ptr<LXMaterialInstance> materialFinalizeInstance = LXMaterialInstance::Create(_materialFinalize);
		materialFinalizeInstance->Compile();
		materialFinalizeInstance->SetPropertyTextureByName(L"TextureToCopy", textureTmp0);
		std::shared_ptr<LXMaterialBase> materialBase = std::static_pointer_cast<LXMaterialBase>(materialFinalizeInstance);


		//if (propertyTextureToCopy)
		{
			//propertyTextureToCopy->SetValue(texture);
			_renderToTextureFinalize = new LXActorRenderToTexture(nullptr);
			_renderToTextureFinalize->SetPersistent(false);
			_renderToTextureFinalize->SetName("_renderToTextureFinalize");
			_renderToTextureFinalize->SetMaterial(materialBase/*_materialFinalize*/);
			_renderToTextureFinalize->SetTexture(HeightMap);
			_renderToTextureFinalize->SetRenderingDrive(ERenderingDrive::OnDemand);
			GetScene()->AddChild(_renderToTextureFinalize);
			_renderToTextureFinalize->AllFrameRendered.AttachMemberLambda([this]()
				{
					_renderToTextureFinalize->CopyDeviceToBitmap();
				});

			// TODO : issue a chaque Compile la l'adresse de la lambda change
			HeightMap->BitmapChanged.AttachMemberLambda([this]()
				{
					// At this point the texture HeightMap is finalized, also copied on the bitmap,
					// so 'Rebuild' primitives BBoxes.
					LogI(Terrain, L"finalized the Texture HeightMap");
					OnCompiled();
				});
		}
	}
}

void LXComponentMetaPatch::GenerateHeigtMap(const LXTerrainInfo& terrainInfo)
{
	// Compile the materials
	_materialNoise->Compile();
	if (_materialErode)
		_materialErode->Compile();
	if (_materialFinalize)
		_materialFinalize->Compile();

	_renderToTextureNoise->Render(1);
	if (_renderToTextureErode)
		_renderToTextureErode->Render(_erodeFrameCount);
	if (_renderToTextureFinalize)
		_renderToTextureFinalize->Render(TerrainMetaPatch::Erode ? _erodeFrameCount : 1);
}

void LXComponentMetaPatch::OnCompiled()
{
	_compiling = false;

	_materialNoise.reset();
	_materialErode.reset();
	_materialFinalize.reset();

	GetScene()->RemoveChild(_renderToTextureNoise);
	if (_renderToTextureErode)
		GetScene()->RemoveChild(_renderToTextureErode);
	GetScene()->RemoveChild(_renderToTextureFinalize);

	LX_SAFE_DELETE(_renderToTextureNoise);
	LX_SAFE_DELETE(_renderToTextureErode);
	LX_SAFE_DELETE(_renderToTextureFinalize);

	//Terrain->OnCompiled(); // TODO !
	Compiled.Invoke();
}

#endif

vec2f LXComponentMetaPatch::GetMinMaxHeight(const LXWorldPrimitive* worldPrimitive, const LXTexture* texture, float heightScale)
{
	//if (!_doneHeightMap)
		//return vec2f(0.f, heightScale);

	const ArrayVec3f& positions = worldPrimitive->PrimitiveInstance->Primitive->GetArrayPositions();

	float min = FLT_MAX;
	float max = -FLT_MAX;

	for (vec3f position : positions)
	{
		worldPrimitive->MatrixWorld.LocalToParentPoint(position);
		min = LXMin(min, GetHeightAt(position.x, position.y, texture, heightScale));
		max = LXMax(max, GetHeightAt(position.x, position.y, texture, heightScale));
	}

	return vec2f(min, max);
}

float LXComponentMetaPatch::GetHeightAt(float x, float y, const LXTexture* texture, float heightScale) const
{
	if (texture)
	{
		if (LXBitmap* bitmap = texture->GetBitmap(0))
		{
			if (bitmap->Busy)
				return 0.f;

			const int textureWidth = texture->GetWidth();
			const int textureHeight = texture->GetHeight();
			//const float terrainSize = GetMetaPatchSize();

			// Local terrain coordinate to Texture coordinate
			/*
			x = (int)x % (int)GetMetaPatchSize();
			y = (int)y % (int)GetMetaPatchSize();

			int uvx = (float)x * (float)(textureWidth - 1) / terrainSize;
			int uvy = (float)y * (float)(textureHeight - 1) / terrainSize;
			*/

			//const LXTerrainMetaPatch& metaPatch = GetMetaPatchAt(x, y);

			vec3f translation = _actor->GetTransformation().GetTranslation();

			// TODO : Transformation & GetQuadSize
			int uvx = (x - translation.x) / 100.0;
			int uvy = (y - translation.y) / 100.0;

			CHK(uvx >= 0);
			CHK(uvy >= 0);
			CHK(uvx < textureWidth);
			CHK(uvy < textureHeight);

			return bitmap->GetNormalized(uvx, uvy) * heightScale;
		}
		else
		{
			if (texture->TextureSource == ETextureSource::TextureSourceMaterial)
			{
				//(const_cast<LXTexture*>(texture))->CopyDeviceToBitmap();
			}
			else
			{
				CHK_ONCE(0);
			}
		}
	}
	return 0.f;
}

float LXComponentMetaPatch::GetHeightAt(float x, float y) const
{
	if (Material)
	{
		float heightScale = 0.f;

		if (Material->GetFloatParameter(L"HeightScale", heightScale))
		{
			const LXTexture* texture = HeightMap.get();
			return GetHeightAt(x, y, texture, heightScale);
		}
		else
		{
			LogE(ComponentMetaPatch, L"Missing \"HeightScale\" material node in material %s.", Material->GetName().GetBuffer());
		}
	}

	return 0.f;
}

void LXComponentMetaPatch::ComputeBBoxLocal()
{
	if (_BBoxLocal.IsValid())
		return;

	// Reset the BBox
	_BBoxLocal.Reset();

	// Update the PrimtiveInstance bounds 
	if (_mesh)
	{
		VectorPrimitiveInstances primitiveInstances;
		_mesh->GetAllPrimitives(primitiveInstances);

		for (const std::shared_ptr<LXPrimitiveInstance>& primitiveInstance : primitiveInstances)
		{
			//primitiveInstance->
		}
	}

	if (_mesh)
	{
		const LXBBox& BBoxMesh = _mesh->GetBounds();

		if (BBoxMesh.IsValid())
			_BBoxLocal.Add(BBoxMesh);
	}

	if (!_BBoxLocal.IsValid())
	{
		LogD(ComponentMesh, L"Set the default BBox size for %s", GetName().GetBuffer());
		_BBoxLocal.Add(LX_VEC3F_XYZ_50);
		_BBoxLocal.Add(LX_VEC3F_NXYZ_50);
	}
}