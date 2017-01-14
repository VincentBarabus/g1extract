#pragma once
#include <windows.h>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>
#include <fbxsdk.h>
#include "G1MTypes.h"

class G1MGeometryContainer;

class G1MVertexDescriptorBuffer
{
public:
    DWORD NumVBs;
	std::vector<DWORD> VBIndices;
    DWORD NumElements;
	std::vector<VERTEX_ELEMENT_DESCRIPTOR> Elements;
public:
	void ReadFromStream(std::ifstream& file);
	DWORD GetHash();
};

class G1MVertexBuffer
{
public:
	VERTEX_BUFFER_HEADER vertexBufferHeader;
	std::vector<BYTE> rawData;
public:
	void ReadFromStream(std::ifstream& file);
};


class G1MIndexBuffer
{
public:
	INDEX_BUFFER_HEADER indexBufferHeader;
	std::vector<DWORD> indices;

public:
	void ReadFromStream(std::ifstream& file);
};

class G1MPartsInfoEntry
{
public:
	PARTS_INFO_ENTRY_HEADER partsInfoEntryHeader;
	std::vector<DWORD> meshInfoIndices;
public:
	void ReadFromStream(std::ifstream& file);
};

class G1MPartsInfoBuffer
{
public:
	PARTS_INFO_HEADER partsInfoHeader;
	std::vector<G1MPartsInfoEntry> parts;
	std::vector<G1MPartsInfoEntry> bodies;
public:
	void ReadFromStream(std::ifstream& file);
};

class G1MShaderParameter
{
public:
	SHADER_PARAMETER_HEADER shaderParameterHeader;

	std::string Name;
	std::vector<BYTE> Data;
public:
	void ReadFromStream(std::ifstream& file);
};

class G1MShaderParamsBuffer
{
public:
    DWORD NumParameters;
    std::vector<G1MShaderParameter> parameters;
public:
	void ReadFromStream(std::ifstream& file);
};

class G1MMaterialBuffer
{
public:
	MATERIAL_BUFFER_HEADER textureBufferHeader;
	std::vector<TEXTURE_REF> textureRefs;
public:
	void ReadFromStream(std::ifstream& file);
};

class G1MMeshMatricesInfoBuffer
{
public:
	DWORD NumEntries;
	std::vector<MESH_MATRICES_INFO_ENTRY> meshMatricesInfoEntries;
public:
	void ReadFromStream(std::ifstream& file);
};

class G1MGeometryContainer
{
public:
	std::vector<G1MVertexBuffer> vertexBuffers;
	std::vector<G1MVertexDescriptorBuffer> vertexDescriptorBuffers;
	std::vector<G1MIndexBuffer> indexBuffers;
	std::vector<G1MPartsInfoBuffer> partsInfoBuffers;
	std::vector<G1MShaderParamsBuffer> shaderParamsBuffers;
	std::vector<G1MMaterialBuffer> textureBuffers;
	std::vector<G1MMeshMatricesInfoBuffer> meshMatricesInfoBuffers;

	std::vector<MESH_INFO> meshInfoIndices;

public:
	void ReadFromStream(std::ifstream& file);
	void Reset();
};
