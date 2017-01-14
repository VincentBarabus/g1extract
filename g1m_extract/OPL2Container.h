#pragma once

class OPL2PrefabInstance
{
public:
	VECTOR4 position;
	VECTOR4 rotation;
	VECTOR3 scale;
	DWORD ID;
	DWORD extIDSize;
	std::vector<BYTE> extID;
	BYTE Reserved[32];

	void LoadFromStream(std::ifstream& file);
private:
	void RecalculateRotation(VECTOR4& quaternion);
};

class OPL2Prefab
{
public:
	DWORD NameSize;
	std::string Name;
	LONG ModelIndex;
	LONG Reserved[3];
	LONG AnimationIndex;
	LONG TextureIndex;
	DWORD NumInstances;
	std::vector<OPL2PrefabInstance> Instances;
	void LoadFromStream(std::ifstream& file);
};



class OPL2Container
{
public:
	DWORD NumPrefabs;
	std::vector<OPL2Prefab> Prefabs;
	void LoadFromStream(std::ifstream& file);
	void ExportContentToFBX(ElixirFile& scene, ElixirFile& sceneObjects, std::string& fbxFileName);
};

