#include "StdAfx.h"

void G1M_HEADER::SwapBytes()
{
	::SwapBytes(Magic);
	::SwapBytes(FileSize);
	::SwapBytes(DataOffset);
	::SwapBytes(Unknown);
	::SwapBytes(NumContainers);
}

void G1M_CONTAINER_HEADER::SwapBytes()
{
	::SwapBytes(Magic);
	::SwapBytes(Size);
}

void G1MM0020_CONTAINER_HEADER::SwapBytes()
{
	::SwapBytes(NumBones);
}

void SKELETON_CONTAINER_HEADER::SwapBytes()
{
	::SwapBytes(Size);
	::SwapBytes(Unknown);
	::SwapBytes(NumBones);
	::SwapBytes(NumIndices);
	::SwapBytes(NumSkeletons);
	::SwapBytes(Num5);
}

void GEOMETRY_CONTAINER_HEADER::SwapBytes()
{
	BoundingBox.SwapBytes();
	::SwapBytes(NumBuffers);
}

void G1M_BUFFER_HEADER::SwapBytes()
{
	::SwapBytes(BufferID);
	::SwapBytes(BufferDataSize);
	::SwapBytes(NumDataSets);
}

void VERTEX_BUFFER_HEADER::SwapBytes()
{
	::SwapBytes(StreamOffset);
	::SwapBytes(VertexStride);
	::SwapBytes(NumVertices);
	::SwapBytes(StreamIndex);
}

void VERTEX_ELEMENT_DESCRIPTOR::SwapBytes()
{
	::SwapBytes(StreamIndex);
	::SwapBytes(Offset);
	::SwapBytes(Usage);
}

void INDEX_BUFFER_HEADER::SwapBytes()
{
	::SwapBytes(NumIndices);
	::SwapBytes(SizeOfIndex);
	::SwapBytes(DataOffset);
}

void MESH_INFO::SwapBytes()
{
	::SwapBytes(ID);
	::SwapBytes(VertexDescriptorBufferIndex);
	::SwapBytes(MeshMatricesInfoIndex);
	::SwapBytes(ShaderParamIndex);
	::SwapBytes(Unknown0);
	::SwapBytes(ShaderParamIndex2); // 
	::SwapBytes(MaterialIndex);
	::SwapBytes(IndexBufferIndex); // VertexDescriptorIndex
	::SwapBytes(Unknown1);
	::SwapBytes(Unknown2);
	::SwapBytes(StartVertex);
	::SwapBytes(NumVertices);
	::SwapBytes(StartIndex);
	::SwapBytes(NumIndices);
}

void PARTS_INFO_ENTRY_HEADER::SwapBytes()
{
	::SwapBytes(Flags);
	::SwapBytes(Reserved);
	::SwapBytes(NumEntries);
}

void PARTS_INFO_HEADER::SwapBytes()
{
	::SwapBytes(Num0);
	::SwapBytes(Num1);
	::SwapBytes(Num2);
	::SwapBytes(NumParts);
	::SwapBytes(NumBodies);
	::SwapBytes(Num5);
	::SwapBytes(Num6);
	::SwapBytes(Num7);
	::SwapBytes(Num8);
};

void SHADER_PARAMETER_HEADER::SwapBytes()
{
	::SwapBytes(Size);
	::SwapBytes(NameSize);
	::SwapBytes(Unk0);
	::SwapBytes(vType);
	::SwapBytes(vType0);
};

void TEXTURE_REF::SwapBytes()
{
	::SwapBytes(TextureIndex);
	::SwapBytes(Index1);
	::SwapBytes(Index2);
	::SwapBytes(Index3);
	::SwapBytes(Index4);
	::SwapBytes(Index5);
};

void MATERIAL_BUFFER_HEADER::SwapBytes()
{
	::SwapBytes(Num0);
	::SwapBytes(NumTextures);
	::SwapBytes(Num2);
	::SwapBytes(Reserved);
};

void UNIFIED_VERTEX_FORMAT::SwapBytes()
{
	Position.SwapBytes();
	Normal;
	Tangent;
	Blendweight.SwapBytes();
	TexCoord.SwapBytes();
	TexCoord1.SwapBytes();
	TexCoord2.SwapBytes();
	TexCoord3.SwapBytes();
	TexCoord4.SwapBytes();
	PSize.SwapBytes();

	void SwapBytes();
};

void MESH_MATRICES_INFO_ENTRY::SwapBytes()
{
    ::SwapBytes(BoneMatrixIndex);
    ::SwapBytes(hfUnknown0);
    ::SwapBytes(wUnknown0);
    ::SwapBytes(hfUnknown1);
    ::SwapBytes(BoneIndex);
};
