#include "stdafx.h"
#include "G1MFile.h"


void G1MFile::WriteToFBX(FbxNode* pRootNode)
{
	BuildTextures(pRootNode);
	BuildMaterials(pRootNode);
	BuildSkeleton(pRootNode);
	// <<< for tests only
	FbxNode* pMeshNode = FbxNode::Create(pRootNode->GetScene(), "Mesh Node");
	pRootNode->AddChild(pMeshNode);
	// >>> for tests only
	BuildMesh(pMeshNode);
}

void G1MFile::WriteMeshesToFBX(FbxNode* pRootNode)
{
	BuildMeshes(pRootNode);
}

void G1MFile::BuildTexturesAndMatirials(FbxNode* pRootNode)
{
	BuildTextures(pRootNode);
}

void G1MFile::BuildBrefab(FbxNode* pMeshNode)
{
	BuildMaterials(pMeshNode);
	// BuildSkeleton(pRootNode);
	BuildMesh(pMeshNode);
}

void G1MFile::LoadMeshFromStream(std::ifstream& stream)
{
	stream.read(reinterpret_cast<char*>(&header), sizeof(G1M_HEADER));
	header.SwapBytes();

	for (DWORD i = 0; i < header.NumContainers; i++)
	{
		G1M_CONTAINER_HEADER containerHeader;
		stream.read(reinterpret_cast<char*>(&containerHeader), sizeof(G1M_CONTAINER_HEADER));
		containerHeader.SwapBytes();
		// Load mesh bone matrices
		if (containerHeader.Magic == ID_G1M_MESH_MATRICES_CONTAINER)
		{
			meshMatricesContainer.LoadFromStream(stream);
		}
		// Load mesh data
		else if (containerHeader.Magic == ID_G1M_GEOMETRY_CONTAINER)
		{
			geometryContainer.ReadFromStream(stream);
		}
		// Skip unused blocks
		else
		{
			stream.seekg(containerHeader.Size - sizeof(G1M_CONTAINER_HEADER), std::ios_base::cur);
		}
	}
}

void G1MFile::LoadSkeletonFromStream(std::ifstream& stream)
{
	stream.read(reinterpret_cast<char*>(&header), sizeof(G1M_HEADER));
	header.SwapBytes();

	for (DWORD i = 0; i < header.NumContainers; i++)
	{
		G1M_CONTAINER_HEADER containerHeader;
		stream.read(reinterpret_cast<char*>(&containerHeader), sizeof(G1M_CONTAINER_HEADER));
		containerHeader.SwapBytes();
		// Load skeleton data
		if (containerHeader.Magic == ID_G1M_SKELETON_CONTAINER)
		{
			skeletonContainer.ReadFromStream(stream);
		}
		// Skip unused blocks
		else
		{
			stream.seekg(containerHeader.Size - sizeof(G1M_CONTAINER_HEADER), std::ios_base::cur);
		}
	}
}

void G1MFile::LoadTexturesFromStream(std::ifstream& stream)
{
	textureSet.LoadFromStream(stream);
}

void G1MFile::LoadBoneNamesFromStream(std::ifstream& stream)
{
	char nameSize;
	stream.read(&nameSize, sizeof(char));
	while (nameSize > 0)
	{
		std::string name;
		name.resize(nameSize);
		stream.read(const_cast<char*>(name.data()), nameSize);
		skeletonBoneNames.push_back(name);
		stream.read(&nameSize, sizeof(char));
	}
}

