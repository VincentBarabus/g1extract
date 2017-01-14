// g1m_extract.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int main(int argc, CHAR* argv[])
{
	/* files in base elixir file of location
	SCENE_NAME.ktf
	SCENE_NAME.g1s - shaders
	SCENE_NAME.g1t - texture pack for inscene objects
	SCENE_NAME_pat.g1t
	SCENE_NAME.opl2 - scanegraph
	SCENE_NAME.khm
	SCENE_NAME_col.g1co - collision model for scene
	LAND_MODEL_final_output.g1t
	SCENE_NAME_colchara.g1co - collision model for scene mobs
	*/
	std::string sceneFileName = "E:\\ProjectsData\\BLJM61264-[Yoru No Nai Kuni]\\PS3_GAME\\USRDIR\\Data\\PS3\\Field\\BOSS_G02\\BOSS_G02.elixir";
	std::string sceneObjectsFileName = sceneFileName.substr(0, sceneFileName.length() - 7) + "_Obj.elixir";
	std::string fbxFilename = "C:\\Users\\Barabus\\Documents\\3dsMax\\import\\file.fbx";
	ElixirFile scene;
	ElixirFile sceneObjects;
	scene.LoadFromFile(sceneFileName);
	sceneObjects.LoadFromFile(sceneObjectsFileName);

	OPL2Container opl2Contaiter;
	// std::ifstream oplFile(scenegraphFileName, std::ios_base::binary);
	opl2Contaiter.LoadFromStream(scene.GetStreamAt(4));
	opl2Contaiter.ExportContentToFBX(scene, sceneObjects, fbxFilename);

}
/*
int main(int argc, CHAR* argv[])
{
	// OPL2Container opl2Contaiter;
	// opl2Contaiter.LoadFromStream(std::ifstream("", std::ios_base::binary));
	// Create the FBX SDK manager
	FbxManager* lSdkManager = FbxManager::Create();

	// Create an IOSettings object.
	FbxIOSettings * ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	ios->SetBoolProp(EXP_FBX_EMBEDDED, true);
	lSdkManager->SetIOSettings(ios);

	// ... Configure the FbxIOSettings object here ...

	// Create an exporter.
	FbxExporter* lExporter = FbxExporter::Create(lSdkManager, "");

	// Declare the path and filename of the file to which the scene will be exported.
	// In this case, the file will be in the same directory as the executable.
	const char* lFilename = "C:\\Users\\Barabus\\Documents\\3dsMax\\import\\file.fbx";

	// Initialize the exporter.
	bool lExportStatus = lExporter->Initialize(lFilename, -1, lSdkManager->GetIOSettings());
	lExporter->SetFileExportVersion(FBX_2014_00_COMPATIBLE);

	if (!lExportStatus)
	{
		printf("Call to FbxExporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lExporter->GetStatus().GetErrorString());
		return -1;
	}

	// Create a new scene so it can be populated by the imported file.
	FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");

	// ... Import a scene, or build a new one ...
	G1MFile loader;
	loader.LoadBoneNamesFromStream(std::ifstream("PC00A_MODELOid.bin", std::ios_base::binary));
	loader.LoadSkeletonFromStream(std::ifstream("PC00A_MODEL.g1m", std::ios_base::binary));
	loader.LoadTexturesFromStream(std::ifstream("PC00A_MODEL.g1t", std::ios_base::binary));
	loader.LoadMeshFromStream(std::ifstream("PC00A_MODEL_default.g1m", std::ios_base::binary));
	loader.WriteToFBX(lScene->GetRootNode());

	// Export the scene to the file.
	lExporter->Export(lScene);
	// Destroy the exporter.
	lExporter->Destroy();


	return 0;
}
//*/