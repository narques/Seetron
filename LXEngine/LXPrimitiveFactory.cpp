//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#include "pch.h"
#include "LXPrimitiveFactory.h"
#include "LXPrimitive.h"
#include "LXMatrix.h"
#include "LXMath.h"

void InterpolateSmooth2(const float& vStart, const float& vEnd, float t, float& vResult)
{
	CHK(t >= 0.0f && t <= 1.0f);
	t = (1.0f - cosf(LX_PI * t * t * t)) * 0.5f;
	vResult = vStart + (vEnd - vStart) * t;
}

void InterpolateSmooth3(const float& vStart, const float& vEnd, float t, float& vResult)
{
	CHK(t >= 0.0f && t <= 1.0f);
	t = sinf(LX_PI_2 * t);
	vResult = vStart + (vEnd - vStart) * t;
}

LXPrimitiveFactory::LXPrimitiveFactory()
{
	_PrimitiveWireFrameCube = CreateWireframeCube(1.f, 1.f, 1.f, true);
	// By default, the WireFrame primitives are rendered with a yellow unlit material/
	_PrimitiveWireFrameCube->SetMaterial(L"Materials/M_BBox.smat");
	
	_PlaneXZ = CreateQuadXZ(1.f,  1.f, true);
}

LXPrimitiveFactory::~LXPrimitiveFactory()
{
	DeleteAllPrimitives();
}

void LXPrimitiveFactory::DeleteAllPrimitives()
{
	ListPrimitives.clear();
}

const std::shared_ptr<LXPrimitive>& LXPrimitiveFactory::CreateSSTriangle()
{
	const std::shared_ptr<LXPrimitive>& p = CreatePrimitive();
	
	p->SetName(L"SSTriangle");
	p->SetTopology(LX_TRIANGLES);
	
	p->GetArrayPositions().push_back(vec3f(-1.0f, 3.0f, 0.5f));
	p->GetArrayPositions().push_back(vec3f(-1.0f, -1.0f, 0.5f));
	p->GetArrayPositions().push_back(vec3f(3.0f, -1.0f, 0.5f));
	
	p->GetArrayTexCoords().push_back(vec2f(0.f, -1.f));
	p->GetArrayTexCoords().push_back(vec2f(0.f, 1.f));
	p->GetArrayTexCoords().push_back(vec2f(2.f, 1.f));
	
	return p;
}

const std::shared_ptr<LXPrimitive>& LXPrimitiveFactory::CreateCone(float radius, float height)
{
	int slices = 8; // Min : 3
	int stacks = 2; // Min : 2 

	const std::shared_ptr<LXPrimitive>& p = CreatePrimitive();
	p->SetName(L"Cone");
	p->SetTopology(LX_TRIANGLES);

	for (int i = 0; i < stacks; i++)
	{

		for (int j = 0; j < slices; j++)
		{
			float angle = LX_2PI * (float)j / (float)slices;
			float cos1 = cos(angle);
			float sin1 = sin(angle);

			vec3f vertex;
			if (i == 0)
				vertex = vec3f(0.f, 0.f, i * (height / (stacks - 1)));
			else
				vertex = vec3f(radius*cos(angle), -radius * sin(angle), i * (-height / (stacks - 1)));

			p->GetArrayPositions().push_back(vertex);
			p->GetArrayNormals().push_back(vec3f(cos(angle), sin(angle), 0.f));
			vec2f TextCoord = (vec2f(vertex.x / radius, vertex.y / radius) + 1.f) * 0.5;
			p->GetArrayTexCoords().push_back(TextCoord);
		}
	}

	// Indices
	for (int i = 0; i < (stacks - 1); i++)
	{

		for (int j = 0; j < slices; j++)
		{
			if (j == (slices - 1))
			{
				p->GetArrayIndices().push_back(j + (i * slices));
				p->GetArrayIndices().push_back((i * slices));
				p->GetArrayIndices().push_back(j + ((i + 1) * slices));

				p->GetArrayIndices().push_back(j + ((i + 1) * slices));
				p->GetArrayIndices().push_back((i * slices));
				p->GetArrayIndices().push_back((i + 1) * slices);

			}
			else
			{
				p->GetArrayIndices().push_back(j + (i * slices));
				p->GetArrayIndices().push_back(j + (i * slices) + 1);
				p->GetArrayIndices().push_back(j + ((i + 1) * slices));

				p->GetArrayIndices().push_back(j + ((i + 1) * slices));
				p->GetArrayIndices().push_back(j + (i * slices) + 1);
				p->GetArrayIndices().push_back(j + ((i + 1) * slices) + 1);
			}
		}
	}

	return p;
}

