#include "StdAfx.h"


void G1MSkeletonContainer::ReadFromStream(std::ifstream& file)
{
	file.read(reinterpret_cast<char*>(&skeletonContainerHeader), sizeof(SKELETON_CONTAINER_HEADER));
	skeletonContainerHeader.SwapBytes();

	for(WORD i = 0; i < skeletonContainerHeader.NumSkeletons; i++)
	{
		SHORT skeletonOffset;
		file.read(reinterpret_cast<char*>(&skeletonOffset), sizeof(SHORT));
		::SwapBytes(skeletonOffset);
		skeletonOffsets.push_back(skeletonOffset);
	}

	for(WORD i = 0; i < skeletonContainerHeader.NumIndices; i++)
	{
		SHORT skeletonIndex;
		file.read(reinterpret_cast<char*>(&skeletonIndex), sizeof(SHORT));
		::SwapBytes(skeletonIndex);
		skeletonIndices.push_back(skeletonIndex);
	}
	AlignTo(file, 4);

	for(WORD i = 0; i < skeletonContainerHeader.NumBones; i++)
	{
		BONE skeletonBone;
		file.read(reinterpret_cast<char*>(&skeletonBone), sizeof(BONE));
		skeletonBone.SwapBytes();
		skeletonBones.push_back(skeletonBone);
	}

	return;
}

