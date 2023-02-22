#include "DrawableGameObject.h"
#include <WICTextureLoader.h>

DrawableGameObject::DrawableGameObject(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext, Microsoft::WRL::ComPtr<ID3D11Device> pDevice, Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, const wchar_t* textureFilename, const std::string& modelFilename)
{
	DirectX::CreateWICTextureFromFile(pDevice.Get(), textureFilename, nullptr, texture.GetAddressOf());
	model.Initialize(pContext.Get(), pDevice.Get(), indexBuffer.Get(), vertexBuffer.Get(), constantBuffer.Get(), modelFilename, texture.Get());
	SetPosition(0.0f, 0.0f, 0.0f);
	SetRotation(0.0f, 0.0f, 0.0f);
	UpdateMatrix();
}

void DrawableGameObject::Draw(const XMMATRIX& viewProjectionMatrix)
{
	model.Draw(worldMatrix, viewProjectionMatrix);
}

void DrawableGameObject::UpdateMatrix()
{
	worldMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z) * XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z);
	UpdateDirectionVectors();
}