void G1MFile::BuildMeshes(FbxNode* pParentNode)
{
	// for (auto& partsInfoBuffer : geometryContainer.partsInfoBuffers)
	auto& partsInfoBuffer = geometryContainer.partsInfoBuffers[0];
	{
		// FbxArray<FbxNode *> meshesArray;
		std::cout << "Creating model" << std::endl;
		for (auto& part : partsInfoBuffer.parts)
		{
			std::cout << "Creating part " << part.partsInfoEntryHeader.Name << std::endl;
			for (auto& meshIndex : part.meshInfoIndices)
			{
				std::cout << "Creating mesh " << meshIndex << std::endl;
				auto& meshInfo = geometryContainer.meshInfoIndices[meshIndex];
				auto& vertexDescriptorBuffer = geometryContainer.vertexDescriptorBuffers[meshInfo.VertexDescriptorBufferIndex];
				auto& vertexBuffers = geometryContainer.vertexBuffers;
				auto& indexBuffer = geometryContainer.indexBuffers[meshInfo.IndexBufferIndex];
				auto& meshMatricesInfoEntries = geometryContainer.meshMatricesInfoBuffers[meshInfo.MeshMatricesInfoIndex].meshMatricesInfoEntries;
				auto& textureRefs = geometryContainer.textureBuffers[meshInfo.MaterialIndex].textureRefs;

				// create meshnode and mesh
				FbxNode* pMeshNode = FbxNode::Create(pParentNode->GetScene(), ("Mesh " + std::to_string(meshIndex)).c_str());
				pParentNode->AddChild(pMeshNode);

				FbxMesh* pMesh = FbxMesh::Create(pParentNode->GetScene(), (part.partsInfoEntryHeader.Name + std::to_string(meshIndex)).c_str());
				pMeshNode->SetNodeAttribute(pMesh);
				pMeshNode->SetShadingMode(FbxNode::eTextureShading);
				// meshesArray.Add(pMeshNode);

				// process vertices
				pMesh->InitControlPoints(meshInfo.NumVertices);
				FbxVector4* pControlPoints = pMesh->GetControlPoints();
				FbxLayerElementNormal* pLayerElementNormal = NULL;
				FbxLayerElementVertexColor * pLayerElementVertexColor = NULL;
				std::vector<FbxLayerElementUV*> layerElementsUV;

				std::vector <VECTOR4> boneWeights;
				std::vector <BYTE4> boneIndices;

				for (DWORD vertexIndex = meshInfo.StartVertex; vertexIndex < (meshInfo.StartVertex + meshInfo.NumVertices); vertexIndex++)
				{
					for (auto& element : vertexDescriptorBuffer.Elements)
					{
						DWORD vertexBufferIndex = vertexDescriptorBuffer.VBIndices[element.StreamIndex];
						BYTE* vertexBufferData = vertexBuffers[vertexBufferIndex].rawData.data();
						DWORD VertexStride = vertexBuffers[vertexBufferIndex].vertexBufferHeader.VertexStride;
						switch (element.Usage)
						{
						case VERTEX_ATTRIBUTE_POSITION:
						{
							VECTOR3 position = *reinterpret_cast<VECTOR3*>(vertexBufferData + (VertexStride * vertexIndex) + element.Offset);
							position.SwapBytes();
							pControlPoints[vertexIndex - meshInfo.StartVertex] = position;
						}
						break;

						case VERTEX_ATTRIBUTE_BLENDWEIGHT:
						{
							VECTOR4 weights;
							switch (element.Format)
							{
							case ATTRIBUTE_FORMAT_FLOAT1:
							{
								float weights1 = *reinterpret_cast<float*>(vertexBufferData + (VertexStride * vertexIndex) + element.Offset);
								::SwapBytes(weights1);
								weights.x = weights1;
								weights.y = 0.0f;
								weights.z = 0.0f;
								weights.w = 0.0f;
							}
							break;
							case ATTRIBUTE_FORMAT_FLOAT2:
							{
								VECTOR2 weights2 = *reinterpret_cast<VECTOR2*>(vertexBufferData + (VertexStride * vertexIndex) + element.Offset);
								weights2.SwapBytes();
								weights.x = weights2.x;
								weights.y = weights2.y;
								weights.z = 0.0f;
								weights.w = 0.0f;
							}
							break;
							case ATTRIBUTE_FORMAT_FLOAT3:
							{
								VECTOR3 weights3 = *reinterpret_cast<VECTOR3*>(vertexBufferData + (VertexStride * vertexIndex) + element.Offset);
								weights3.SwapBytes();
								weights.x = weights3.x;
								weights.y = weights3.y;
								weights.z = weights3.z;
								weights.w = 0.0f;
							}
							break;
							case ATTRIBUTE_FORMAT_FLOAT4:
							{
								VECTOR4 weights4 = *reinterpret_cast<VECTOR4*>(vertexBufferData + (VertexStride * vertexIndex) + element.Offset);
								weights4.SwapBytes();
								weights.x = weights4.x;
								weights.y = weights4.y;
								weights.z = weights4.z;
								weights.w = weights4.w;
							}
							break;
							}
							boneWeights.push_back(weights);
						}
						break;

						case VERTEX_ATTRIBUTE_BLENDINDICES:
						{
							boneIndices.push_back(*reinterpret_cast<BYTE4*>(vertexBufferData + (VertexStride * vertexIndex) + element.Offset));
						}
						break;

						case VERTEX_ATTRIBUTE_NORMAL:
						{
							if (!pLayerElementNormal)
							{
								pLayerElementNormal = FbxLayerElementNormal::Create(pMesh, "Normals");
								pLayerElementNormal->SetMappingMode(FbxLayerElement::eByControlPoint);
								pLayerElementNormal->SetReferenceMode(FbxLayerElement::eDirect);
							}
							VECTOR3 normal = *reinterpret_cast<VECTOR3*>(vertexBufferData + (VertexStride * vertexIndex) + element.Offset);
							normal.SwapBytes();
							pLayerElementNormal->GetDirectArray().Add(normal);
						}
						break;

						case VERTEX_ATTRIBUTE_PSIZE:
							// nothing
							break;

						case VERTEX_ATTRIBUTE_TEXCOORD:
						{
							while (layerElementsUV.size() <= element.UsageIndex)
							{
								FbxLayerElementUV* elementUV = FbxLayerElementUV::Create(pMesh, ("UV" + std::to_string(element.UsageIndex)).c_str());
								elementUV->SetMappingMode(FbxLayerElement::eByControlPoint);
								elementUV->SetReferenceMode(FbxLayerElement::eDirect);
								layerElementsUV.push_back(elementUV);
							}
							VECTOR2 texCoord = *reinterpret_cast<VECTOR2*>(vertexBufferData + (VertexStride * vertexIndex) + element.Offset);
							texCoord.SwapBytes();
							texCoord.y *= -1.0f;
							layerElementsUV[element.UsageIndex]->GetDirectArray().Add(texCoord);
						}
						break;

						case VERTEX_ATTRIBUTE_TANGENT:
							// nothing
							break;

						case VERTEX_ATTRIBUTE_BINORMAL:
							// nothing
							break;

						case VERTEX_ATTRIBUTE_COLOR:
							if (!pLayerElementVertexColor)
							{
								pLayerElementVertexColor = FbxLayerElementVertexColor::Create(pMesh, "Colors");
								pLayerElementVertexColor->SetMappingMode(FbxLayerElement::eByControlPoint);
								pLayerElementVertexColor->SetReferenceMode(FbxLayerElement::eDirect);
							}
							BYTE4 color = *reinterpret_cast<BYTE4*>(vertexBufferData + (VertexStride * vertexIndex) + element.Offset);
							pLayerElementNormal->GetDirectArray().Add(FbxVector4(static_cast<double>(color.i3) / 256.0, static_cast<double>(color.i2) / 256.0, static_cast<double>(color.i1) / 256.0, static_cast<double>(color.i0) / 256.0));
							break;
						}

					}
				}
				if (pLayerElementNormal)
				{
					pMesh->GetLayer(pMesh->CreateLayer())->SetNormals(pLayerElementNormal);
				}
				for (auto& layerElementUV : layerElementsUV)
				{
					if (layerElementUV)
					{
						pMesh->GetLayer(pMesh->CreateLayer())->SetUVs(layerElementUV);
					}
				}
				if (pLayerElementVertexColor)
				{
					pMesh->GetLayer(pMesh->CreateLayer())->SetVertexColors(pLayerElementVertexColor);
				}
				// process indices
				for (DWORD i = meshInfo.StartIndex + 2; i < (meshInfo.StartIndex + meshInfo.NumIndices); i++)
				{
					TRIANGLE triangle;
					bool isEven = (i % 2 == 0);

					triangle.v0 = (indexBuffer.indices[i - 2]);
					triangle.v1 = (isEven ? indexBuffer.indices[i - 1] : indexBuffer.indices[i]);
					triangle.v2 = (isEven ? indexBuffer.indices[i] : indexBuffer.indices[i - 1]);

					if (triangle.v0 != triangle.v1 && triangle.v1 != triangle.v2 && triangle.v2 != triangle.v0)
					{
						pMesh->BeginPolygon();
						pMesh->AddPolygon(triangle.v0 - meshInfo.StartVertex);
						pMesh->AddPolygon(triangle.v1 - meshInfo.StartVertex);
						pMesh->AddPolygon(triangle.v2 - meshInfo.StartVertex);
						pMesh->EndPolygon();
					}
				}
				pMesh->BuildMeshEdgeArray();

				// add material
				pMeshNode->AddMaterial(materialMap[meshInfo.MaterialIndex]);

				// process skin
				if (boneIndices.size())
				{
					FbxGeometry* pMeshNodeAttribute = (FbxGeometry*)pMeshNode->GetNodeAttribute();
					FbxSkin* pSkin = FbxSkin::Create(pParentNode->GetScene(), "");
				
					std::vector<FbxCluster*> boneMap;
					for (auto& meshMatricesInfoEntry : meshMatricesInfoEntries)
					{
						FbxCluster *pCluster = FbxCluster::Create(pParentNode->GetScene(), "");
						pCluster->SetLink(skeletonBoneNodes[meshMatricesInfoEntry.BoneIndex]);
						pCluster->SetLinkMode(FbxCluster::eAdditive);
						pCluster->SetTransformMatrix(pMeshNode->EvaluateGlobalTransform());
						pCluster->SetTransformLinkMatrix(skeletonBoneNodes[meshMatricesInfoEntry.BoneIndex]->EvaluateGlobalTransform());
						pSkin->AddCluster(pCluster);
				
						boneMap.push_back(pCluster);
					}
				
					pMeshNodeAttribute->AddDeformer(pSkin);
					for (DWORD i = 0; i < meshInfo.NumVertices; i++)
					{
						auto& skinPair = std::pair<VECTOR4, BYTE4>(boneWeights[i], boneIndices[i]);
						if (skinPair.first.x > 0.0f)
						{
							boneMap[skinPair.second.i0 / 3]->AddControlPointIndex(i, skinPair.first.x);
						}
						if (skinPair.first.y > 0.0f)
						{
							boneMap[skinPair.second.i1 / 3]->AddControlPointIndex(i, skinPair.first.y);
						}
						if (skinPair.first.z > 0.0f)
						{
							boneMap[skinPair.second.i2 / 3]->AddControlPointIndex(i, skinPair.first.z);
						}
						if (skinPair.first.w > 0.0f)
						{
							boneMap[skinPair.second.i3 / 3]->AddControlPointIndex(i, skinPair.first.w);
						}
					}
				}
			}
		}
		// FbxGeometryConverter geometryConverter(pParentNode->GetFbxManager());
		// // geometryConverter.RemoveBadPolygonsFromMeshes(pParentNode->GetScene(), &meshesArray);
		// FbxNode* pMergedMesh;
		// try
		// {
		// 	pMergedMesh = geometryConverter.MergeMeshes(meshesArray, "Merged Mesh", pParentNode->GetScene());
		// 
		// }
		// catch (const std::exception& e)
		// {
		// 	std::cout << e.what;
		// }
		// pParentNode->AddChild(pMergedMesh);

	}
}

