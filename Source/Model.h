#pragma once
#include "Mesh.h"

using namespace DirectX;

class Model
{
public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext,
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice,
		Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer,
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer,
		Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer,
		const std::string& filePath, ID3D11ShaderResourceView* texture);
	void SetTexture(ID3D11ShaderResourceView* texture);
	void Draw(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix);
private:
	std::vector<Mesh> meshes;
	bool LoadModel(const std::string& filePath);
	void ProcessNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransformMatrix);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& transformMatrix);

	ConstantBufferStruct constantBufferStruct;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	ID3D11ShaderResourceView* texture = nullptr;
};