const std::shared_ptr<LXPrimitive>& LXPrimitiveFactory::CreateCube(float x, float y, float z, bool bCentered)
{
	const std::shared_ptr<LXPrimitive>& p = CreatePrimitive();
	p->SetTopology(LX_TRIANGLES);
	p->SetName(L"Cube");

	float nx, ny, nz, px, py, pz; // n means negative, not normal

	if (bCentered)
	{
		nx = -x * 0.5f;
		ny = -y * 0.5f;
		nz = -z * 0.5f;
		px = x * 0.5f;
		py = y * 0.5f;
		pz = z * 0.5f;
	}
	else
	{
		nx = 0.f;
		ny = 0.f;
		nz = 0.f;
		px = x;
		py = y;
		pz = z;
	}

	const vec3f v0(nx, ny, nz);
	const vec3f v1(nx, ny, pz);
	const vec3f v2(px, ny, pz);
	const vec3f v3(px, ny, nz);
	
	const vec3f v4(nx, py, nz);
	const vec3f v5(nx, py, pz);
	const vec3f v6(px, py, pz);
	const vec3f v7(px, py, nz);
	 
	const vec2f t0(0.f, 0.f);
	const vec2f t1(0.f, 1.f);
	const vec2f t2(1.f, 1.f);
	const vec2f t3(1.f, 0.f);

	// --- Vertices ---

	// Y-

	p->GetArrayPositions().push_back(v0);
	p->GetArrayPositions().push_back(v1);
	p->GetArrayPositions().push_back(v2);
	p->GetArrayPositions().push_back(v3);

	p->GetArrayTexCoords().push_back(t2);
	p->GetArrayTexCoords().push_back(t3);
	p->GetArrayTexCoords().push_back(t0);
	p->GetArrayTexCoords().push_back(t1);

	// Y+

	p->GetArrayPositions().push_back(v7);
	p->GetArrayPositions().push_back(v6);
	p->GetArrayPositions().push_back(v5);
	p->GetArrayPositions().push_back(v4);

	p->GetArrayTexCoords().push_back(t2);
	p->GetArrayTexCoords().push_back(t3);
	p->GetArrayTexCoords().push_back(t0);
	p->GetArrayTexCoords().push_back(t1);

	// X-

	p->GetArrayPositions().push_back(v4);
	p->GetArrayPositions().push_back(v5);
	p->GetArrayPositions().push_back(v1);
	p->GetArrayPositions().push_back(v0);

	p->GetArrayTexCoords().push_back(t2);
	p->GetArrayTexCoords().push_back(t3);
	p->GetArrayTexCoords().push_back(t0);
	p->GetArrayTexCoords().push_back(t1);

	// X+

	p->GetArrayPositions().push_back(v3);
	p->GetArrayPositions().push_back(v2);
	p->GetArrayPositions().push_back(v6);
	p->GetArrayPositions().push_back(v7);

	p->GetArrayTexCoords().push_back(t2);
	p->GetArrayTexCoords().push_back(t3);
	p->GetArrayTexCoords().push_back(t0);
	p->GetArrayTexCoords().push_back(t1);

	// Z-

	p->GetArrayPositions().push_back(v4);
	p->GetArrayPositions().push_back(v0);
	p->GetArrayPositions().push_back(v3);
	p->GetArrayPositions().push_back(v7);

	p->GetArrayTexCoords().push_back(t2);
	p->GetArrayTexCoords().push_back(t3);
	p->GetArrayTexCoords().push_back(t0);
	p->GetArrayTexCoords().push_back(t1);

	// Z+

	p->GetArrayPositions().push_back(v6);
	p->GetArrayPositions().push_back(v2);
	p->GetArrayPositions().push_back(v1);
	p->GetArrayPositions().push_back(v5);

	p->GetArrayTexCoords().push_back(t2);
	p->GetArrayTexCoords().push_back(t3);
	p->GetArrayTexCoords().push_back(t0);
	p->GetArrayTexCoords().push_back(t1);

	// --- Indices ---

	for (int i = 0, offset = 0; i < 6; i++)
	{
		p->GetArrayIndices().push_back(0 + offset);
		p->GetArrayIndices().push_back(1 + offset);
		p->GetArrayIndices().push_back(3 + offset);
		p->GetArrayIndices().push_back(3 + offset);
		p->GetArrayIndices().push_back(1 + offset);
		p->GetArrayIndices().push_back(2 + offset);
		offset += 4;
	}

	return p;
}

