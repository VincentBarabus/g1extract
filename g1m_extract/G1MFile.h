#pragma once
class G1MFile
{
private:
	G1M_HEADER header;
	G1MSkeletonContainer skeletonContainer;
	G1MGeometryContainer geometryContainer;
	G1MMeshMatricesContainer meshMatricesContainer;
	G1TContainer textureSet;

	std::vector<FbxNode*> skeletonBoneNodes;
	std::vector<std::string> skeletonBoneNames;
	std::vector<FbxFileTexture*> textureMap;
	std::vector<FbxSurfacePhong *> materialMap;

public:
	void LoadMeshFromStream(std::ifstream& stream);
	void LoadSkeletonFromStream(std::ifstream& stream);
	void LoadTexturesFromStream(std::ifstream& stream);
	void LoadBoneNamesFromStream(std::ifstream& stream);

	void WriteToFBX(FbxNode* pRootNode);
	// write meshes only, used for parts in aditional files
	void WriteMeshesToFBX(FbxNode* pRootNode); // obsolete
	// build instances
	void BuildTexturesAndMatirials(FbxNode* pRootNode);
	void BuildBrefab(FbxNode* pMeshNode);
private:
	void BuildSkeleton(FbxNode* pParentNode);
	void BuildMeshes(FbxNode* pParentNode);
	void BuildMesh(FbxNode* pParentNode);
	// void BuildMesh2(FbxNode* pMeshNode); // << incorrect
	void BuildTextures(FbxNode* pMeshNode);
	void BuildMaterials(FbxNode* pParentNode);

};
