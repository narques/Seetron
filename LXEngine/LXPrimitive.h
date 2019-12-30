//------------------------------------------------------------------------------------------------------
//
// This is a part of Seetron Engine
//
// Copyright (c) 2018 Nicolas Arques. All rights reserved.
//
//------------------------------------------------------------------------------------------------------

#pragma once

#include "LXObject.h"
#include "LXBBox.h"
#include "LXSmartObject.h"
#include "LXVec3.h"
#include "LXVec2.h"

#define POSITION3F vec3f position;
#define TEXCOORD2F vec2f texCoord;
#define NORMAL3F vec3f normal;
#define TANGENT3F vec3f tangent;
#define BINORMAL3F vec3f binormal;

#define DEFINE_VERTEX_STRUCT(suffix, ARGS)   struct Vertex_##suffix{ ARGS };

DEFINE_VERTEX_STRUCT(P, POSITION3F)
DEFINE_VERTEX_STRUCT(PN, POSITION3F NORMAL3F)
DEFINE_VERTEX_STRUCT(PT, POSITION3F TEXCOORD2F)
DEFINE_VERTEX_STRUCT(PNT, POSITION3F NORMAL3F TEXCOORD2F)
DEFINE_VERTEX_STRUCT(PNA, POSITION3F NORMAL3F TANGENT3F)
DEFINE_VERTEX_STRUCT(PNAB, POSITION3F NORMAL3F TANGENT3F BINORMAL3F)
DEFINE_VERTEX_STRUCT(PNAT, POSITION3F NORMAL3F TANGENT3F TEXCOORD2F)
DEFINE_VERTEX_STRUCT(PNABT, POSITION3F NORMAL3F TANGENT3F BINORMAL3F TEXCOORD2F)

#define LX_PRIMITIVE_INDICES	LX_BIT(0)
#define LX_PRIMITIVE_POSITIONS	LX_BIT(1)
#define LX_PRIMITIVE_NORMALS	LX_BIT(2)
#define LX_PRIMITIVE_TANGENTS	LX_BIT(3)
#define LX_PRIMITIVE_TEXCOORDS	LX_BIT(4)
#define LX_PRIMITIVE_BINORMALS	LX_BIT(5)
#define LX_PRIMITIVE_INSTANCEPOSITIONS LX_BIT(6)

enum LXDataType /* DataModel : The new items must be added to the end */
{
	LX_DATATYPE_UNDEFINED,
	LX_VEC2F,
	LX_VEC3F,
	LX_VEC4F,
	LX_VEC2UI,
	LX_VEC3UI,
	LX_VEC4UI
};

enum LXDataTarget /* DataModel : The new items must be added to the end */
{
	LX_DATATARGET_UNDEFINED,
	LX_INDICES,
	LX_VERTICES,
	LX_TEXTCOORD,
	LX_NORMALS,
	LX_TANGENTS,
	LX_BINORMALS,
};

enum LXPrimitiveTopology /* DataModel : The new items must be added to the end */
{
	LX_MODE_UNDEFINED = -1,
	LX_POINTS = 0,
	LX_LINES = 1,
	LX_LINE_LOOP = 2,
	LX_LINE_STRIP = 3,
	LX_TRIANGLES = 4,
	LX_TRIANGLE_STRIP = 5,
	LX_TRIANGLE_FAN = 6,
	LX_TRIANGLES_ADJACENCY = 7,
	LX_3_CONTROL_POINT_PATCH,
	LX_4_CONTROL_POINT_PATCH
};

struct TFileTag
{
	int						nId;
	LXDataTarget			eDataTarget;
	LXDataType				eDataType;
	LXPrimitiveTopology		eMode;
	uint					nCount;
};

class LXCORE_API LXPrimitive : public LXSmartObject
{

	friend class LXTriStripper;

public:

	// Constructors
	LXPrimitive(void);
	LXPrimitive(const LXPrimitive& primitive);

	// Destructor
	virtual ~LXPrimitive(void);

	LXPrimitive*		Copy				( ) { return new LXPrimitive(*this); }
	LXBBox&				GetBBoxLocal() 
	{ 
		if (!m_bboxLocal.IsValid())
			ComputeBBoxLocal();
		CHK(m_bboxLocal.IsValid());
		return m_bboxLocal; 
	}

	void				SetPositions		( float* lVertices, int lPolygonVertexCount, const int VERTEX_STRIDE );
	void				SetNormals			( float* pNormals, int lPolygonVertexCount, const int NORMAL_STRIDE );
	void				SetTexCoords		( float* pTexCoords, int lPolygonVertexCount, const int UV_STRIDE );
	
	
	static LXPrimitive* CreateLineStrip		( );
	static LXPrimitive* CreateLineLoop		( );
	static LXPrimitive* CreatePoint			( );

