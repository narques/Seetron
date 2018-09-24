//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "LXTerrain.h"
#include "LXPrimitive.h"
#include "LXMesh.h"
#include "LXMath.h"
#include "LXPrimitiveFactory.h"
#include "LXController.h"
#include "LXCore.h"
#include "LXMaterial.h"
#include "LXTexture.h"
#include "LXBitmap.h"
#include "LXMaterialNode.h"
#include "LXMemory.h" // --- Must be the last included ---

LXTerrain::LXTerrain(LXProject* pDocument) : 
	LXActorMesh(pDocument)
{
	SetName(L"Terrain");

	Mesh = new LXMesh(nullptr);

	_ExtendZ = KM2CM(0.5f);
	
	LXProperty::SetCurrentGroup(L"Terrain");
	
	auto PropPatchCount = DefineProperty("PatchCount", &PatchCount);
	PropPatchCount->SetMinMax(1, 100);
	PropPatchCount->SetLambdaOnChange([this](LXProperty* pProperty)
	{
		//Build();
	});

	auto PropQuadCount = DefinePropertyEnum(L"QuadCount", GetAutomaticPropertyID(), (uint*)&QuadCount);
	PropQuadCount->AddChoice(L"7", 7);
	PropQuadCount->AddChoice(L"31", 31);
	PropQuadCount->AddChoice(L"63", 63);
	PropQuadCount->AddChoice(L"127", 127);
	PropQuadCount->AddChoice(L"255", 255);
	PropQuadCount->SetLambdaOnChange([this](LXProperty* pProperty)
	{
		//Build();
	});

	auto PropQuadSize = DefineProperty("QuadSize", &QuadSize);
	PropQuadSize->SetLambdaOnChange([this](LXProperty* pProperty)
	{
		//Build();
	});

	auto PropOverallSize = DefineProperty("OverallSize", (float*)nullptr);
	PropOverallSize->SetPersistent(false);
	PropOverallSize->SetReadOnly(true);
	PropOverallSize->SetLambdaOnGet([this]
	{
		return CM2KM(PatchCount * (QuadCount + 1) * QuadSize);
	});

	LXPropertyAssetPtr* pPropMaterial = DefinePropertyAsset(L"Material", LXPropertyID::PRIMITIVE_MATERIAL, (LXAsset**)&Material);
	pPropMaterial->SetName(L"Material");
	pPropMaterial->SetLambdaOnChange([this](LXPropertyAssetPtr* PropertyAsset)
	{
		if (LXAsset* Asset = PropertyAsset->GetValue())
		{
			LXString Key = PropertyAsset->GetValue()->GetRelativeFilename();
			Mesh->SetMaterial(Key);
			//GetController()->MaterialChanged(this, dynamic_cast<LXMaterial*>(Asset));
			InvalidateRenderState();
		}
	});

	//auto PropertyHeightScale = DefineProperty("HeightScale", &HeightScale);
	
	Build();
}

/*virtual*/
LXTerrain::~LXTerrain(void)
{
	delete Mesh;
}

float LXTerrain::GetHeightAt(float x, float y) const
{
	if (Material)
	{
#if 0
		if (LXMaterialNodeTextureSampler* TextureSampler = Material->GetMaterialNodeTextureSamplerFromName(L"HeightMap"))
		{
			if (LXMaterialNodeFloat* MaterialNodeFloat = Material->GetMaterialNodeFloatFromName(L"HeightScale"))
			{
				LXTexture* Texture = TextureSampler->GetTexture();
				if (LXBitmap* Bitmap = Texture->GetBitmap(0))
				{
					x = (float)x * (float)Texture->GetWidth() / 409600.f/*GetOverallSize()*/;
					y = (float)y * (float)Texture->GetHeight() / 409600.f/*GetOverallSize()*/;

					CHK(x >= 0.f);
					CHK(y >= 0.f);
					CHK(x < 4096.f);
					CHK(y < 4096.f);
					
					unsigned short b = Bitmap->GetPixel<unsigned short>((int)x, (int)y)[0];
					float r = (float)(b) / USHRT_MAX * MaterialNodeFloat->Value;
					return r;
				}
			}
			else
			{
				LogE(Terrain, L"Missing \"HeightScale\" float material node in material %s.", Material->GetName().GetBuffer());
			}
		}
		else
#endif
		{
			LogE(Terrain, L"Missing \"HeightMap\" texture material node in material %s.", Material->GetName().GetBuffer());
		}
	}
	
	return 0.f;
}