void G1MFile::BuildSkeleton(FbxNode* pParentNode)
{
	if (skeletonContainer.skeletonBones.size())
	{
		// create bones
		FbxPose* pPose = FbxPose::Create(pParentNode->GetScene(), "BindPose");
		for (DWORD i = 0; i < skeletonContainer.skeletonBones.size(); i++)
		{
			std::string name = skeletonBoneNames[i];
			FbxSkeleton* pBoneAttribute = FbxSkeleton::Create(pParentNode->GetScene(), name.c_str());

			if (skeletonContainer.skeletonBones[i].parentIndex < 0)
			{
				// Create skeleton root. 
				pBoneAttribute->SetSkeletonType(FbxSkeleton::eRoot);
			}
			else
			{
				// Create skeleton bone. 
				pBoneAttribute->SetSkeletonType(FbxSkeleton::eLimbNode);
			}

			pBoneAttribute->Size.Set(100.0);
			FbxNode* pBoneNode = FbxNode::Create(pParentNode->GetScene(), name.c_str());
			pBoneNode->SetNodeAttribute(pBoneAttribute);

			FbxAMatrix matrix;
			matrix.SetTQS(skeletonContainer.skeletonBones[i].transform, skeletonContainer.skeletonBones[i].rotation, skeletonContainer.skeletonBones[i].scale);
			pBoneNode->LclTranslation.Set(matrix.GetT());
			pBoneNode->LclRotation.Set(matrix.GetR());
			pBoneNode->LclScaling.Set(matrix.GetS());

			pPose->Add(pBoneNode, matrix);

			skeletonBoneNodes.push_back(pBoneNode);
		}
		// build skeleton hierarchy
		for (DWORD i = 0; i < skeletonContainer.skeletonBones.size(); i++)
		{
			if (skeletonContainer.skeletonBones[i].parentIndex < 0)
			{
				pParentNode->AddChild(skeletonBoneNodes[i]);
			}
			else
			{
				skeletonBoneNodes[skeletonContainer.skeletonBones[i].parentIndex]->AddChild(skeletonBoneNodes[i]);
			}
		}
		pParentNode->GetScene()->AddPose(pPose);
	}

}

