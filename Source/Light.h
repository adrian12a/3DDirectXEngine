#pragma once
#include "LightBufferStruct.h"
#include <d3d11.h>
#include <wrl.h>

class Light
{
public:
	Light();
	Light(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext, Microsoft::WRL::ComPtr<ID3D11Buffer> lightBuffer);
	void setLight(LightBufferStruct light);
	void setBuffer();
private:
	LightBufferStruct lightStruct;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11Buffer> lightBuffer;
};