const std::shared_ptr<LXPrimitive>& LXPrimitiveFactory::CreatePrimitive()
{
	ListPrimitives.push_back(std::make_shared<LXPrimitive>());
	return ListPrimitives.back();
}

const std::shared_ptr<LXPrimitive>& LXPrimitiveFactory::CreateTube(float innerRadius, float radius, const LXMatrix* Matrix /* = nullptr */)
{
	const int slices = 48;	 // Min : 3
	//const int stacks = 2;	 // Min : 2 
	//const float height = 1.f;

	const std::shared_ptr<LXPrimitive>& p = CreatePrimitive();
	p->SetTopology(LX_TRIANGLES);
	p->SetName(L"Tube");

	vec3f PreviousInner;
	vec3f Previous;

	for (int j = 0; j < (slices + 1); j++)
	{
		float angle = LX_2PI * (float)j / (float)slices;
		float cos1 = cos(angle);
		float sin1 = sin(angle);

		/*
		vec3f vertex(radius*cos(angle), -radius * sin(angle), i * (height / (stacks - 1)));
		p->GetArrayPositions().push_back(vertex);
		p->GetArrayNormals().push_back(vec3f(cos(angle), sin(angle), 0.f));
		vec2f TextCoord = (vec2f(vertex.x / radius, vertex.y / radius) + 1.f) * 0.5;
		p->GetArrayTexCoords().push_back(TextCoord);
		*/

		vec3f vertex(radius * cos(angle), radius * sin(angle), 0.f);
		vec3f vertexInner(innerRadius * cos(angle), innerRadius * sin(angle), 0.f);

		if (Matrix)
		{
			Matrix->LocalToParentPoint(vertex);
			Matrix->LocalToParentPoint(vertexInner);
		}

		if (j > 0)
		{
			p->GetArrayPositions().push_back(Previous);
			p->GetArrayPositions().push_back(PreviousInner);
			p->GetArrayPositions().push_back(vertex);

			p->GetArrayPositions().push_back(vertex);
			p->GetArrayPositions().push_back(PreviousInner);
			p->GetArrayPositions().push_back(vertexInner);
		}

		Previous = vertex;
		PreviousInner = vertexInner;
	}

	

	/*

	for (int i = 0; i < stacks; i++)
	{

		for (int j = 0; j < slices; j++)
		{
			float angle = LX_2PI * (float)j / (float)slices;
			float cos1 = cos(angle);
			float sin1 = sin(angle);

			vec3f vertex(radius*cos(angle), -radius * sin(angle), i * (height / (stacks - 1)));
			p->GetArrayPositions().push_back(vertex);
			p->GetArrayNormals().push_back(vec3f(cos(angle), sin(angle), 0.f));
			vec2f TextCoord = (vec2f(vertex.x / radius, vertex.y / radius) + 1.f) * 0.5;
			p->GetArrayTexCoords().push_back(TextCoord);
		}
	}

	*/





	// Indices
	/*
	for (int i = 0; i < (stacks - 1); i++)
	{

		for (int j = 0; j < slices; j++)
		{
			if (j == (slices - 1))
			{
				p->GetArrayIndices().push_back(j + (i * slices));
				p->GetArrayIndices().push_back((i * slices));
				p->GetArrayIndices().push_back(j + ((i + 1) * slices));

				p->GetArrayIndices().push_back(j + ((i + 1) * slices));
				p->GetArrayIndices().push_back((i * slices));
				p->GetArrayIndices().push_back((i + 1) * slices);

			}
			else
			{
				p->GetArrayIndices().push_back(j + (i * slices));
				p->GetArrayIndices().push_back(j + (i * slices) + 1);
				p->GetArrayIndices().push_back(j + ((i + 1) * slices));

				p->GetArrayIndices().push_back(j + ((i + 1) * slices));
				p->GetArrayIndices().push_back(j + (i * slices) + 1);
				p->GetArrayIndices().push_back(j + ((i + 1) * slices) + 1);
			}
		}
	}
	*/

	return p;
}