void G1MFile::BuildTextures(FbxNode* pRootNode)
{
	DWORD i = 0;
	for (auto& texture : textureSet.textures)
	{
		std::string dirName = "tmp";
		dirName += ".textures";
		std::experimental::filesystem::create_directory(dirName);
		std::string fileName = dirName + "\\" + std::to_string(i) + ".dds";
		texture.SaveToDDS(fileName);

		FbxFileTexture* pTexture = FbxFileTexture::Create(pRootNode->GetScene(), ("Texture" + std::to_string(i)).c_str());

		// Set texture properties.
		pTexture->SetFileName(fileName.c_str()); // Resource file is in current directory.
		pTexture->SetTextureUse(FbxTexture::eStandard);
		pTexture->SetMappingType(FbxTexture::eUV);
		pTexture->SetMaterialUse(FbxFileTexture::eModelMaterial);
		pTexture->SetSwapUV(false);
		pTexture->SetTranslation(0.0, 0.0);
		pTexture->SetScale(1.0, 1.0);
		pTexture->SetRotation(0.0, 0.0);
		pTexture->UVSet.Set("UV0"); // Connect texture to the proper UV

		textureMap.push_back(pTexture);
		i++;
	}

}

void G1MFile::BuildMaterials(FbxNode* pParentNode)
{
	materialMap.clear();
	DWORD i = 0;
	for (auto& material : geometryContainer.textureBuffers)
	{
		FbxSurfacePhong *pMaterial = FbxSurfacePhong::Create(pParentNode->GetScene(), ("material " + std::to_string(i)).c_str());
		pMaterial->Diffuse.ConnectSrcObject(textureMap[material.textureRefs[0].TextureIndex]);
		// if (material.textureRefs[0].TextureIndex >= textureMap.size()) // hack
		// {
		// 	// incorrect maping
		// 	pMaterial->Diffuse.ConnectSrcObject(textureMap[0]);
		// }
		// else
		// {
		// 	pMaterial->Diffuse.ConnectSrcObject(textureMap[material.textureRefs[0].TextureIndex]);
		// }
		materialMap.push_back(pMaterial);
		i++;
	}
}

