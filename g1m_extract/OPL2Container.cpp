#include "stdafx.h"
#include "OPL2Container.h"


void OPL2PrefabInstance::RecalculateRotation(VECTOR4& rotation)
{
	rotation.x = rotation.x / 3.1415927410125732421875f * 180.0f;
	rotation.y = rotation.y / 3.1415927410125732421875f * 180.0f;
	rotation.z = rotation.z / 3.1415927410125732421875f * 180.0f;
}

void OPL2PrefabInstance::LoadFromStream(std::ifstream& file)
{
	file.read(reinterpret_cast<char*>(&position), sizeof(VECTOR4));
	position.SwapBytes();
	file.read(reinterpret_cast<char*>(&rotation), sizeof(VECTOR4));
	rotation.SwapBytes();
	RecalculateRotation(rotation);
	file.read(reinterpret_cast<char*>(&scale), sizeof(VECTOR3));
	scale.SwapBytes();
	file.read(reinterpret_cast<char*>(&ID), sizeof(DWORD));
	::SwapBytes(ID);
	file.read(reinterpret_cast<char*>(&extIDSize), sizeof(DWORD));
	::SwapBytes(extIDSize);
	extID.resize(extIDSize);
	file.read(reinterpret_cast<char*>(extID.data()), extIDSize);

	file.read(reinterpret_cast<char*>(Reserved), sizeof(BYTE) * 32);
}

void OPL2Prefab::LoadFromStream(std::ifstream& file)
{
	file.read(reinterpret_cast<char*>(&NameSize), sizeof(DWORD));
	::SwapBytes(NameSize);
	Name.resize(NameSize);
	file.read(const_cast<char*>(Name.data()), NameSize);
	file.read(reinterpret_cast<char*>(&ModelIndex), sizeof(LONG));
	::SwapBytes(ModelIndex);
	file.read(reinterpret_cast<char*>(Reserved), sizeof(LONG) * 3);
	file.read(reinterpret_cast<char*>(&AnimationIndex), sizeof(LONG));
	::SwapBytes(AnimationIndex);
	file.read(reinterpret_cast<char*>(&TextureIndex), sizeof(LONG));
	::SwapBytes(TextureIndex);
	file.read(reinterpret_cast<char*>(&NumInstances), sizeof(LONG));
	::SwapBytes(NumInstances);

	for (DWORD i = 0; i < NumInstances; i++)
	{
		OPL2PrefabInstance Instance;
		Instance.LoadFromStream(file);
		Instances.push_back(Instance);
	}
}

void OPL2Container::LoadFromStream(std::ifstream& file)
{
	file.read(reinterpret_cast<char*>(&NumPrefabs), sizeof(DWORD));
	::SwapBytes(NumPrefabs);

	for (DWORD i = 0; i < NumPrefabs; i++)
	{
		OPL2Prefab Prefab;
		Prefab.LoadFromStream(file);
		Prefabs.push_back(Prefab);
	}

	return;
}


void OPL2Container::ExportContentToFBX(ElixirFile& scene, ElixirFile& sceneObjects, std::string& fbxFileName)
{
	// Create and init the FBX Exporter
	FbxManager* pSdkManager = FbxManager::Create();
	FbxIOSettings * ios = FbxIOSettings::Create(pSdkManager, IOSROOT);
	ios->SetBoolProp(EXP_FBX_EMBEDDED, true);
	pSdkManager->SetIOSettings(ios);
	FbxExporter* pExporter = FbxExporter::Create(pSdkManager, "");

	pExporter->Initialize(fbxFileName.c_str(), -1, pSdkManager->GetIOSettings());
	pExporter->SetFileExportVersion(FBX_2014_00_COMPATIBLE);

	FbxScene* pScene = FbxScene::Create(pSdkManager, "myScene");

	DWORD prefabIndex = 0;
	G1MFile loader;
	loader.LoadTexturesFromStream(scene.GetStreamAt(2));	// << set textures file here
															// skeleton must be loaded
															// fix meshloader for skinned mesh without blendweights
	loader.BuildTexturesAndMatirials(pScene->GetRootNode());
	for (auto& prefab : Prefabs)
	{
		if (prefabIndex == 67)
			__asm nop;

		std::cout << "Creating prefab with index " << std::to_string(prefabIndex) << std::endl;
		loader.LoadMeshFromStream(sceneObjects.GetStreamAt(prefab.ModelIndex));

		FbxNode* pPrefabMeshNode = NULL;
		for (DWORD i = 0; i < prefab.NumInstances; i++)
		{
			auto& instance = prefab.Instances[i];
			if (i == 0)
			{
				std::cout << "Creating prefab " << prefab.Name << std::endl;
				std::cout << "Creating instance " << prefab.Name + " " + std::to_string(i) << std::endl;
				pPrefabMeshNode = FbxNode::Create(pScene, (prefab.Name + std::to_string(i)).c_str());
				FbxAMatrix matrix;
				matrix.SetTRS(instance.position, instance.rotation, instance.scale);
				pPrefabMeshNode->LclTranslation.Set(matrix.GetT());
				pPrefabMeshNode->LclRotation.Set(matrix.GetR());
				pPrefabMeshNode->LclScaling.Set(matrix.GetS());
				loader.BuildBrefab(pPrefabMeshNode);
				pScene->GetRootNode()->AddChild(pPrefabMeshNode);
			}
			else
			{
				std::cout << "Creating instance " << prefab.Name + " " + std::to_string(i) << std::endl;
				FbxNode* pInstanceMeshNode = FbxNode::Create(pScene, (prefab.Name + std::to_string(i)).c_str());
				pInstanceMeshNode->SetShadingMode(FbxNode::eTextureShading);

				pInstanceMeshNode->SetNodeAttribute(pPrefabMeshNode->GetMesh());
				for (int j = 0; j < pPrefabMeshNode->GetMaterialCount(); j++)
				{
					pInstanceMeshNode->AddMaterial(pPrefabMeshNode->GetMaterial(j));
				}

				FbxAMatrix matrix;
				matrix.SetTRS(instance.position, instance.rotation, instance.scale);
				pInstanceMeshNode->LclTranslation.Set(matrix.GetT());
				pInstanceMeshNode->LclRotation.Set(matrix.GetR());
				pInstanceMeshNode->LclScaling.Set(matrix.GetS());
				pScene->GetRootNode()->AddChild(pInstanceMeshNode);
			}
		}
		prefabIndex++;
	}

	// Export the scene to the file and destroy exporter.
	pExporter->Export(pScene);
	pExporter->Destroy();

}