const std::shared_ptr<LXPrimitive>& LXPrimitiveFactory::CreateTerrainPatch(int QuadCount, int QuadSize)
{
	// Uncomment to debug patches : the last line is omitted to separated them
	//if (QuadCount > 1)
		//QuadCount--;

	CHK(QuadCount > 0);
	CHK(QuadSize > 0);

	int VertexCount = (QuadCount + 1) * (QuadCount + 1);
	int IndexCount = QuadCount * QuadCount * 6;

	const std::shared_ptr<LXPrimitive>& Primitive = CreatePrimitive();

	Primitive->SetPersistent(false);
	Primitive->SetName(L"TerrainPatch");
	Primitive->GetArrayPositions().reserve(VertexCount);
	Primitive->GetArrayNormals().reserve(VertexCount);
	Primitive->GetArrayTexCoords().reserve(VertexCount);
	Primitive->GetArrayIndices().reserve(IndexCount);

	const float uvStep = 1.f / QuadCount;

	// Vertices
	for (int j = 0; j < (QuadCount + 1); j++)
	{
		for (int i = 0; i < (QuadCount + 1); i++)
		{
			float x = (float)(QuadSize * i);
			float y = (float)(QuadSize * j);
			float z = 0.f;
			Primitive->GetArrayPositions().push_back(vec3f(x, y, z));
			Primitive->GetArrayNormals().push_back(vec3f(0.f, 0.f, 1.f));

			float u = i * uvStep;
			float v = j * uvStep;

			Primitive->GetArrayTexCoords().push_back(vec2f(u, v));
		}
	}

	// Indices 
	for (int j = 0; j < QuadCount; j++)
	{
		for (int i = 0; i < QuadCount; i++)
		{
			int a = i + ((j + 1) * (QuadCount + 1));
			int d = i + ((j + 1) * (QuadCount + 1)) + 1;
			int b = i + (j * (QuadCount + 1));
			int c = i + (j * (QuadCount + 1)) + 1;

			Primitive->GetArrayIndices().push_back(a);
			Primitive->GetArrayIndices().push_back(d);
			Primitive->GetArrayIndices().push_back(b);

			Primitive->GetArrayIndices().push_back(b);
			Primitive->GetArrayIndices().push_back(d);
			Primitive->GetArrayIndices().push_back(c);
		}
	}

	Primitive->ComputeTangents();

	return Primitive;
}

const std::shared_ptr<LXPrimitive>& LXPrimitiveFactory::CreateCylinder(float radius, float height)
{
	int slices = 8; // Min : 3
	int stacks = 3; // Min : 2 

	const std::shared_ptr<LXPrimitive>& p = CreatePrimitive();
	p->SetName(L"Cylinder");
	p->SetTopology(LX_TRIANGLES);

	for (int i = 0; i < stacks; i++)
	{

		for (int j = 0; j < slices; j++)
		{
			float angle = LX_2PI * (float)j / (float)slices;
			float cos1 = cos(angle);
			float sin1 = sin(angle);

			vec3f vertex(radius*cos(angle), -radius*sin(angle), i * (height / (stacks - 1)));
			p->GetArrayPositions().push_back(vertex);
			p->GetArrayNormals().push_back(vec3f(cos(angle), sin(angle), 0.f));
			vec2f TextCoord = (vec2f(vertex.x / radius, vertex.y / radius) + 1.f) * 0.5;
			p->GetArrayTexCoords().push_back(TextCoord);


		}
	}

	// Indices
	for (int i = 0; i < (stacks - 1); i++)
	{

		for (int j = 0; j < slices; j++)
		{
			if (j == (slices - 1))
			{
				p->GetArrayIndices().push_back(j + (i * slices));
				p->GetArrayIndices().push_back((i * slices));
				p->GetArrayIndices().push_back(j + ((i + 1) * slices));

				p->GetArrayIndices().push_back(j + ((i + 1) * slices));
				p->GetArrayIndices().push_back((i * slices));
				p->GetArrayIndices().push_back((i + 1) * slices);

			}
			else
			{
				p->GetArrayIndices().push_back(j + (i * slices));
				p->GetArrayIndices().push_back(j + (i * slices) + 1);
				p->GetArrayIndices().push_back(j + ((i + 1) * slices));

				p->GetArrayIndices().push_back(j + ((i + 1) * slices));
				p->GetArrayIndices().push_back(j + (i * slices) + 1);
				p->GetArrayIndices().push_back(j + ((i + 1) * slices) + 1);
			}
		}
	}

	return p;
}

