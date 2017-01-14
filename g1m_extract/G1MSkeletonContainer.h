#pragma once
#include <windows.h>
#include <fstream>
#include <vector>
#include "G1MTypes.h"
class G1MFile;

class G1MSkeletonContainer
{
private:
	SKELETON_CONTAINER_HEADER skeletonContainerHeader;
	std::vector<SHORT> skeletonOffsets;
    std::vector<SHORT> skeletonIndices;
    std::vector<BONE> skeletonBones;
public:
	void ReadFromStream(std::ifstream& file);

	friend class G1MFile;
};
