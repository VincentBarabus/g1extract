#include "StdAfx.h"

void G1MVertexDescriptorBuffer::ReadFromStream(std::ifstream& file)
{
	file.read(reinterpret_cast<char*>(&NumVBs), sizeof(DWORD));
	::SwapBytes(NumVBs);

	for(DWORD i = 0; i < NumVBs; i++)
	{
		DWORD VBIndex;
		file.read(reinterpret_cast<char*>(&VBIndex), sizeof(DWORD));
		::SwapBytes(VBIndex);
		VBIndices.push_back(VBIndex);
	}

	file.read(reinterpret_cast<char*>(&NumElements), sizeof(DWORD));
	::SwapBytes(NumElements);
	
	for(DWORD i = 0; i < NumElements; i++)
	{
		VERTEX_ELEMENT_DESCRIPTOR Element;
		file.read(reinterpret_cast<char*>(&Element), sizeof(VERTEX_ELEMENT_DESCRIPTOR));
		Element.SwapBytes();
		Elements.push_back(Element);
	}
}

DWORD G1MVertexDescriptorBuffer::GetHash()
{
	return crc32(reinterpret_cast<BYTE*>(Elements.data()), sizeof(VERTEX_ELEMENT_DESCRIPTOR) * Elements.size());
}

void G1MVertexBuffer::ReadFromStream(std::ifstream& file)
{
	file.read(reinterpret_cast<char*>(&vertexBufferHeader), sizeof(VERTEX_BUFFER_HEADER));
	vertexBufferHeader.SwapBytes();

	rawData.resize(vertexBufferHeader.VertexStride * vertexBufferHeader.NumVertices);
	file.read(reinterpret_cast<char*>(rawData.data()), vertexBufferHeader.VertexStride * vertexBufferHeader.NumVertices);
}

void G1MIndexBuffer::ReadFromStream(std::ifstream& file)
{
	file.read(reinterpret_cast<char*>(&indexBufferHeader), sizeof(INDEX_BUFFER_HEADER));
	indexBufferHeader.SwapBytes();

	if(indexBufferHeader.SizeOfIndex == 16)
	{
		for(DWORD i = 0; i < indexBufferHeader.NumIndices; i++)
		{
			WORD Index;
			file.read(reinterpret_cast<char*>(&Index), sizeof(WORD));
			::SwapBytes(Index);
			indices.push_back(Index);
		}
	}
	else if(indexBufferHeader.SizeOfIndex == 32)
	{
		for(DWORD i = 0; i < indexBufferHeader.NumIndices; i++)
		{
			DWORD Index;
			file.read(reinterpret_cast<char*>(&Index), sizeof(DWORD));
			::SwapBytes(Index);
			indices.push_back(Index);
		}
	}
	else
	{
		assert(0);
	}
}

void G1MPartsInfoEntry::ReadFromStream(std::ifstream& file)
{

	file.read(reinterpret_cast<char*>(&partsInfoEntryHeader), sizeof(PARTS_INFO_ENTRY_HEADER));
	partsInfoEntryHeader.SwapBytes();

	for(DWORD i  = 0; i < partsInfoEntryHeader.NumEntries; i++)
	{
		DWORD Index;
		file.read(reinterpret_cast<char*>(&Index), sizeof(DWORD));
		::SwapBytes(Index);
		meshInfoIndices.push_back(Index);
	}

}

void G1MPartsInfoBuffer::ReadFromStream(std::ifstream& file)
{
	file.read(reinterpret_cast<char*>(&partsInfoHeader), sizeof(PARTS_INFO_HEADER));
	partsInfoHeader.SwapBytes();

	for(DWORD i  = 0; i < partsInfoHeader.NumParts + partsInfoHeader.NumBodies; i++)
	{
		G1MPartsInfoEntry part;
		part.ReadFromStream(file);
		parts.push_back(part);
	}
	/* hack
	for(DWORD i  = 0; i < partsInfoHeader.NumBodies; i++)
	{
		G1MPartsInfoEntry body;
		body.ReadFromStream(file);
		bodies.push_back(body);
	}
	*/
}