	// Creates an indexed Disk with UVs and Normals
	static LXPrimitive* CreateDisk(float radius, float height);
	
	// Transformation
	void				LocalToParent		( const LXMatrix& matrix );

	// Misc
	void				Empty				( );
	void				Invalidate() { m_bValid = false; }

	// Overridden from LXSmartObject
	void DefineProperties() ;

	uint				GetIndices			( ) const { return (uint)m_arrayIndices.size(); }
	uint				GetVertices			( ) const 
	{ 
		if (m_arrayPositions.size())
			return (uint)m_arrayPositions.size();
		else if (m_arrayPositions4f.size())
			return (uint)m_arrayPositions4f.size();
		else
			return 0;
	}

	ArrayUint&			GetArrayIndices		( ) { return m_arrayIndices; }
	ArrayVec3f&			GetArrayPositions	( ) { return m_arrayPositions; }
	ArrayVec4f&			GetArrayPositions4f	( ) { return m_arrayPositions4f; }
	ArrayVec3f&			GetArrayNormals		( ) { return m_arrayNormals; }
	ArrayVec3f&			GetArrayTangents	( ) { return m_arrayTangents; }
	ArrayVec3f&			GetArrayBiNormals	( ) { return m_arrayBiNormals; }
	ArrayVec2f&			GetArrayTexCoords	( ) { return m_arrayTexCoords; }
	ArrayVec3f&			GetArrayTexCoords3f	( ) { return m_arrayTexCoords3f; }

	void				AddPosition			( const vec3f& v ) { m_arrayPositions.push_back(v);}
	void				AddPosition			( const vec2f& v ) { m_arrayPositions.push_back(vec3f(v.x, v.y, 0.f));}
		
	bool				HasNormals			( ) const { return m_arrayNormals.size()?true:false; }
	bool				HasTangents			( ) const { return m_arrayTangents.size()?true:false; }
	bool				HasBiNormals		( ) const { return m_arrayBiNormals.size()?true:false; }
	bool				HasTexCoords		( ) const { return m_arrayTexCoords.size()?true:false; }
	bool				HasTexCoords3f		( ) const { return m_arrayTexCoords3f.size()?true:false; }
	
	int					GetId				( );
	
	bool				Save				(  const TSaveContext& saveContext  );
	void				SetMaterial			( LXMaterial* pMaterial );
	void				SetMaterial			( const LXString& Filename );
	LXMaterial*			GetMaterial() const { return m_pMaterial; }
	
	// Draw Options
	LXPrimitiveTopology	GetTopology			( ) const { return _Topology; }
	void				SetTopology			( LXPrimitiveTopology e ) { _Topology = e; }

	void				ComputeNormals		( );
	void				ComputeTangents		( ); // And BiNormals
	bool				Merge				( const LXPrimitive& v);

	// Return the composition mask
	int					GetMask()			const;

	void*				CreateInterleavedVertexArray(const int Mask, const int VertexCount, int *OutVertexStructSize);

private:

	template<class T, class U>
	void				ComputeTangents2	( vector<U>& arrayPositions, vector<T>& arrayTexCoords ); // And BiNormals
	void				MergeIndices		( ArrayUint& dest, const ArrayUint& src );
	void				ComputeBBoxLocal	( );
	
public:

	//
	// Factory
	//

public :

	void				AddQuadXY			( const vec3f& vPosition, float width, float height, float depth );
	void				AddQuadYZ			( const vec3f& vPosition, float width, float height, float depth );
	void				AddQuadXZ			( const vec3f& vPosition, float width, float height, float depth );
		
	//protected:
	
	ArrayUint		m_arrayIndices;
		
	ArrayVec3f		m_arrayPositions;
	ArrayVec4f		m_arrayPositions4f;
	ArrayVec3f		m_arrayNormals;
	ArrayVec3f		m_arrayTangents;
	ArrayVec3f		m_arrayBiNormals;
	ArrayVec2f		m_arrayTexCoords;
	ArrayVec3f		m_arrayTexCoords3f;

	// Misc
	LXPrimitiveTopology	_Topology;
	LXMaterial*		m_pMaterial;
	int				m_nId;
	bool			m_bValid;
	LXBBox			m_bboxLocal;
	static int		m_snPrimitives;
	static int		m_snPoints;

	#ifdef LX_DEBUG_PRIMITIVE_PROPVISIBLE
	bool			_bVisible = true;
	#endif
};

typedef list<shared_ptr<LXPrimitive>> ListPrimitives;
typedef vector<LXPrimitive*> ArrayPrimitives;
typedef set<LXPrimitive*> SetPrimitives;
