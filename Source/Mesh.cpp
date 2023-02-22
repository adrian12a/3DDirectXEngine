#include "Mesh.h"

Mesh::Mesh(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext, Microsoft::WRL::ComPtr<ID3D11Device> pDevice, Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, std::vector<Vertex>& vertices, std::vector<DWORD>& indices, const DirectX::XMMATRIX& transformMatrix)
{
	this->vertexBuffer = vertexBuffer;
	this->indexBuffer = indexBuffer;
	this->pContext = pContext;
	this->pDevice = pDevice;
	this->transformMatrix = transformMatrix;
	initVertexBuffer(vertices.data(), vertices.size());
	initIndexBuffer(indices.data(), indices.size());
}

Mesh::Mesh(const Mesh& mesh)
{
	pContext = mesh.pContext;
	pDevice = mesh.pDevice;
	indexBuffer = mesh.indexBuffer;
	vertexBuffer = mesh.vertexBuffer;
	stride = mesh.stride;
	indexBufferSize = mesh.indexBufferSize;
	vertexBufferSize = mesh.vertexBufferSize;
	transformMatrix = mesh.transformMatrix;
}

void Mesh::Draw()
{
	UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), stride.get(), &offset);
	pContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	pContext->DrawIndexed(indexBufferSize, 0, 0);
}

void Mesh::initVertexBuffer(Vertex* vertices, int size)
{
	if (vertexBuffer.Get() != nullptr)
		vertexBuffer.Reset();

	vertexBufferSize = size;
	if (stride.get() == nullptr)
		stride = std::make_shared<UINT>(sizeof(Vertex));

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * size;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = vertices;

	pDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, vertexBuffer.GetAddressOf());
}

void Mesh::initIndexBuffer(DWORD* indices, int size)
{
	if (indexBuffer.Get() != nullptr)
		indexBuffer.Reset();

	indexBufferSize = size;

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * size;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA indexBufferData;
	indexBufferData.pSysMem = indices;
	pDevice->CreateBuffer(&indexBufferDesc, &indexBufferData, indexBuffer.GetAddressOf());
}

const DirectX::XMMATRIX& Mesh::GetTransformMatrix()
{
	return transformMatrix;
}