const std::shared_ptr<LXPrimitive>& LXPrimitiveFactory::CreateGrassPatch()
{
	const std::shared_ptr<LXPrimitive>& p = CreatePrimitive();
	p->SetName(L"GrassPatch");
	float m_fGrassHeight = 20.f;
	float m_fGrassWidth = 1.f;
	int m_nBladeSteps = 4;
	float m_fPatchWidth = 100.f;

	srand(59654);
	 
	//if (!m_pBlades)
	//{
	// 		LXMaterial* pMaterial = dynamic_cast<LXMaterial*>(const_cast<LXDocument*>(m_pDocument)->GetMaterialManager().GetObjectFromFile(L"sysGrass", false));
	// 		pMaterial->SetName(L"sysGrass");
	// 		pMaterial->SetSystem(true);
	// 		pMaterial->SetTwoSided(true);
	// 		pMaterial->SetTexture2(GetSettings().GetTexturesFolder() + L"grass\\texture-98x700.jpg");
	// 		pMaterial->SetDiffuse(WHITE);
	// 		m_pBlades = new LXTerrainLODPrimitive(this);
	// 		m_pBlades->SetMaterial(pMaterial);	
	// 		AddDrawable(m_pBlades);
	// 	}
	// 
	// 	if (!m_pHBlades)
	// 	{
	// 		m_pHBlades = new LXPrimitive();
	// 		m_pHBlades->SetMaterial(m_pBlades->GetMaterial());
	// 		AddDrawable(m_pHBlades);
	// 	}
	// 
	// 	//for(uint nBlade=0; nBlade<m_nBlades; nBlade++)
	// 	
	uint nBlade = 0;			// Grid positioning
	for (int j = 0; j < 40; j++)		// Grid positioning
	{
		for (int k = 0; k < 40; k++)   // Grid positioning
		{
			// Blade topology 
			const float fHeight = m_fGrassHeight;// -RandomFloat() * 100.f;
			const float fWidth = m_fGrassWidth;
			const int nSteps = m_nBladeSteps;

			//float posx = RandomInt(m_fPatchWidth) - (m_fPatchWidth * 0.5f);
			//float posy = RandomInt(m_fPatchWidth) - (m_fPatchWidth * 0.5f);

			const float toto = 100.0 / 40.0;  // Grid positioning
			float posx = ((float)j * toto + RandomInt((int)toto)) - (m_fPatchWidth * 0.5f); // Grid positioning
			float posy = ((float)k * toto + RandomInt((int)toto)) - (m_fPatchWidth * 0.5f); // Grid positioning

			// Tools
			int nVertices = nSteps * 2 + 1;
			float hw = fWidth * 0.5f;			// Half width
			float sh = fHeight / nSteps;		// Step height

			vec3f* v = new vec3f[nVertices];
			vec3f* v2 = new vec3f[nVertices];

			//
			// Sample with 3 steps
			// 
			// 0  2  4
			// -------
			// | /| /|\
			// |/ |/ |/ 6     --> z   
			// -------
			// 1  3  5
			//

			LXMatrix matBlade, matBlade2, matBladePosition, matBladeBent, matBladeRotation;

			// Position du brin
			matBladePosition.SetOrigin(posx, posy, 0.f);

			// Rotation du brin autour de z
			matBladeRotation.SetRotation(RandomFloat() * LX_PI * 2.f, 0.f, 0.f, 1.f);

			matBlade = matBladePosition * matBladeRotation;
			matBlade2 = matBlade;

			// Inclinaison
			float fBlend = RandomFloat() * 45.0f;

			for (int i = 0; i < nSteps; i++)
			{
				float he = sh * i;

				float falloff = 0.f;
				//falloff = hw * he / fHeight;
				InterpolateSmooth2(0.f, hw, he / fHeight, falloff);

				v[i * 2].Set(-hw + falloff, 0.f, he);
				v[i * 2 + 1].Set(hw - falloff, 0.f, he);

				// 				if (m_pHBlades)
				// 				{
				// 					v2[i * 2].Set(-hw + falloff, he, 0.f);
				// 					v2[i * 2 + 1].Set(hw - falloff, he, 0.f);
				// 				}

				matBlade.LocalToParentPoint(v[i * 2]);
				matBlade.LocalToParentPoint(v[i * 2 + 1]);

				// 				if (m_pHBlades)
				// 				{
				// 					matBlade2.LocalToParentPoint(v2[i*2]);
				// 					matBlade2.LocalToParentPoint(v2[i*2+1]);
				// 				}

				matBladeBent.SetRotation(LX_DEGTORAD(fBlend * (i / (float)nSteps)), 1.0f, 1.0f, 0.0f);

				matBlade = matBladePosition * matBladeRotation * matBladeBent;
				matBlade2 = matBladePosition * matBladeRotation;

				// Position
				p->GetArrayPositions().push_back(v[i * 2]);
				p->GetArrayPositions().push_back(v[i * 2 + 1]);

				// 				if (m_pHBlades)
				// 				{
				// 					m_pHBlades->GetArrayPositions().push_back(v2[i * 2]);
				// 					m_pHBlades->GetArrayPositions().push_back(v2[i * 2 + 1]);
				// 				}

								// TexCoord
				p->GetArrayTexCoords().push_back(vec2f(0.f, he / fHeight));
				p->GetArrayTexCoords().push_back(vec2f(1.f, he / fHeight));

				// 				if (m_pHBlades)
				// 				{
				// 					m_pHBlades->GetArrayTexCoords().push_back(vec2f(0.f, he / fHeight));
				// 					m_pHBlades->GetArrayTexCoords().push_back(vec2f(1.f, he / fHeight));
				// 				}
			}

			v[nVertices - 1].Set(0.f, 0.f, fHeight);
			matBlade.LocalToParentPoint(v[nVertices - 1]);

			// 		if (m_pHBlades)
			// 		{
			// 			v2[nVertices - 1].Set(0.f, fHeight, 0.f);
			// 			matBlade2.LocalToParentPoint(v2[nVertices - 1]);
			// 		}

			p->GetArrayPositions().push_back(v[nVertices - 1]);
			p->GetArrayTexCoords().push_back(vec2f(0.5f, 1.f));

			// 		if (m_pHBlades)
			// 		{
			// 			m_pHBlades->GetArrayPositions().push_back(v2[nVertices - 1]);
			// 			m_pHBlades->GetArrayTexCoords().push_back(vec2f(0.5f, 1.f));
			// 		}

			int offset = nVertices * nBlade;

			nBlade++; // Grid positioning

			int j = 0;
			const int nQuads = nSteps - 1;
			for (int i = 0; i < nQuads; i++)
			{
				p->GetArrayIndices().push_back(offset + j);
				p->GetArrayIndices().push_back(offset + j + 1);
				p->GetArrayIndices().push_back(offset + j + 2);

				p->GetArrayIndices().push_back(offset + j + 2);
				p->GetArrayIndices().push_back(offset + j + 1);
				p->GetArrayIndices().push_back(offset + j + 3);

				// 			if (m_pHBlades)
				// 			{
				// 				m_pHBlades->GetArrayIndices().push_back(offset + j);
				// 				m_pHBlades->GetArrayIndices().push_back(offset + j + 1);
				// 				m_pHBlades->GetArrayIndices().push_back(offset + j + 2);
				// 
				// 				m_pHBlades->GetArrayIndices().push_back(offset + j + 2);
				// 				m_pHBlades->GetArrayIndices().push_back(offset + j + 1);
				// 				m_pHBlades->GetArrayIndices().push_back(offset + j + 3);
				// 			}

				j += 2;
			}

			// Last Triangle
			p->GetArrayIndices().push_back(offset + j);
			p->GetArrayIndices().push_back(offset + j + 1);
			p->GetArrayIndices().push_back(offset + j + 2);

			// 		if (m_pHBlades)
			// 		{
			// 			m_pHBlades->GetArrayIndices().push_back(offset + j);
			// 			m_pHBlades->GetArrayIndices().push_back(offset + j + 1);
			// 			m_pHBlades->GetArrayIndices().push_back(offset + j + 2);
			// 		}

			delete[] v;
			delete[] v2;
		}
	}

	p->ComputeNormals();
	p->ComputeTangents();

	// 	if (m_pHBlades)
	// 	{
	// 		m_pHBlades->ComputeNormals();
	// 		m_pHBlades->ComputeTangents();
	// 	}
	
	return p;
}