void G1MShaderParameter::ReadFromStream(std::ifstream& file)
{
	file.read(reinterpret_cast<char*>(&shaderParameterHeader), sizeof(SHADER_PARAMETER_HEADER));
	shaderParameterHeader.SwapBytes();

	Name.resize(shaderParameterHeader.NameSize + 1);
	file.read(const_cast<char*>(Name.data()), shaderParameterHeader.NameSize);

	switch(shaderParameterHeader.vType)
    {
    case SHADER_PARAMETER_FORMAT_FLOAT1:
		Data.resize(sizeof(float));
		file.read(reinterpret_cast<char*>(Data.data()), sizeof(float));
		::SwapBytes(*reinterpret_cast<float*>(Data.data()));
        break;

    case SHADER_PARAMETER_FORMAT_FLOAT2:
		Data.resize(sizeof(VECTOR2));
		file.read(reinterpret_cast<char*>(Data.data()), sizeof(VECTOR2));
		reinterpret_cast<VECTOR2*>(Data.data())->SwapBytes();
        break;

    case SHADER_PARAMETER_FORMAT_FLOAT3:
		Data.resize(sizeof(VECTOR3));
		file.read(reinterpret_cast<char*>(Data.data()), sizeof(VECTOR3));
		reinterpret_cast<VECTOR3*>(Data.data())->SwapBytes();
        break;

    case SHADER_PARAMETER_FORMAT_FLOAT4:
		Data.resize(sizeof(VECTOR4));
		file.read(reinterpret_cast<char*>(Data.data()), sizeof(VECTOR4));
		reinterpret_cast<VECTOR4*>(Data.data())->SwapBytes();
        break;

    case SHADER_PARAMETER_FORMAT_DWORD:
		Data.resize(sizeof(DWORD));
		file.read(reinterpret_cast<char*>(Data.data()), sizeof(DWORD));
		::SwapBytes(*reinterpret_cast<DWORD*>(Data.data()));
        break;
	default:
		assert(0);
    }


	// for(DWORD i  = 0; i < shaderParameterHeader.vSize; i++)
	// {
	// 	float var;
	// 	file.read(reinterpret_cast<char*>(&var), sizeof(float));
	// 	::SwapBytes(var);
	// 	Data.push_back(var);
	// }
}

void G1MShaderParamsBuffer::ReadFromStream(std::ifstream& file)
{
	file.read(reinterpret_cast<char*>(&NumParameters), sizeof(DWORD));
	::SwapBytes(NumParameters);

	for(DWORD i  = 0; i < NumParameters; i++)
	{
		G1MShaderParameter parameter;
		parameter.ReadFromStream(file);
		parameters.push_back(parameter);
	}
}

void G1MMaterialBuffer::ReadFromStream(std::ifstream& file)
{
	file.read(reinterpret_cast<char*>(&textureBufferHeader), sizeof(MATERIAL_BUFFER_HEADER));
	textureBufferHeader.SwapBytes();

	for(DWORD i  = 0; i < textureBufferHeader.NumTextures; i++)
	{
		TEXTURE_REF TextureRef;
		file.read(reinterpret_cast<char*>(&TextureRef), sizeof(TEXTURE_REF));
		TextureRef.SwapBytes();
		textureRefs.push_back(TextureRef);
	}

}

void G1MMeshMatricesInfoBuffer::ReadFromStream(std::ifstream& file)
{
	file.read(reinterpret_cast<char*>(&NumEntries), sizeof(DWORD));
	::SwapBytes(NumEntries);

	for(DWORD i  = 0; i < NumEntries; i++)
	{
		MESH_MATRICES_INFO_ENTRY meshMatricesInfoEntry;
		file.read(reinterpret_cast<char*>(&meshMatricesInfoEntry), sizeof(MESH_MATRICES_INFO_ENTRY));
		meshMatricesInfoEntry.SwapBytes();

		meshMatricesInfoEntries.push_back(meshMatricesInfoEntry);
	}
}

