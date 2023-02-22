#pragma once
#include <DirectXMath.h>

struct LightBufferStruct
{
	DirectX::XMFLOAT3 ambientLightColor;
	float ambientLightStrength;

	DirectX::XMFLOAT3 LightColor;
	float LightStrength;
	DirectX::XMFLOAT3 LightPosition;

	float LightAttenuation_a;
	float LightAttenuation_b;
	float LightAttenuation_c;
};