const std::shared_ptr<LXPrimitive>& LXPrimitiveFactory::CreateWireframeCube(float SizeX, float SizeY, float SizeZ, bool bCentered /*= true*/)
{
	float nx, ny, nz, px, py, pz; // n means negative, not normal

	if (bCentered)
	{
		nx = -SizeX * 0.5f;
		ny = -SizeY * 0.5f;
		nz = -SizeZ * 0.5f;
		px = SizeX * 0.5f;
		py = SizeY * 0.5f;
		pz = SizeZ * 0.5f;
	}
	else
	{
		nx = 0.f;
		ny = 0.f;
		nz = 0.f;
		px = SizeX;
		py = SizeY;
		pz = SizeZ;
	}

	return CreateWireframeCube(nx, ny, nz, px, py, pz);
}

const std::shared_ptr<LXPrimitive>& LXPrimitiveFactory::CreateWireframeCube(float nx, float ny, float nz, float px, float py, float pz)
{
	const std::shared_ptr<LXPrimitive>& p = CreatePrimitive();
	p->SetName(L"WireFrameCube");
	p->SetTopology(LX_LINES);

	const vec3f v0(nx, ny, nz);
	const vec3f v1(nx, ny, pz);
	const vec3f v2(px, ny, pz);
	const vec3f v3(px, ny, nz);

	const vec3f v4(nx, py, nz);
	const vec3f v5(nx, py, pz);
	const vec3f v6(px, py, pz);
	const vec3f v7(px, py, nz);

	// Z-

	p->GetArrayPositions().push_back(v0);
	p->GetArrayPositions().push_back(v3);

	p->GetArrayPositions().push_back(v3);
	p->GetArrayPositions().push_back(v7);

	p->GetArrayPositions().push_back(v7);
	p->GetArrayPositions().push_back(v4);

	p->GetArrayPositions().push_back(v4);
	p->GetArrayPositions().push_back(v0);

	// Z+

	p->GetArrayPositions().push_back(v1);
	p->GetArrayPositions().push_back(v2);

	p->GetArrayPositions().push_back(v2);
	p->GetArrayPositions().push_back(v6);

	p->GetArrayPositions().push_back(v6);
	p->GetArrayPositions().push_back(v5);

	p->GetArrayPositions().push_back(v5);
	p->GetArrayPositions().push_back(v1);

	// Z- to Z+

	p->GetArrayPositions().push_back(v1);
	p->GetArrayPositions().push_back(v0);

	p->GetArrayPositions().push_back(v2);
	p->GetArrayPositions().push_back(v3);

	p->GetArrayPositions().push_back(v6);
	p->GetArrayPositions().push_back(v7);

	p->GetArrayPositions().push_back(v5);
	p->GetArrayPositions().push_back(v4);

	p->SetMaterial(L"Materials/M_BBox.smat");

	return p;
}

