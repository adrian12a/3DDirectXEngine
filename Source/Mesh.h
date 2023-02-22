#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <vector>
#include <memory>
#include "Vertex.h"
#include "ConstantBufferStruct.h"
#include <DirectXMath.h>

using namespace DirectX;

#pragma comment(lib, "assimp-vc140-mt.lib")

class Mesh
{
public:
	Mesh(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext,
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice,
		Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer,
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer,
		std::vector<Vertex>& vertices, std::vector<DWORD>& indices,
		const DirectX::XMMATRIX& transformMatrix);
	Mesh(const Mesh& mesh);
	void Draw();
	void initVertexBuffer(Vertex* vertices, int size);
	void initIndexBuffer(DWORD* indices, int size);
	const DirectX::XMMATRIX& GetTransformMatrix();
private:
	std::shared_ptr<UINT> stride;
	UINT vertexBufferSize;
	UINT indexBufferSize;
	DirectX::XMMATRIX transformMatrix;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
};