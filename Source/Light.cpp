#include "Light.h"

Light::Light()
{
}

Light::Light(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext, Microsoft::WRL::ComPtr<ID3D11Buffer> lightBuffer)
{
	this->pContext = pContext;
	this->lightBuffer = lightBuffer;
}

void Light::setLight(LightBufferStruct light)
{
	lightStruct = light;
}

void Light::setBuffer()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	pContext->Map(lightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &lightStruct, sizeof(LightBufferStruct));
	pContext->Unmap(lightBuffer.Get(), 0);
}