const std::shared_ptr<LXPrimitive>& LXPrimitiveFactory::CreateQuadXY(float SizeX, float SizeY, bool Centered)
{
	const std::shared_ptr<LXPrimitive>& p = CreatePrimitive();
	p->SetName(L"QuadXY");
	p->SetTopology(LX_TRIANGLE_STRIP);


	float nx, ny, nz, px, py, pz; // n means negative, not normal

	if (Centered)
	{
		nx = -SizeX * 0.5f;
		ny = -SizeY * 0.5f;
		nz = 0.f;// -SizeZ * 0.5f;
		px = SizeX * 0.5f;
		py = SizeY * 0.5f;
		pz = 0.f;// SizeZ * 0.5f;
	}
	else
	{
		nx = 0.f;
		ny = 0.f;
		nz = 0.f;
		px = SizeX;
		py = SizeY;
		pz = 0.0;// SizeZ;
	}
	
	const vec3f v1(nx, ny, pz);
	const vec3f v2(px, ny, pz);
	const vec3f v3(nx, py, nz);
	const vec3f v4(px, py, nz);

	const vec3f vn(0.0f, 0.0f, 1.0f);

	vec2f t1(0.0f, 0.0f);
	vec2f t2(0.0f, 1.0f);
	vec2f t3(1.0f, 0.0f);
	vec2f t4(1.0f, 1.0f);

	p->GetArrayPositions().push_back(v1);
	p->GetArrayPositions().push_back(v3);
	p->GetArrayPositions().push_back(v2);
	p->GetArrayPositions().push_back(v4);

	p->GetArrayNormals().push_back(vn);
	p->GetArrayNormals().push_back(vn);
	p->GetArrayNormals().push_back(vn);
	p->GetArrayNormals().push_back(vn);

	p->GetArrayTexCoords().push_back(t1);
	p->GetArrayTexCoords().push_back(t2);
	p->GetArrayTexCoords().push_back(t3);
	p->GetArrayTexCoords().push_back(t4);

	return p;
}

const std::shared_ptr<LXPrimitive>& LXPrimitiveFactory::CreateQuadXZ(float SizeX, float SizeZ, bool Centered)
{
	const std::shared_ptr<LXPrimitive>& p = CreatePrimitive();
	p->SetName(L"QuadXZ");
	p->SetTopology(LX_TRIANGLE_STRIP);
	
	float nx, ny, nz, px, py, pz; // n means negative, not normal

	if (Centered)
	{
		nx = -SizeX * 0.5f;
		ny = 0.f;//-SizeY * 0.5f;
		nz = -SizeZ * 0.5f;
		px = SizeX * 0.5f;
		py = 0.f;// SizeY * 0.5f;
		pz = SizeZ * 0.5f;
	}
	else
	{
		nx = 0.f;
		ny = 0.f;
		nz = 0.f;
		px = SizeX;
		py = 0.f;// SizeY;
		pz = SizeZ;
	}
	
	const vec3f v1(nx, ny, pz);
	const vec3f v2(px, ny, pz);
	const vec3f v3(nx, ny, nz);
	const vec3f v4(px, ny, nz);

	const vec3f vn(0.0f, 1.0f, 0.0f);

	vec2f t1(0.0f, 0.0f);
	vec2f t2(0.0f, 1.0f);
	vec2f t3(1.0f, 0.0f);
	vec2f t4(1.0f, 1.0f);

	p->GetArrayPositions().push_back(v1);
	p->GetArrayPositions().push_back(v3);
	p->GetArrayPositions().push_back(v2);
	p->GetArrayPositions().push_back(v4);

	p->GetArrayNormals().push_back(vn);
	p->GetArrayNormals().push_back(vn);
	p->GetArrayNormals().push_back(vn);
	p->GetArrayNormals().push_back(vn);

	p->GetArrayTexCoords().push_back(t1);
	p->GetArrayTexCoords().push_back(t2);
	p->GetArrayTexCoords().push_back(t3);
	p->GetArrayTexCoords().push_back(t4);

	return p;
}

