#pragma once
#include "Model.h"
#include "GameObject.h"

class DrawableGameObject : public GameObject
{
public:
	DrawableGameObject(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext,
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice,
		Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer,
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer,
		Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer,
		const wchar_t* textureFilename, const std::string& modelFilename);
	void UpdateMatrix() override;
	void Draw(const XMMATRIX& viewProjectionMatrix);
	Model model;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
	XMMATRIX worldMatrix = XMMatrixIdentity();
	DirectX::XMFLOAT3 forward;
};
