#include "StdAfx.h"

void G1MMeshMatricesContainer::LoadFromStream(std::ifstream& file)
{
	Reset();
	file.read(reinterpret_cast<char*>(&NumBones), sizeof(DWORD));
	::SwapBytes(NumBones);

	for(DWORD i = 0; i < NumBones; i++)
	{
		MATRIX44 boneMatrix;
		file.read(reinterpret_cast<char*>(&boneMatrix), sizeof(MATRIX44));
		boneMatrix.SwapBytes();

		boneMatrices.push_back(boneMatrix);
	}
}

void G1MMeshMatricesContainer::Reset()
{
	NumBones = 0;
	boneMatrices.clear();
};
