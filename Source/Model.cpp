#include "Model.h"

bool Model::Initialize(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext, Microsoft::WRL::ComPtr<ID3D11Device> pDevice, Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, const std::string& filePath, ID3D11ShaderResourceView* texture)
{
	this->pContext = pContext;
	this->pDevice = pDevice;
	this->constantBuffer = constantBuffer;
	this->vertexBuffer = vertexBuffer;
	this->indexBuffer = indexBuffer;
	this->texture = texture;

	if (!this->LoadModel(filePath))
		return false;

	return true;
}

void Model::SetTexture(ID3D11ShaderResourceView* texture)
{
	this->texture = texture;
}

void Model::Draw(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix)
{
	pContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
	for (int i = 0; i < meshes.size(); i++)
	{
		constantBufferStruct.MVP = meshes[i].GetTransformMatrix() * worldMatrix * viewProjectionMatrix;
		constantBufferStruct.worldMVP = meshes[i].GetTransformMatrix() * worldMatrix;

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		pContext->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		CopyMemory(mappedResource.pData, &constantBufferStruct, sizeof(ConstantBufferStruct));
		pContext->Unmap(constantBuffer.Get(), 0);

		pContext->PSSetShaderResources(0, 1, &this->texture); //Set Texture

		meshes[i].Draw();
	}
}

bool Model::LoadModel(const std::string& filePath)
{
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(filePath,
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded);

	if (pScene == nullptr)
		return false;

	this->ProcessNode(pScene->mRootNode, pScene, DirectX::XMMatrixIdentity());
	return true;
}

void Model::ProcessNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransformMatrix)
{
	XMMATRIX nodeTransformMatrix = XMMatrixTranspose(XMMATRIX(&node->mTransformation.a1)) * parentTransformMatrix;

	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(this->ProcessMesh(mesh, scene, nodeTransformMatrix));
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessNode(node->mChildren[i], scene, nodeTransformMatrix);
	}
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& transformMatrix)
{
	// Data to fill
	std::vector<Vertex> vertices;
	std::vector<DWORD> indices;

	//Get vertices
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		vertex.x = mesh->mVertices[i].x;
		vertex.y = mesh->mVertices[i].y;
		vertex.z = mesh->mVertices[i].z;

		vertex.normalX = mesh->mNormals[i].x;
		vertex.normalY = mesh->mNormals[i].y;
		vertex.normalZ = mesh->mNormals[i].z;

		if (mesh->mTextureCoords[0])
		{
			vertex.u = (float)mesh->mTextureCoords[0][i].x;
			vertex.v = (float)mesh->mTextureCoords[0][i].y;
		}

		vertices.push_back(vertex);
	}

	//Get indices
	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	return Mesh(pContext.Get(), pDevice.Get(), indexBuffer.Get(), vertexBuffer.Get(), vertices, indices, transformMatrix);
}