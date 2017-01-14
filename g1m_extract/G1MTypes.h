#pragma once
#include "utility.h"


enum CONTAINER_ID : QWORD
{
    ID_G1M_0036 = 0x47314D5F30303336,
    ID_G1MF0023 = 0x47314D4630303233,
    ID_G1M_SKELETON_CONTAINER = 0x47314D5330303332,
    ID_G1M_MESH_MATRICES_CONTAINER = 0x47314D4D30303230,
    ID_G1M_GEOMETRY_CONTAINER = 0x47314D4730303434,
    ID_EXTR0010 = 0x4558545230303130,
};


enum BUFFER_ID : DWORD
{
    ID_00010001_BUFFER = 0x00010001, // refs from/to shader params
    ID_TEXTURE_BUFFER = 0x00010002,
    ID_SHADER_PARAMS_BUFFER = 0x00010003,
    ID_VERTEX_BUFFER = 0x00010004,
    ID_VERTEX_DESCRIPTOR_BUFFER = 0x00010005,
    ID_MESH_MATRICES_INFO_BUFFER = 0x00010006,
    ID_INDEX_BUFFER = 0x00010007,
    ID_MESH_INFO_BUFFER = 0x00010008,
    ID_PARTS_INFO_BUFFER = 0x00010009,
};

enum ATTRIBUTE_FORMAT : BYTE
{
	ATTRIBUTE_FORMAT_FLOAT1 = 0,
	ATTRIBUTE_FORMAT_FLOAT2 = 1,
	ATTRIBUTE_FORMAT_FLOAT3 = 2,
	ATTRIBUTE_FORMAT_FLOAT4 = 3,
	ATTRIBUTE_FORMAT_BYTE4 = 5,
	ATTRIBUTE_FORMAT_COLOR = 13,
};

enum VERTEX_ATTRIBUTE : WORD
{
    VERTEX_ATTRIBUTE_POSITION = 0,
    VERTEX_ATTRIBUTE_BLENDWEIGHT = 1,
    VERTEX_ATTRIBUTE_BLENDINDICES = 2,
    VERTEX_ATTRIBUTE_NORMAL = 3,
    VERTEX_ATTRIBUTE_PSIZE = 4,
    VERTEX_ATTRIBUTE_TEXCOORD = 5,
    VERTEX_ATTRIBUTE_TANGENT = 6,
    VERTEX_ATTRIBUTE_BINORMAL = 7,
    VERTEX_ATTRIBUTE_TESSFACTOR = 8,
    VERTEX_ATTRIBUTE_COLOR = 10,
    VERTEX_ATTRIBUTE_FOG = 11,
    VERTEX_ATTRIBUTE_DEPTH = 12,
    VERTEX_ATTRIBUTE_SAMPLE = 13
};

enum SHADER_PARAMETER_FORMAT : WORD
{
	SHADER_PARAMETER_FORMAT_FLOAT1 = 1,
	SHADER_PARAMETER_FORMAT_FLOAT2 = 2,
	SHADER_PARAMETER_FORMAT_FLOAT3 = 3,
	SHADER_PARAMETER_FORMAT_FLOAT4 = 4,
	SHADER_PARAMETER_FORMAT_DWORD = 5,

};


#pragma pack( push, 1  )

struct G1M_HEADER
{
	QWORD Magic;
	DWORD FileSize;
	DWORD DataOffset;
	DWORD Unknown;
	DWORD NumContainers;

	void SwapBytes();
};

struct G1M_CONTAINER_HEADER
{
    QWORD Magic;
    DWORD Size;

	void SwapBytes();
};

struct G1MM0020_CONTAINER_HEADER
{
	DWORD NumBones;

	void SwapBytes();
};

struct SKELETON_CONTAINER_HEADER
{
    DWORD Size;
    float Unknown;
    WORD NumBones;
    WORD NumIndices;
    WORD NumSkeletons;
    WORD Num5;

	void SwapBytes();

};

struct GEOMETRY_CONTAINER_HEADER
{
    BYTE Plarform[8];
    BBOX BoundingBox;
    DWORD NumBuffers;

	void SwapBytes();
};

struct G1M_BUFFER_HEADER
{
    DWORD BufferID;
    DWORD BufferDataSize;
    DWORD NumDataSets;

	void SwapBytes();
};

struct VERTEX_BUFFER_HEADER
{
    DWORD StreamOffset;
    DWORD VertexStride;
    DWORD NumVertices;
    DWORD StreamIndex;

	void SwapBytes();
};

struct VERTEX_ELEMENT_DESCRIPTOR
{
	WORD StreamIndex;
	WORD Offset;
	BYTE Format;
	WORD Usage;
	BYTE UsageIndex;

	void SwapBytes();
};

struct INDEX_BUFFER_HEADER
{
    DWORD NumIndices;
    DWORD SizeOfIndex;
    DWORD DataOffset;

	void SwapBytes();
};

struct MESH_INFO
{
    DWORD ID;
    DWORD VertexDescriptorBufferIndex;
    DWORD MeshMatricesInfoIndex;
    DWORD ShaderParamIndex;
    DWORD Unknown0;
    DWORD ShaderParamIndex2; // 
    DWORD MaterialIndex;
    DWORD IndexBufferIndex;
    DWORD Unknown1;
    DWORD Unknown2;
    DWORD StartVertex;
    DWORD NumVertices;
    DWORD StartIndex;
    DWORD NumIndices;

	void SwapBytes();
};

struct PARTS_INFO_ENTRY_HEADER
{
    CHAR Name[16];
    DWORD Flags;
    DWORD Reserved;
    DWORD NumEntries;

	void SwapBytes();
};

struct PARTS_INFO_HEADER
{
    DWORD Num0;
    DWORD Num1;
    DWORD Num2;
    DWORD NumParts;
    DWORD NumBodies;
    LONG Num5;
    LONG Num6;
    LONG Num7;
    LONG Num8;

	void SwapBytes();
};


struct SHADER_PARAMETER_HEADER
{
    DWORD Size;
    DWORD NameSize;
    DWORD Unk0;
    WORD vType;
    WORD vType0;

	void SwapBytes();
};

struct TEXTURE_REF
{
    WORD TextureIndex;
    WORD Index1;
    WORD Index2;
    WORD Index3;
    WORD Index4;
    WORD Index5;

	void SwapBytes();
};

struct MATERIAL_BUFFER_HEADER
{
    DWORD Num0;
    DWORD NumTextures;
    DWORD Num2;
    DWORD Reserved;

	void SwapBytes();
};

struct UNIFIED_VERTEX_FORMAT
{
	VECTOR3 Position;
	VECTOR3 Normal;
	VECTOR4 Tangent;
	COLOR Color;
	BYTE4 Blendindices;
	VECTOR4 Blendweight;
	VECTOR2 TexCoord;
	VECTOR2 TexCoord1;
	VECTOR2 TexCoord2;
	VECTOR2 TexCoord3;
	VECTOR2 TexCoord4;
	VECTOR3 PSize;

	void SwapBytes();
};

struct MESH_MATRICES_INFO_ENTRY
{
    DWORD BoneMatrixIndex;
    WORD hfUnknown0;
    WORD wUnknown0;
    WORD hfUnknown1;
    WORD BoneIndex;

	void SwapBytes();
};

#pragma pack( pop  )