const std::shared_ptr<LXPrimitive>& LXPrimitiveFactory::CreateSphere(float radius)
{
	int slices = 64; // Min 3
	int stacks = 32; // Min 2

	const std::shared_ptr<LXPrimitive>& p = CreatePrimitive();
	p->SetName(L"Sphere");
	p->SetTopology(LX_TRIANGLES);

	// Top
	p->GetArrayPositions().push_back(vec3f(0.f, 0.f, radius));
	p->GetArrayNormals().push_back(vec3f(0.f, 0.f, 1.f));
	p->GetArrayTexCoords().push_back(vec2f(0.f, 1.f));

	for (int i = 1; i < stacks; i++)
	{
		float angle2 = LX_PI * (0.5f + (float)i / ((float)(stacks)));
		float cos2 = cos(angle2);
		float sin2 = sin(angle2);

		for (int j = 0; j<slices; j++)
		{
			float angle1 = LX_2PI * (float)j / (float)slices;
			float cos1 = cos(angle1);
			float sin1 = sin(angle1);

			float x = cos1 * cos2;
			float y = sin1 * cos2;
			float z = sin2;

			p->GetArrayPositions().push_back(vec3f(x*radius, y*radius, z*radius));
			p->GetArrayNormals().push_back(vec3f(x, y, z));
			
			float v =  (j * (LX_2PI / slices)) / LX_2PI;
			float u = (i * (LX_PI / stacks)) / LX_PI;

			p->GetArrayTexCoords().push_back(vec2f(u, v));
		}
	}

	// bottom
	p->GetArrayPositions().push_back(vec3f(0.f, 0.f, -radius));
	p->GetArrayNormals().push_back(vec3f(0.f, 0.f, -1.f));
	p->GetArrayTexCoords().push_back(vec2f(0.f, 0.f));

	//
	// Indices
	//

	for (int i = 0; i < slices; i++)
	{
		const int a = 0;
		int b = (i == slices - 1)? 1 : i + 2;
		int c = i + 1;
		p->GetArrayIndices().push_back(a);
		p->GetArrayIndices().push_back(b);
		p->GetArrayIndices().push_back(c);
	}
	
	for (int y = 0; y < stacks - 2; y++)
	{
		for (int x = 0; x < slices; x++)
		{
			const int offsety = y * slices + 1;
			const int offsety2 = (y + 1) * slices + 1;

			int x2 = x + 1;
			if (x2 >= slices)
				x2 = 0;

			int a = offsety + x;
			int b = offsety + x2;
			int c = offsety2 + x;
			int d = offsety2 + x2;

			p->GetArrayIndices().push_back(a);
			p->GetArrayIndices().push_back(b);
			p->GetArrayIndices().push_back(c);
  
			p->GetArrayIndices().push_back(c);
			p->GetArrayIndices().push_back(b);
			p->GetArrayIndices().push_back(d);
		}
	}

	int lastIndex = slices * (stacks - 1) + 1;
	int lastStackIndex = lastIndex - slices;

	for (int i = 0; i < slices; i++)
	{
		const int a = lastIndex;
		int b = lastStackIndex + ((i == slices - 1) ? 0 : i + 1);
		int c = lastStackIndex + i;
		p->GetArrayIndices().push_back(a);
		p->GetArrayIndices().push_back(c);
		p->GetArrayIndices().push_back(b);
	}
		
	return p;
}

void LXPrimitiveFactory::Tick()
{

	// Si la primi est crée dans le LoadThread ... elle peut être imédiatement detruire par le Tick (MainTread)

	return;

	for (auto it = ListPrimitives.begin(); it != ListPrimitives.end();)
	{
		if (it->use_count() <= 1)
		{
			it = ListPrimitives.erase(it);
		}
		else
		{
			it++;
		}
	}
}