void G1MFile::BuildMesh(FbxNode* pMeshNode)
{
	// for (auto& partsInfoBuffer : geometryContainer.partsInfoBuffers)
	auto& partsInfoBuffer = geometryContainer.partsInfoBuffers[0];
	{
		DWORD poligonIndex = 0;
		std::cout << "Creating model" << std::endl;
		// create meshnode and mesh
		FbxMesh* pMesh = FbxMesh::Create(pMeshNode->GetScene(), (std::string(pMeshNode->GetName()) + "mesh").c_str());
		pMeshNode->SetNodeAttribute(pMesh);
		pMeshNode->SetShadingMode(FbxNode::eTextureShading);

		// vertex buffer
		std::vector <VECTOR3> positions;
		std::vector <VECTOR3> normals;
		std::vector <VECTOR2> texCoords;
		std::vector <VECTOR4> boneWeights;
		std::vector <BYTE4> boneIndices;
		std::vector <BYTE4> boneTransformIndices;
		std::vector <DWORD> materialIndices;
		DWORD previousVBSize = 0;
		// index buffer
		std::vector<TRIANGLE> triangles;

		// process vertices
		FbxLayerElementNormal* pLayerElementNormal = FbxLayerElementNormal::Create(pMesh, ("Normals"));
		pLayerElementNormal->SetMappingMode(FbxLayerElement::eByControlPoint);
		pLayerElementNormal->SetReferenceMode(FbxLayerElement::eDirect);
		pMesh->GetLayer(pMesh->CreateLayer())->SetNormals(pLayerElementNormal);


		FbxLayerElementUV* pLayerElementUV = FbxLayerElementUV::Create(pMesh, ("UV0"));
		pLayerElementUV->SetMappingMode(FbxLayerElement::eByControlPoint);
		pLayerElementUV->SetReferenceMode(FbxLayerElement::eDirect);
		pMesh->GetLayer(pMesh->CreateLayer())->SetUVs(pLayerElementUV);

		FbxLayerElementMaterial* pLayerElementMaterial = FbxLayerElementMaterial::Create(pMesh, ("Materials"));
		pLayerElementMaterial->SetMappingMode(FbxLayerElement::eByPolygon);
		pLayerElementMaterial->SetReferenceMode(FbxLayerElement::eIndexToDirect);
		pMesh->GetLayer(pMesh->CreateLayer())->SetMaterials(pLayerElementMaterial);

		for (auto& material : materialMap)
		{
			pMeshNode->AddMaterial(material);
		}

		for (auto& part : partsInfoBuffer.parts)
		{
			std::cout << "Creating part " << part.partsInfoEntryHeader.Name << std::endl;
			for (auto& meshIndex : part.meshInfoIndices)
			{
				std::cout << "Creating mesh " << meshIndex << std::endl;
				auto& meshInfo = geometryContainer.meshInfoIndices[meshIndex];
				auto& vertexDescriptorBuffer = geometryContainer.vertexDescriptorBuffers[meshInfo.VertexDescriptorBufferIndex];
				auto& vertexBuffers = geometryContainer.vertexBuffers;
				auto& indexBuffer = geometryContainer.indexBuffers[meshInfo.IndexBufferIndex];
				auto& meshMatricesInfoEntries = geometryContainer.meshMatricesInfoBuffers[meshInfo.MeshMatricesInfoIndex].meshMatricesInfoEntries;

				// bone map for skin
				std::vector<DWORD> boneMap;
				for (auto& meshMatricesInfoEntry : meshMatricesInfoEntries)
				{
					boneMap.push_back(meshMatricesInfoEntry.BoneIndex);
				}

				for (DWORD vertexIndex = meshInfo.StartVertex; vertexIndex < (meshInfo.StartVertex + meshInfo.NumVertices); vertexIndex++)
				{
					materialIndices.push_back(meshInfo.MaterialIndex);
					for (auto& element : vertexDescriptorBuffer.Elements)
					{
						DWORD vertexBufferIndex = vertexDescriptorBuffer.VBIndices[element.StreamIndex];
						BYTE* vertexBufferData = vertexBuffers[vertexBufferIndex].rawData.data();
						DWORD vertexStride = vertexBuffers[vertexBufferIndex].vertexBufferHeader.VertexStride;
						switch (element.Usage)
						{
							case VERTEX_ATTRIBUTE_POSITION:
							{
								VECTOR3 position = *reinterpret_cast<VECTOR3*>(vertexBufferData + (vertexStride * vertexIndex) + element.Offset);
								position.SwapBytes();
								positions.push_back(position);
							}
							break;

							case VERTEX_ATTRIBUTE_BLENDWEIGHT:
							{
								VECTOR4 weights;
								switch (element.Format)
								{
								case ATTRIBUTE_FORMAT_FLOAT1:
								{
									float weights1 = *reinterpret_cast<float*>(vertexBufferData + (vertexStride * vertexIndex) + element.Offset);
									::SwapBytes(weights1);
									weights.x = weights1;
									weights.y = 0.0f;
									weights.z = 0.0f;
									weights.w = 0.0f;
								}
								break;
								case ATTRIBUTE_FORMAT_FLOAT2:
								{
									VECTOR2 weights2 = *reinterpret_cast<VECTOR2*>(vertexBufferData + (vertexStride * vertexIndex) + element.Offset);
									weights2.SwapBytes();
									weights.x = weights2.x;
									weights.y = weights2.y;
									weights.z = 0.0f;
									weights.w = 0.0f;
								}
								break;
								case ATTRIBUTE_FORMAT_FLOAT3:
								{
									VECTOR3 weights3 = *reinterpret_cast<VECTOR3*>(vertexBufferData + (vertexStride * vertexIndex) + element.Offset);
									weights3.SwapBytes();
									weights.x = weights3.x;
									weights.y = weights3.y;
									weights.z = weights3.z;
									weights.w = 0.0f;
								}
								break;
								case ATTRIBUTE_FORMAT_FLOAT4:
								{
									VECTOR4 weights4 = *reinterpret_cast<VECTOR4*>(vertexBufferData + (vertexStride * vertexIndex) + element.Offset);
									weights4.SwapBytes();
									weights.x = weights4.x;
									weights.y = weights4.y;
									weights.z = weights4.z;
									weights.w = weights4.w;
								}
								break;
								}
								boneWeights.push_back(weights);
							}
							break;

							case VERTEX_ATTRIBUTE_BLENDINDICES:
							{
								BYTE4 boneIndicesElement = *reinterpret_cast<BYTE4*>(vertexBufferData + (vertexStride * vertexIndex) + element.Offset);
								boneIndicesElement.i0 = (BYTE)boneMap[boneIndicesElement.i0 / 3];
								boneIndicesElement.i1 = (BYTE)boneMap[boneIndicesElement.i1 / 3];
								boneIndicesElement.i2 = (BYTE)boneMap[boneIndicesElement.i2 / 3];
								boneIndicesElement.i3 = (BYTE)boneMap[boneIndicesElement.i3 / 3];
								boneIndices.push_back(boneIndicesElement);
							}
							break;

							case VERTEX_ATTRIBUTE_NORMAL:
							{
								VECTOR3 normal = *reinterpret_cast<VECTOR3*>(vertexBufferData + (vertexStride * vertexIndex) + element.Offset);
								normal.SwapBytes();
								normals.push_back(normal);
							}
							break;

							case VERTEX_ATTRIBUTE_PSIZE:
								// nothing
								break;

							case VERTEX_ATTRIBUTE_TEXCOORD:
							{
								if (element.UsageIndex == 0)
								{
									VECTOR2 texCoord = *reinterpret_cast<VECTOR2*>(vertexBufferData + (vertexStride * vertexIndex) + element.Offset);
									texCoord.SwapBytes();
									texCoord.y = -texCoord.y;
									texCoords.push_back(texCoord);
								}
							}
							break;

							case VERTEX_ATTRIBUTE_TANGENT:
							{
								// nothing
							}
							break;

							case VERTEX_ATTRIBUTE_BINORMAL:
							{
								// nothing
							}
							break;

							case VERTEX_ATTRIBUTE_COLOR:
							{
								// nothing
								break;
							}
						}

					}
				}

				for (DWORD i = meshInfo.StartIndex + 2; i < (meshInfo.StartIndex + meshInfo.NumIndices); i++)
				{
					TRIANGLE triangle;
					bool isEven = (i % 2 == 0);

					triangle.v0 = (indexBuffer.indices[i - 2]);
					triangle.v1 = (isEven ? indexBuffer.indices[i - 1] : indexBuffer.indices[i]);
					triangle.v2 = (isEven ? indexBuffer.indices[i] : indexBuffer.indices[i - 1]);

					if (triangle.v0 != triangle.v1 && triangle.v1 != triangle.v2 && triangle.v2 != triangle.v0)
					{
						pMesh->BeginPolygon(-1, -1, -1, false);
						pMesh->AddPolygon(triangle.v0 + previousVBSize - meshInfo.StartVertex);
						pMesh->AddPolygon(triangle.v1 + previousVBSize - meshInfo.StartVertex);
						pMesh->AddPolygon(triangle.v2 + previousVBSize - meshInfo.StartVertex);
						pMesh->EndPolygon();
						pLayerElementMaterial->GetIndexArray().Add(meshInfo.MaterialIndex);
						poligonIndex++;
					}
				}
				previousVBSize = positions.size();
			}
		}

		pMesh->InitControlPoints(positions.size());
		FbxVector4* pControlPoints = pMesh->GetControlPoints();
		for (DWORD i = 0; i < positions.size(); i++)
		{
			pControlPoints[i] = positions[i];
			pLayerElementNormal->GetDirectArray().Add(normals[i]);
			pLayerElementUV->GetDirectArray().Add(texCoords[i]);
		}

		pMesh->BuildMeshEdgeArray();

		// process skin
		if (boneIndices.size())
		{
			assert(positions.size() == boneIndices.size());
			FbxGeometry* pMeshNodeAttribute = (FbxGeometry*)pMeshNode->GetNodeAttribute();
			FbxSkin* pSkin = FbxSkin::Create(pMeshNode->GetScene(), "Mesh Skin");
			pMeshNodeAttribute->AddDeformer(pSkin);

			std::map<DWORD, FbxCluster*> clusterMap;
			for (DWORD i = 0; i < positions.size(); i++)
			{
				auto& skinPair = std::pair<VECTOR4, BYTE4>(boneWeights[i], boneIndices[i]);
				if (skinPair.first.x > 0.0f)
				{
					if (clusterMap[skinPair.second.i0])
					{
						clusterMap[skinPair.second.i0]->AddControlPointIndex(i, skinPair.first.x);
					}
					else
					{
						FbxCluster *pCluster = FbxCluster::Create(pMeshNode->GetScene(), "");
						pCluster->SetLink(skeletonBoneNodes[skinPair.second.i0]);
						pCluster->SetLinkMode(FbxCluster::eAdditive);
						pCluster->SetTransformMatrix(pMeshNode->EvaluateGlobalTransform());
						pCluster->SetTransformLinkMatrix(skeletonBoneNodes[skinPair.second.i0]->EvaluateGlobalTransform());
						pSkin->AddCluster(pCluster);

						clusterMap[skinPair.second.i0] = pCluster;
						clusterMap[skinPair.second.i0]->AddControlPointIndex(i, skinPair.first.x);
					}
				}
				if (skinPair.first.y > 0.0f)
				{
					if (clusterMap[skinPair.second.i1])
					{
						clusterMap[skinPair.second.i1]->AddControlPointIndex(i, skinPair.first.y);
					}
					else
					{
						FbxCluster *pCluster = FbxCluster::Create(pMeshNode->GetScene(), "");
						pCluster->SetLink(skeletonBoneNodes[skinPair.second.i1]);
						pCluster->SetLinkMode(FbxCluster::eAdditive);
						pCluster->SetTransformMatrix(pMeshNode->EvaluateGlobalTransform());
						pCluster->SetTransformLinkMatrix(skeletonBoneNodes[skinPair.second.i1]->EvaluateGlobalTransform());
						pSkin->AddCluster(pCluster);

						clusterMap[skinPair.second.i1] = pCluster;
						clusterMap[skinPair.second.i1]->AddControlPointIndex(i, skinPair.first.y);
					}
				}
				if (skinPair.first.z > 0.0f)
				{
					if (clusterMap[skinPair.second.i2])
					{
						clusterMap[skinPair.second.i2]->AddControlPointIndex(i, skinPair.first.z);
					}
					else
					{
						FbxCluster *pCluster = FbxCluster::Create(pMeshNode->GetScene(), "");
						pCluster->SetLink(skeletonBoneNodes[skinPair.second.i2]);
						pCluster->SetLinkMode(FbxCluster::eAdditive);
						pCluster->SetTransformMatrix(pMeshNode->EvaluateGlobalTransform());
						pCluster->SetTransformLinkMatrix(skeletonBoneNodes[skinPair.second.i2]->EvaluateGlobalTransform());
						pSkin->AddCluster(pCluster);

						clusterMap[skinPair.second.i2] = pCluster;
						clusterMap[skinPair.second.i2]->AddControlPointIndex(i, skinPair.first.z);
					}
				}
				if (skinPair.first.w > 0.0f)
				{
					if (clusterMap[skinPair.second.i3])
					{
						clusterMap[skinPair.second.i3]->AddControlPointIndex(i, skinPair.first.w);
					}
					else
					{
						FbxCluster *pCluster = FbxCluster::Create(pMeshNode->GetScene(), "");
						pCluster->SetLink(skeletonBoneNodes[skinPair.second.i3]);
						pCluster->SetLinkMode(FbxCluster::eAdditive);
						pCluster->SetTransformMatrix(pMeshNode->EvaluateGlobalTransform());
						pCluster->SetTransformLinkMatrix(skeletonBoneNodes[skinPair.second.i3]->EvaluateGlobalTransform());
						pSkin->AddCluster(pCluster);

						clusterMap[skinPair.second.i3] = pCluster;
						clusterMap[skinPair.second.i3]->AddControlPointIndex(i, skinPair.first.w);
					}
				}
			}
		}

	}
}
