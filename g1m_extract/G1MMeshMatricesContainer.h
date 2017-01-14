#pragma once
#include <fstream>

class G1MMeshMatricesContainer
{
public:
	DWORD NumBones;
	std::vector<MATRIX44> boneMatrices;
public:
	void LoadFromStream(std::ifstream& file);
	void Reset();
};