// float LXTerrain::GetHeightScale() const 
// {
// // 	if (Material)
// // 	{
// // 		if (LXPropertyFloat* Property = dynamic_cast<LXPropertyFloat*>(Material->GetProperty(L"HeightScale")))
// // 		{
// // 			return Property->GetValue();
// // 		}
// // 	}
// 
// 	return 1.f;
// }

void LXTerrain::Build()
{
	const shared_ptr<LXPrimitive>& Primitive = GetPrimitiveFactory()->CreateTerrainPatch(QuadCount, (int)QuadSize);
	Primitive->SetPersistent(false);

	//Primitive->SetMaterial(L"Materials/M_TerrainDefault.smat");
	//Material = Primitive->GetMaterial();
// 	Material->RegisterCB_OnPropertyChanged(this, [this](LXSmartObject*, LXProperty* Property) 
// 	{
// 		if (Property->GetName() == L"HeightScale")
// 		{
// 			InvalidatePrimitiveBounds();
// 		}
// 	});
	

// Patch
	
	float PatchSize = QuadSize * QuadCount;
	
#if _DEBUG
	PatchCount = 2;
#endif

	for (int l = 0; l < PatchCount; l++)
	{
		for (int k = 0; k < PatchCount; k++)
		{
			float PatchOffsetX = k * PatchSize;
			float PatchOffsetY = l * PatchSize;
			LXMatrix Matrix;
			Matrix.SetOrigin(PatchOffsetX, PatchOffsetY, 0.f);
			AddPrimitive(Primitive, &Matrix);
		}
	}

//	InvalidateRenderState();
}

// void LXTerrain::BuildContent()
// {
// 	//
// 	// Grass
// 	//
// 
// 	LXPrimitive* GrassPrimitive = GetPrimitiveFactory()->CreateGrassPatch();
// 	// 	for (int i = 0; i < 63; i++)
// 	// 	{
// 	// 		for (int j = 0; j < 63; j++)
// 	// 		{
// 	// 			float x = i * 100.f;
// 	// 			float y = j * 100.f;
// 	// 			float z = pTerrain->GetHeightAt(x, y);
// 	// 			LXActorMesh* ActorMesh = new LXActorMesh;
// 	// 			ActorMesh->AddPrimitive(GrassPrimitive);
// 	// 			LXMatrix Translation;
// 	// 			Translation.SetTranslation(x, y, z);
// 	// 			ActorMesh->SetMatrix(Translation);
// 	// 			m_pScene->AddChild(ActorMesh);
// 	// 		}
// 	// 	}
// 
// 	//
// 	// Trees
// 	//
// 
// 	const int MaxTrees = 0;
// 	for (int i = 0; i < MaxTrees; i++)
// 	{
// 		float x = RandomFloat() * GetOverallSize();
// 		float y = RandomFloat() * GetOverallSize();
// 		float z = GetHeightAt(x, y);
// 
// 		/*
// 		auto* Tree = new LXActorMeshCube(this);
// 		Tree->SetName(L"Tree" + LXString::Number(++i));
// 		*/
// 
// 		LXActorMesh* ActorMesh = new LXActorMesh;
// 		ActorMesh->AddPrimitive(GrassPrimitive);
// 		ActorMesh->SetOrigin(vec3f(x, y, z));
// 		AddChild(ActorMesh);
// 	}
// 
// 	//LXTree* Tree = new LXTree;
// 	//m_pScene->AddChild(Tree);
// 
// }
// 