void G1MGeometryContainer::ReadFromStream(std::ifstream& file)
{
	Reset();
	GEOMETRY_CONTAINER_HEADER geometryContainerHeader;
	file.read(reinterpret_cast<char*>(&geometryContainerHeader), sizeof(GEOMETRY_CONTAINER_HEADER));
	geometryContainerHeader.SwapBytes();

	// Load buffers
	for(DWORD i = 0; i < geometryContainerHeader.NumBuffers; i++)
	{
		G1M_BUFFER_HEADER bufferHeader;
		file.read(reinterpret_cast<char*>(&bufferHeader), sizeof(G1M_BUFFER_HEADER));
		bufferHeader.SwapBytes();

		switch(bufferHeader.BufferID)
		{
			case ID_00010001_BUFFER:
				// now skip
				file.seekg(bufferHeader.BufferDataSize - sizeof(G1M_BUFFER_HEADER), std::ios_base::cur);
				break;

			case ID_TEXTURE_BUFFER:
				for(DWORD j = 0; j < bufferHeader.NumDataSets; j++)
				{
					G1MMaterialBuffer textureBuffer;
					textureBuffer.ReadFromStream(file);
					textureBuffers.push_back(textureBuffer);
				}
				break;

			case ID_SHADER_PARAMS_BUFFER:
				for(DWORD j = 0; j < bufferHeader.NumDataSets; j++)
				{
					G1MShaderParamsBuffer shaderParamsBuffer;
					shaderParamsBuffer.ReadFromStream(file);
					shaderParamsBuffers.push_back(shaderParamsBuffer);
				}
				break;

			case ID_VERTEX_BUFFER:
				for(DWORD j = 0; j < bufferHeader.NumDataSets; j++)
				{
					G1MVertexBuffer vertexBuffer;
					vertexBuffer.ReadFromStream(file);
					vertexBuffers.push_back(vertexBuffer);
				}
				break;

			case ID_VERTEX_DESCRIPTOR_BUFFER:
				for(DWORD j = 0; j < bufferHeader.NumDataSets; j++)
				{
					G1MVertexDescriptorBuffer vertexDescriptorBuffer;
					vertexDescriptorBuffer.ReadFromStream(file);
					vertexDescriptorBuffers.push_back(vertexDescriptorBuffer);
				}
				break;
	
			case ID_INDEX_BUFFER:
				for(DWORD j = 0; j < bufferHeader.NumDataSets; j++)
				{
					G1MIndexBuffer indexBuffer;
					indexBuffer.ReadFromStream(file);
					indexBuffers.push_back(indexBuffer);
				}
				break;

			case ID_MESH_MATRICES_INFO_BUFFER:
				for(DWORD j = 0; j < bufferHeader.NumDataSets; j++)
				{
					G1MMeshMatricesInfoBuffer meshMatricesInfoBuffer;
					meshMatricesInfoBuffer.ReadFromStream(file);
					meshMatricesInfoBuffers.push_back(meshMatricesInfoBuffer);
				}
				break;

			case ID_MESH_INFO_BUFFER:
				for(DWORD j = 0; j < bufferHeader.NumDataSets; j++)
				{
					MESH_INFO meshInfo;
					file.read(reinterpret_cast<char*>(&meshInfo), sizeof(MESH_INFO));
					meshInfo.SwapBytes();
					meshInfoIndices.push_back(meshInfo);
				}
				break;

			case ID_PARTS_INFO_BUFFER:
				for(DWORD j = 0; j < bufferHeader.NumDataSets; j++)
				{
					G1MPartsInfoBuffer partsInfoBuffer;
					partsInfoBuffer.ReadFromStream(file);
					partsInfoBuffers.push_back(partsInfoBuffer);
				}
				break;

			default:
				// skip unknown
				file.seekg(bufferHeader.BufferDataSize, std::ios_base::cur);
				break;
		}
	}
}

void G1MGeometryContainer::Reset()
{
	vertexBuffers.clear();
	vertexDescriptorBuffers.clear();
	indexBuffers.clear();
	partsInfoBuffers.clear();
	shaderParamsBuffers.clear();
	textureBuffers.clear();
	meshMatricesInfoBuffers.clear();
	meshInfoIndices.clear();
}